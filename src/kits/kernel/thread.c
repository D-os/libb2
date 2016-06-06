#include <OS.h>

#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <linux/futex.h>
#include <assert.h>

#include "private.h"

static _thread_info _threads[256];

static void _thread_init(int argc, char* argv[], char* envp[])
{
    _thread_info *info = _threads;
    info->thread = pthread_self();
    info->task_state = TASK_RUNNING;
    prctl(PR_GET_NAME, (unsigned long) info->name, 0, 0, 0);
    pthread_attr_t attr;
    pthread_getattr_np(info->thread, &attr);
    size_t size;
    pthread_attr_getstack(&attr, &info->stack_base, &size);
    info->stack_end = info->stack_base + size;
}
__attribute__((section(".init_array"))) void (* p_thread_init)(int,char*[],char*[]) = &_thread_init;

_thread_info *_find_thread_info(thread_id thread)
{
    _thread_info *info = _threads;
    _thread_info *fin = _threads + B_COUNT_OF(_threads);
    while (info < fin){
        if (info->thread == thread) {
            return info;
        }
        info++;
    }
    return NULL;
}

static _thread_info *_acquire_thread_info()
{
    _thread_info *info = _threads;
    _thread_info *fin = _threads + B_COUNT_OF(_threads);
    while (info < fin){
        if (cmpxchg(&info->thread, 0, -1) == 0) {
            return info;
        }
        info++;
    }
    return NULL;
}

static void _sigaction_handler(int sig)
{
    if (sig == SIGTERM) {
        pthread_exit((void*)(intptr_t)(0x80 + sig));
    }

    _thread_info *info = _find_thread_info(pthread_self());
    assert(info != NULL);

    if (sig == SIGCONT && info->task_state < TASK_RUNNING) {
        info->task_state = TASK_RUNNING;
    }
}

static void* _thread_wrapper(void *arg)
{
    _thread_info *info = (_thread_info *)arg;

    struct sigaction sa;
    sigfillset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = _sigaction_handler;
    sigaction(SIGCONT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    prctl(PR_SET_NAME, (unsigned long) info->name, 0, 0, 0);

    pthread_attr_t attr;
    pthread_getattr_np(info->thread, &attr);
    size_t size;
    pthread_attr_getstack(&attr, &info->stack_base, &size);
    info->stack_end = info->stack_base + size;

    if (cmpxchg(&info->task_state, TASK_NEW, TASK_WAITING) == TASK_NEW) {
        sigset_t wait_mask;
        sigfillset(&wait_mask);
        sigdelset(&wait_mask, SIGCONT);
        sigdelset(&wait_mask, SIGTERM);
        do {
            sigsuspend(&wait_mask);
        }
        while (info->task_state == TASK_WAITING);
    }

    status_t exit = info->func(info->data);
    info->task_state = TASK_EXITED;
    return (void*)(intptr_t)exit;
}


thread_id spawn_thread(thread_func func, const char *name, int32 priority, void *data)
{
    pthread_attr_t attr;
    _thread_info *info = _acquire_thread_info();

    if (info == NULL) {
        return B_NO_MORE_THREADS;
    }

    info->task_state = TASK_NEW;

    if (name) {
        strncpy(info->name, name, B_OS_NAME_LENGTH);
    }
    else {
        info->name[0] = 0;
    }

    if (pthread_attr_init(&attr) != 0) {
        return B_NO_MEMORY;
    }

    if (pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED) != 0) {
        return B_NO_MORE_THREADS;
    }

    if (priority > 99) {
        if (pthread_attr_setschedpolicy(&attr, SCHED_RR) != 0) {
            return B_NO_MORE_THREADS;
        }
        struct sched_param schedparam;
        schedparam.sched_priority = priority - 99;
        if (schedparam.sched_priority > 99) schedparam.sched_priority = 99;
        if (pthread_attr_setschedparam(&attr, &schedparam) != 0) {
            return B_NO_MORE_THREADS;
        }
    }
    else {
        /* FIXME! use BFS SCHED_ scheduling! */
        if (pthread_attr_setschedpolicy(&attr, SCHED_OTHER) != 0) {
            return B_NO_MORE_THREADS;
        }
    }
    info->priority = priority;

    info->func = func;
    info->data = data;

    info->has_data = 0;

    /* finally actually kick the thread */
    int s = pthread_create(&info->thread, &attr, _thread_wrapper, info);
    if (s != 0) {
        info->thread = 0;
        switch (s) {
        case EAGAIN:
            return B_NO_MORE_THREADS;
        case ENOMEM:
            return B_NO_MEMORY;
        case EPERM:
            return B_PERMISSION_DENIED;
        default:
            return B_FROM_POSIX_ERROR(s);
        }
    }

    pthread_attr_destroy(&attr);

    return info->thread;
}

