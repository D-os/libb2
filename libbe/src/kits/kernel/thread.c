#include <OS.h>

#include <sched.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <signal.h>
#include <linux/futex.h>
#include <assert.h>

#define cmpxchg(P, O, N) __sync_val_compare_and_swap((P), (O), (N))

static const int STACK_SIZE = 256 * 1024;

typedef enum {
    THREAD_STARTING = 0,
    THREAD_RUNNING	= 1,
    THREAD_SUSPEND,
    THREAD_SUSPENDED,
    THREAD_DIE
} _thread_state;

typedef struct {
    pid_t           tid;
    char			name[B_OS_NAME_LENGTH];
    int32			priority;
    void			*stack_base;
    void			*stack_end;
    thread_func		func;
    void			*data;
    status_t		exit;
    int				dirty;
    _thread_state   state;
    int             has_data;
    thread_id       data_sender;
    int32           data_code;
    void            *data_buffer;
    size_t          data_buffer_size;
} _thread_info;

static _thread_info _threads[256];

static _thread_info *_find_thread_info(thread_id thread)
{
    _thread_info *info = _threads;
    _thread_info *end = _threads + sizeof(_threads)/sizeof(_threads[0]);
    while ( info < end ){
        if (info->tid == thread) {
            return info;
        }
        info++;
    }
    return NULL;
}

static void _thread_control(int sig)
{
    if (sig != SIGUSR2) return;
    _thread_info *info = _find_thread_info(syscall(SYS_gettid));
    assert(info != NULL);

    if (info->state == THREAD_DIE) syscall(SYS_exit);

    if (info->state == THREAD_SUSPENDED) return;
    if (info->state == THREAD_SUSPEND) {
        info->state = THREAD_SUSPENDED;
        sigset_t wait_mask;
        sigfillset(&wait_mask);
        sigdelset(&wait_mask, SIGUSR2);
        do {
            sigsuspend(&wait_mask);
        }
        while (info->state == THREAD_SUSPENDED);
    }
}

static status_t _signal_thread_control(thread_id thread, _thread_state state)
{
    _thread_info *info = _find_thread_info(thread);

    if (info == NULL) {
        return B_BAD_THREAD_ID;
    }

    if (info->state == state) return B_OK;

    int do_signal = info->state != THREAD_STARTING;

    info->state = state;

    if (!do_signal) return B_OK;

    if (syscall(SYS_tgkill, getpid(), thread, SIGUSR2) == 0) {
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
    sigaction(SIGUSR2, &sa, NULL);

    _thread_info *info = (_thread_info *)arg;

    prctl(PR_SET_NAME, (unsigned long) info->name, 0, 0, 0);

    cmpxchg(&info->state, THREAD_STARTING, THREAD_SUSPEND);
    _thread_control(SIGUSR2);

    info->exit = info->func(info->data);

    info->dirty = 0;
    return syscall(SYS_exit);
}


thread_id spawn_thread(thread_func func, const char *name, int32 priority, void *data)
{
    _thread_info *info = _find_thread_info(0);

    if (info == NULL) {
        return B_NO_MORE_THREADS;
    }

    info->dirty = 1;
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

    info->state = THREAD_STARTING;
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

    status_t status = resume_thread(thread);
    if (status != B_OK) {
        return status;
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

    int dirty = info->dirty;
    status_t exit = info->exit;

    munmap(info->stack_base, STACK_SIZE);
    info->stack_base = NULL;

    assert(exit_value != NULL);
    *exit_value = exit;
    return dirty ? B_INTERRUPTED : B_OK;
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
    return _signal_thread_control(thread, THREAD_DIE);
}

status_t resume_thread(thread_id thread)
{
    return _signal_thread_control(thread, THREAD_RUNNING);
}

status_t suspend_thread(thread_id thread)
{
    return _signal_thread_control(thread, THREAD_SUSPEND);
}

void exit_thread(status_t status)
{
    _thread_info *info = _find_thread_info(syscall(SYS_gettid));
    assert(info != NULL);
    info->exit = status;
    info->dirty = 0;
    info->state = THREAD_DIE;
    _thread_control(SIGUSR2);
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


    while (c = cmpxchg(&info->has_data, 0, 1)) {
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

    return info->data_code;
}
