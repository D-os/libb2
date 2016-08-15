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
#include "utlist.h"
#include "rwlock.h"

/* current thread info */
__thread _thread_info *_info = NULL;

/* head of linked list of all _thread_info */
static _thread_info *_threads = NULL;
RWLOCK(_threads)

static void _thread_init(int argc, char* argv[], char* envp[])
{
    /* fill main thread info */
    _info = calloc(1, sizeof(_thread_info));
    _info->pthread = pthread_self();
    _info->tid = syscall(SYS_gettid);
    _info->team = getpid();
    _info->task_state = TASK_RUNNING;
    prctl(PR_GET_NAME, (unsigned long) _info->name, 0, 0, 0);
    pthread_attr_t attr;
    pthread_getattr_np(_info->pthread, &attr);
    size_t size;
    pthread_attr_getstack(&attr, &_info->stack_base, &size);
    _info->stack_end = (char*)_info->stack_base + size;
    /* add it as first thread */
    _threads_wlock();
    DL_APPEND(_threads, _info);
    _threads_unlock();
}
__attribute__((section(".init_array"))) void (* p_thread_init)(int,char*[],char*[]) = &_thread_init;

/* WARNING! you need to lock _threads in caller function! */
_thread_info *_find_thread_info(thread_id thread)
{
    if (_info->tid == thread) return _info;
    _thread_info *info;
    DL_SEARCH_SCALAR(_threads, info, tid, thread);
    return info;
}

static void _sigaction_handler(int sig)
{
    if (sig == SIGTERM) {
        pthread_exit((void*)(intptr_t)(0x80 + sig));
    }

    if (sig == SIGCONT && _info->task_state < TASK_RUNNING) {
        _info->task_state = TASK_RUNNING;
    }
}

static status_t _suspend_thread(_thread_info *info, _task_state state)
{
    sigset_t wait_mask;
    sigfillset(&wait_mask);
    sigdelset(&wait_mask, SIGCONT);
    sigdelset(&wait_mask, SIGTERM);
    if (cmpxchg(&info->task_state, state, TASK_PAUSED) == state) {
        do {
            sigsuspend(&wait_mask);
        }
        while (info->task_state == TASK_PAUSED);
        return B_OK;
    }
    return B_BAD_THREAD_ID;
}

#include <stdio.h>
static void* _thread_wrapper(void *arg)
{
    _info = arg;

    struct sigaction sa;
    sigfillset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = _sigaction_handler;
    sigaction(SIGCONT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    _info->tid = syscall(SYS_gettid);
    prctl(PR_SET_NAME, (unsigned long) _info->name, 0, 0, 0);

    pthread_attr_t attr;
    pthread_getattr_np(_info->pthread, &attr);
    size_t size;
    pthread_attr_getstack(&attr, &_info->stack_base, &size);
    _info->stack_end = (char*)_info->stack_base + size;

    /* init done - release spawning thread */
    syscall(SYS_futex, &_info->tid, FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0);

    _suspend_thread(_info, TASK_NEW);

    status_t exit = _info->func(_info->data);

    _info->task_state = TASK_EXITED;
    if (_info->task_state_copy) *_info->task_state_copy = _info->task_state;

    _threads_wlock();
    DL_DELETE(_threads, _info);
    _threads_unlock();
    free(_info);

    return (void*)(intptr_t)exit;
}


thread_id spawn_thread(thread_func func, const char *name, int32 priority, void *data)
{
    thread_id ret;
    pthread_attr_t attr;
    _thread_info *info = calloc(1, sizeof(_thread_info));

    if (info == NULL) {
        return B_NO_MORE_THREADS;
    }

    info->team = _info->team;
    info->task_state = TASK_NEW;

    if (name) {
        strncpy(info->name, name, B_OS_NAME_LENGTH);
    }
    else {
        info->name[0] = '\0';
    }

    if (pthread_attr_init(&attr) != 0) {
        ret = B_NO_MEMORY;
        goto error1;
    }

    if (pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED) != 0) {
        ret = B_NO_MORE_THREADS;
        goto error2;
    }

    if (priority > 99) {
        if (pthread_attr_setschedpolicy(&attr, SCHED_RR) != 0) {
            ret = B_NO_MORE_THREADS;
            goto error2;
        }
        struct sched_param schedparam;
        schedparam.sched_priority = priority - 99;
        if (schedparam.sched_priority > 99) schedparam.sched_priority = 99;
        if (pthread_attr_setschedparam(&attr, &schedparam) != 0) {
            ret = B_NO_MORE_THREADS;
            goto error2;
        }
    }
    else {
        /* FIXME! use BFS SCHED_ scheduling! */
        if (pthread_attr_setschedpolicy(&attr, SCHED_OTHER) != 0) {
            ret = B_NO_MORE_THREADS;
            goto error2;
        }
    }
    info->priority = priority;

    info->func = func;
    info->data = data;

    info->has_data = 0;

    /* finally actually kick the thread */
    int s = pthread_create(&info->pthread, &attr, _thread_wrapper, info);
    if (s != 0) {
        info->pthread = 0;
        switch (s) {
        case EAGAIN:
            ret = B_NO_MORE_THREADS;
            goto error2;
        case ENOMEM:
            ret = B_NO_MEMORY;
            goto error2;
        case EPERM:
            ret = B_PERMISSION_DENIED;
            goto error2;
        default:
            ret = B_FROM_POSIX_ERROR(s);
            goto error2;
        }
    }

    pthread_attr_destroy(&attr);

    /* wait for new thread initialisation function */
    syscall(SYS_futex, &info->tid, FUTEX_WAIT_PRIVATE, 0, NULL, NULL, 0);

    _threads_wlock();
    DL_APPEND(_threads, info);
    _threads_unlock();

    return info->tid;
error2:
    pthread_attr_destroy(&attr);
error1:
    free(info);
    return ret;
}