status_t wait_for_thread(thread_id thread, status_t* exit_value)
{
    _thread_info *info = _find_thread_info(thread);

    if (info == NULL) {
        return B_BAD_THREAD_ID;
    }

    if (info->task_state != TASK_RUNNING) {
        status_t status = resume_thread(thread);
        if (status != B_OK) {
            return status;
        }
    }

    void *exit = 0;
    int s = pthread_join(info->thread, &exit);
    if (s != 0) {
        switch (s) {
        case EINTR:
        case EDEADLK:
            return B_INTERRUPTED;
        case EINVAL:
        case ESRCH:
            return B_BAD_THREAD_ID;
        default:
            return B_FROM_POSIX_ERROR(s);
        }
    }

    _task_state state = info->task_state;
    info->thread = 0; /* free task structure for reuse */
    *exit_value = (intptr_t)exit;
    return state == TASK_EXITED ? B_OK : B_INTERRUPTED;
}

thread_id find_thread(const char* name)
{
    if (name == NULL) {
        return pthread_self();
    }

    _thread_info *info = _threads;
    _thread_info *end = _threads + sizeof(_threads)/sizeof(_threads[0]);
    while (info < end){
        if (strncmp(info->name, name, B_OS_NAME_LENGTH) == 0) {
            return info->thread;
        }
        info++;
    }
    return B_NAME_NOT_FOUND;
}

status_t kill_thread(thread_id thread)
{
    _thread_info *info = _find_thread_info(thread);
    if (info == NULL || info->task_state == TASK_EXITED) {
        return B_BAD_THREAD_ID;
    }

    return B_FROM_POSIX_ERROR(pthread_kill(info->thread, SIGTERM));
}

status_t resume_thread(thread_id thread)
{
    _thread_info *info = _find_thread_info(thread);
    if (info == NULL) {
        return B_BAD_THREAD_ID;
    }

    if (cmpxchg(&info->task_state, TASK_NEW, TASK_RUNNING) == TASK_NEW) {
        return B_OK;
    }

    return B_FROM_POSIX_ERROR(pthread_kill(info->thread, SIGCONT));
}

status_t suspend_thread(thread_id thread)
{
    /* suspending thread is not supported */
    abort();
    return B_BAD_THREAD_STATE;
}

inline void exit_thread(status_t status)
{
    _thread_info *info = _find_thread_info(pthread_self());
    assert(info != NULL);
    info->task_state = TASK_EXITED;
    pthread_exit(&status);
}

status_t kill_team(team_id team)
{
    if (kill(team, SIGKILL) != 0) {
        switch (errno) {
        case ESRCH:
            return B_BAD_TEAM_ID;
        default:
            return B_FROM_POSIX_ERROR(errno);
        }
    }
    return B_OK;
}

bool has_data(thread_id thread)
{
    _thread_info *info = _find_thread_info(thread);
    return info && info->has_data;
}

