#include <OS.h>

#include <sched.h>
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

static const int STACK_SIZE = 256 * 1024;

static _thread_info _threads[256];

_thread_info *_find_thread_info(thread_id thread)
{
    _thread_info *info = _threads;
    _thread_info *end = _threads + sizeof(_threads)/sizeof(_threads[0]);
    if (!likely(_threads[0].tid)) info++;
    while ( info < end ){
        if (info->tid == thread) {
            return info;
        }
        info++;
    }
    if (thread == getpid()) {
        assert(!_threads[0].tid);
        info = _threads;
        info->tid = thread;
        info->task_state = THREAD_RUNNING;
        info->stack_base = (void*)((addr_t)&info & ~(B_PAGE_SIZE - 1));
        info->stack_end = info->stack_base + STACK_SIZE;
        prctl(PR_GET_NAME, (unsigned long) info->name, 0, 0, 0);
        return info;
    }
    return NULL;
}

static void _thread_control(int sig)
{
    if (sig == SIGTERM) {
        syscall(SYS_exit);
    }

    _thread_info *info = _find_thread_info(syscall(SYS_gettid));
    assert(info != NULL);

    if (sig == SIGCONT && info->task_state < THREAD_RUNNING) {
        info->task_state = THREAD_RUNNING;
    }
}

static status_t _signal_thread(pid_t thread, int sig)
{
    if (syscall(SYS_tgkill, getpid(), thread, sig) == 0) {
        return B_OK;
    }
    switch (errno) {
    case EINVAL:
    case ESRCH:
        return B_BAD_THREAD_ID;
    default:
        return B_FROM_POSIX_ERROR(errno);
    }
}

static int _thread_wrapper(void *arg)
{
    struct sigaction sa;
    sigfillset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = _thread_control;
    sigaction(SIGCONT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    _thread_info *info = (_thread_info *)arg;

    prctl(PR_SET_NAME, (unsigned long) info->name, 0, 0, 0);

    if (cmpxchg(&info->task_state, THREAD_NEW, THREAD_WAITING) == THREAD_NEW) {
        sigset_t wait_mask;
        sigfillset(&wait_mask);
        sigdelset(&wait_mask, SIGCONT);
        sigdelset(&wait_mask, SIGTERM);
        do {
            sigsuspend(&wait_mask);
        }
        while (info->task_state == THREAD_WAITING);
    }

    info->exit = info->func(info->data);

    info->task_state = THREAD_EXITED;
    return syscall(SYS_exit);
}


thread_id spawn_thread(thread_func func, const char *name, int32 priority, void *data)
{
    _thread_info *info = _find_thread_info(0);

    if (info == NULL) {
        return B_NO_MORE_THREADS;
    }

    info->task_state = THREAD_NEW;

    strncpy(info->name, name, B_OS_NAME_LENGTH);

    if (info->stack_base == NULL) {
        info->stack_base = mmap(NULL, STACK_SIZE,
                                PROT_READ | PROT_WRITE,
                                MAP_PRIVATE | MAP_ANONYMOUS | MAP_32BIT | MAP_STACK | MAP_GROWSDOWN,
                                -1, 0);
        if (info->stack_base == MAP_FAILED) {
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
        info->stack_end = info->stack_base + STACK_SIZE;
    }
    else {
        memset(info->stack_base, 0, STACK_SIZE);
    }

    info->priority = priority; /* FIXME: not implemented */

    info->func = func;
    info->data = data;

    info->has_data = 0;

    /* finally actually kick the thread */
    info->tid = clone(_thread_wrapper, info->stack_end,
                    CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_IO | CLONE_SIGHAND | CLONE_CHILD_CLEARTID,
                    info, NULL, NULL, &info->tid);
    if (info->tid == -1) {
        switch (errno) {
        case EAGAIN:
            return B_NO_MORE_THREADS;
        case ENOMEM:
            return B_NO_MEMORY;
        default:
            return B_FROM_POSIX_ERROR(errno);
        }
    }

    return info->tid;
}

status_t wait_for_thread(thread_id thread, status_t* exit_value)
{
    _thread_info *info = _find_thread_info(thread);

    if (info == NULL) {
        return B_BAD_THREAD_ID;
    }

    if (info->task_state != THREAD_RUNNING) {
        status_t status = resume_thread(thread);
        if (status != B_OK) {
            return status;
        }
    }

    if (syscall(SYS_futex, &info->tid, FUTEX_WAIT, info->tid, NULL, NULL, 0) != 0) {
        switch (errno) {
        case EINTR:
            return B_INTERRUPTED;
        case EINVAL:
        case EAGAIN:
            return B_BAD_THREAD_ID;
        default:
            return B_FROM_POSIX_ERROR(errno);
        }
    }

    /* FIXME! there is a race to reuse the thread structure! */
    /* copy-out values ASAP */
    _thread_state state = info->task_state;
    status_t exit = info->exit;
    /* free stack if still not reused */
    if (!info->tid) {
        munmap(info->stack_base, STACK_SIZE);
        info->stack_base = NULL;
    }

    assert(exit_value != NULL);
    *exit_value = exit;
    return state == THREAD_EXITED ? B_OK : B_INTERRUPTED;
}

thread_id find_thread(const char* name)
{
    if (name == NULL) {
        return syscall(SYS_gettid);
    }

    _thread_info *info = _threads;
    _thread_info *end = _threads + sizeof(_threads)/sizeof(_threads[0]);
    while (info < end){
        if (strncmp(info->name, name, B_OS_NAME_LENGTH) == 0) {
            return info->tid;
        }
        info++;
    }
    return B_NAME_NOT_FOUND;
}

status_t kill_thread(thread_id thread)
{
    _thread_info *info = _find_thread_info(thread);
    if (info == NULL || info->task_state == THREAD_EXITED) {
        return B_BAD_THREAD_ID;
    }

    return _signal_thread(info->tid, SIGTERM);
}

status_t resume_thread(thread_id thread)
{
    _thread_info *info = _find_thread_info(thread);
    if (info == NULL) {
        return B_BAD_THREAD_ID;
    }

    if (cmpxchg(&info->task_state, THREAD_NEW, THREAD_RUNNING) == THREAD_NEW) {
        return B_OK;
    }

    return _signal_thread(info->tid, SIGCONT);
}

status_t suspend_thread(thread_id thread)
{
    /* suspending thread is not supported */
    abort();
    return B_BAD_THREAD_STATE;
}

void exit_thread(status_t status)
{
    _thread_info *info = _find_thread_info(syscall(SYS_gettid));
    assert(info != NULL);
    info->exit = status;
    info->task_state = THREAD_EXITED;
    _thread_control(SIGTERM);
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
    info->data_sender = syscall(SYS_gettid);
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

    _thread_info *info = _find_thread_info(syscall(SYS_gettid));
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
    case THREAD_NEW:
    case THREAD_WAITING:
        info->state = B_THREAD_SUSPENDED;
        break;
    case THREAD_RUNNING:
    case THREAD_EXITED:
        info->state = B_THREAD_RUNNING; // FIXME: This could be B_THREAD_READY
        break;
    }

    struct rusage usage;
    getrusage(RUSAGE_THREAD, &usage);
    info->user_time = usage.ru_utime.tv_sec * 1000000 + usage.ru_utime.tv_usec;
    info->kernel_time = usage.ru_stime.tv_sec * 1000000 + usage.ru_stime.tv_usec;

    return B_OK;
}