status_t wait_for_thread(thread_id thread, status_t* exit_value)
{
    _threads_rlock();
    _thread_info *info = _find_thread_info(thread);
    status_t status = B_OK;
    int s;
    void *exit = 0;
    _task_state state;

    if (info == NULL) {
        status = B_BAD_THREAD_ID;
        goto exit;
    }

    if (info->task_state != TASK_RUNNING) {
        status = resume_thread(thread);
        if (status != B_OK) {
            goto exit;
        }
    }

    info->task_state_copy = &state;
    _threads_unlock();
    s = pthread_join(info->pthread, &exit);
    if (s != 0) {
        _threads_rlock();
        info = _find_thread_info(thread);
        if (info) info->task_state_copy = NULL;
        _threads_unlock();
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

    *exit_value = (intptr_t)exit;
    return state == TASK_EXITED ? B_OK : B_INTERRUPTED;

exit:
    _threads_unlock();
    return status;
}

thread_id find_thread(const char* name)
{
    if (name == NULL) {
        return _info->tid;
    }

    thread_id ret = B_NAME_NOT_FOUND;
    _thread_info *info;
    _threads_rlock();
    DL_FOREACH(_threads, info) {
        if (strncmp(info->name, name, B_OS_NAME_LENGTH) == 0) {
            ret = info->tid;
            break;
        }
    }
    _threads_unlock();
    return ret;
}

status_t kill_thread(thread_id thread)
{
    _threads_rlock();
    _thread_info *info = _find_thread_info(thread);
    if (info == NULL || info->task_state == TASK_EXITED) {
        _threads_unlock();
        return B_BAD_THREAD_ID;
    }

    pthread_t pthread = info->pthread;
    _threads_unlock();
    return B_FROM_POSIX_ERROR(pthread_kill(pthread, SIGTERM));
}

status_t resume_thread(thread_id thread)
{
    _threads_rlock();
    _thread_info *info = _find_thread_info(thread);
    if (info == NULL) {
        _threads_unlock();
        return B_BAD_THREAD_ID;
    }

    if (cmpxchg(&info->task_state, TASK_NEW, TASK_RUNNING) == TASK_NEW) {
        _threads_unlock();
        return B_OK;
    }

    pthread_t pthread = info->pthread;
    _threads_unlock();
    return B_FROM_POSIX_ERROR(pthread_kill(pthread, SIGCONT));
}

status_t suspend_thread(thread_id thread)
{
    if (_info->tid == thread) {
        return _suspend_thread(_info, TASK_RUNNING);
    }
    /* suspending other threads is not supported */
    abort();
    return B_NOT_SUPPORTED;
}

inline void exit_thread(status_t status)
{
    _info->task_state = TASK_EXITED;
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
    status_t status = B_OK;

    _threads_rlock();
    _thread_info *info = _find_thread_info(thread);

    if (info == NULL) {
        status = B_BAD_THREAD_ID;
        goto exit;
    }


    while ((c = cmpxchg(&info->has_data, 0, 1))) {
        if (syscall(SYS_futex, &info->has_data, FUTEX_WAIT_PRIVATE, c, NULL, NULL, 0) != 0) {
            switch (errno) {
            case EINTR:
                status = B_INTERRUPTED;
                goto exit;
            default:
                status = B_FROM_POSIX_ERROR(errno);
                goto exit;
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
                status = B_NO_MEMORY;
                goto exit;
            default:
                status = B_FROM_POSIX_ERROR(errno);
                goto exit;
            }
        }
        memcpy(info->data_buffer, buffer, bufferSize);
    }
    else {
        info->data_buffer = NULL;
    }
    info->data_buffer_size = bufferSize;
    info->data_code = code;
    info->data_sender = _info->tid;
    c = cmpxchg(&info->has_data, 1, 2); /* mark as ready to read */
    assert(c == 1);
    if (syscall(SYS_futex, &info->has_data, FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0) != 0) {
        switch (errno) {
        case EINTR:
            status = B_INTERRUPTED;
            goto exit;
        default:
            status = B_FROM_POSIX_ERROR(errno);
            goto exit;
        }
    }
exit:
    _threads_unlock();
    return status;
}

int32 receive_data(thread_id *sender, void *buffer, size_t bufferSize)
{
    int c;

    _info->state = B_THREAD_RECEIVING;

    while ((c = cmpxchg(&_info->has_data, 2, 1)) != 2) {
        if (syscall(SYS_futex, &_info->has_data, FUTEX_WAIT_PRIVATE, c, NULL, NULL, 0) != 0) {
            return B_INTERRUPTED;
        }
    }

    assert(sender != NULL);
    *sender = _info->data_sender;
    if (buffer && bufferSize) {
        memcpy(buffer, _info->data_buffer, min_c(bufferSize, _info->data_buffer_size));
    }
    if (_info->data_buffer && _info->data_buffer_size) {
        munmap(_info->data_buffer, _info->data_buffer_size);
    }

    _info->state = 0;

    return _info->data_code;
}

/** WARNING! you need to lock _threads in caller function! */
static status_t _fill_thread_info(thread_info *info, _thread_info *_nfo, size_t size)
{
    if (!info || !_nfo) return B_BAD_VALUE;

    memset(info, 0, size);
    info->thread = _nfo->tid;
    info->team = _nfo->team;
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
    case TASK_PAUSED:
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

status_t _get_thread_info(thread_id id, thread_info *info, size_t size)
{
    _threads_rlock();
    _thread_info *_nfo = _find_thread_info(id);
    if (!_nfo) return B_BAD_VALUE;
    status_t ret = _fill_thread_info(info, _nfo, size);
    _threads_unlock();
    return ret;
}

status_t _get_next_thread_info(team_id team, int32 *cookie, thread_info *info, size_t size)
{
    if (team != 0 && team != _info->team) {
        // do not support getting threads of other team
        return B_BAD_VALUE;
    }

    status_t ret = B_OK;

    _thread_info *_nfo;
    _threads_rlock();
    if (*cookie) {
        DL_SEARCH_SCALAR(_threads, _nfo, tid, *cookie);
        if (!_nfo) {
            ret = B_BAD_VALUE;
            goto exit;
        }
        _nfo = _nfo->next;
        if (!_nfo) {
            ret = B_BAD_VALUE;
            goto exit;
        }
    } else {
        _nfo = _info;
    }

    *cookie = _nfo->tid;
    ret = _fill_thread_info(info, _nfo, size);
exit:
    _threads_unlock();
    return ret;
}