status_t send_data(thread_id thread, int32 code, const void *buffer, size_t bufferSize)
{
    int c;

    _thread_info *info = _find_thread_info(thread);

    if (info == NULL) {
        return B_BAD_THREAD_ID;
    }


    while ((c = cmpxchg(&info->has_data, 0, 1))) {
        if (syscall(SYS_futex, &info->has_data, FUTEX_WAIT_PRIVATE, c, NULL, NULL, 0) != 0) {
            switch (errno) {
            case EINTR:
                return B_INTERRUPTED;
            default:
                return B_FROM_POSIX_ERROR(errno);
            }
        }
    }

    /* at this point c (info->has_data) is 1, meaning someone is either consuming or producing message */
    /* let's produce */
    if (buffer && bufferSize) {
        info->data_buffer = mmap(NULL, bufferSize,
                                 PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
                                 -1, 0);
        if (info->data_buffer == MAP_FAILED) {
            switch (errno) {
            case EAGAIN:
            case EINVAL:
            case ENFILE:
            case ENOMEM:
                return B_NO_MEMORY;
            default:
                return B_FROM_POSIX_ERROR(errno);
            }
        }
        memcpy(info->data_buffer, buffer, bufferSize);
    }
    else {
        info->data_buffer = NULL;
    }
    info->data_buffer_size = bufferSize;
    info->data_code = code;
    info->data_sender = pthread_self();
    c = cmpxchg(&info->has_data, 1, 2); /* mark as ready to read */
    assert(c == 1);
    if (syscall(SYS_futex, &info->has_data, FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0) != 0) {
        switch (errno) {
        case EINTR:
            return B_INTERRUPTED;
        default:
            return B_FROM_POSIX_ERROR(errno);
        }
    }
    return B_OK;
}

int32 receive_data(thread_id *sender, void *buffer, size_t bufferSize)
{
    int c;

    _thread_info *info = _find_thread_info(pthread_self());
    assert(info != NULL);

    info->state = B_THREAD_RECEIVING;

    while ((c = cmpxchg(&info->has_data, 2, 1)) != 2) {
        if (syscall(SYS_futex, &info->has_data, FUTEX_WAIT_PRIVATE, c, NULL, NULL, 0) != 0) {
            return B_INTERRUPTED;
        }
    }

    assert(sender != NULL);
    *sender = info->data_sender;
    if (buffer && bufferSize) {
        memcpy(buffer, info->data_buffer, min_c(bufferSize, info->data_buffer_size));
    }
    if (info->data_buffer && info->data_buffer_size) {
        munmap(info->data_buffer, info->data_buffer_size);
    }

    info->state = 0;

    return info->data_code;
}

status_t _get_thread_info(thread_id id, thread_info *info, size_t size)
{
    _thread_info *_nfo = _find_thread_info(id);

    if (_nfo == NULL) {
        return B_BAD_VALUE;
    }

    memset(info, 0, size);
    info->thread = id;
    info->team = getpid();
    strncpy(info->name, _nfo->name, B_OS_NAME_LENGTH);
    info->priority = _nfo->priority;
    info->sem = _nfo->sem;
    info->stack_base = _nfo->stack_base;
    info->stack_end = _nfo->stack_end;

    if (_nfo->state) {
        info->state = _nfo->state;
    }
    else switch (_nfo->task_state) {
    case TASK_NEW:
    case TASK_WAITING:
        info->state = B_THREAD_SUSPENDED;
        break;
    case TASK_RUNNING:
    case TASK_EXITED:
        info->state = B_THREAD_RUNNING; // FIXME: This could be B_THREAD_READY
        break;
    }

    struct rusage usage;
    getrusage(RUSAGE_THREAD, &usage);
    info->user_time = usage.ru_utime.tv_sec * 1000000 + usage.ru_utime.tv_usec;
    info->kernel_time = usage.ru_stime.tv_sec * 1000000 + usage.ru_stime.tv_usec;

    return B_OK;
}

status_t _get_next_thread_info(team_id team, int32 *cookie, thread_info *info, size_t size)
{
    if (team != 0 && team != getpid()) {
        // do not support getting threads of other team
        return B_BAD_VALUE;
    }

    while (*cookie < sizeof(_threads)/sizeof(_threads[0]) && _threads[*cookie].task_state != TASK_RUNNING) (*cookie)++;

    if (*cookie >= sizeof(_threads)/sizeof(_threads[0])) return B_BAD_VALUE;

    return _get_thread_info(_threads[(*cookie)++].thread, info, size);
}
