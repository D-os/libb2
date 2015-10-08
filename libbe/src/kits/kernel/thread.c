#include <OS.h>

#include <sched.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <signal.h>
#include <linux/futex.h>

static const int STACK_SIZE = 256 * 1024;

struct _thread_info {
    pid_t			tid;
    char			name[B_OS_NAME_LENGTH];
    void			*stack;
    thread_func		func;
    void			*data;
    status_t		exit;
    int				dirty;
};

static struct _thread_info _threads[256];

static int _thread_wrapper(void *arg)
{
    struct _thread_info *info = (struct _thread_info *)arg;

    prctl(PR_SET_NAME, (unsigned long) info->name, 0, 0, 0);

    /* threads start in suspended state */ //FIXME! potential for race with wait_for_thread()
    syscall(SYS_tgkill, getpid(), info->tid, SIGSTOP);

    info->exit = info->func(info->data);

    free(info->stack);

    info->dirty = 0;
    return 0;
}

static status_t _signal_thread(thread_id thread, int sig)
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

thread_id spawn_thread(thread_func func, const char *name, int32 priority, void *data)
{
    (void)priority; /* FIXME: not implemented */

    struct _thread_info *info = _threads;
    struct _thread_info *end = _threads + sizeof(_threads)/sizeof(_threads[0]);
    while ( info < end ){
        if (info->tid == 0) {
            info->dirty = 1;
            strncpy(info->name, name, B_OS_NAME_LENGTH);
            info->stack = malloc(STACK_SIZE);
            if (!info->stack) {
                return B_NO_MEMORY;
            }
            info->func = func;
            info->data = data;

            int tid = clone(_thread_wrapper, info->stack + STACK_SIZE,
                            CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_IO | CLONE_SIGHAND |
                            CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID, /* FIXME: CLONE_STOPPED */
                            data, NULL, NULL, info);
            if (tid == -1) {
                switch (errno) {
                case EAGAIN:
                    return B_NO_MORE_THREADS;
                case ENOMEM:
                    return B_NO_MEMORY;
                default:
                    return B_FROM_POSIX_ERROR(errno);
                }
            }

            return tid;
        }

        info++; /* check next struct _thread_info */
    }
    return B_NO_MORE_THREADS;
}

status_t wait_for_thread(thread_id thread, status_t* exit_value)
{
    struct _thread_info *info = _threads;
    struct _thread_info *end = _threads + sizeof(_threads)/sizeof(_threads[0]);
    while ( info < end ){
        if (info->tid == thread) {
            status_t status = _signal_thread(thread, SIGCONT);
            if (status != B_OK) {
                return status;
            }
            if (syscall(SYS_futex, info, FUTEX_WAIT, 0, NULL, NULL, 0) != 0) {
                switch (errno) {
                case EINTR:
                    return B_INTERRUPTED;
                case EINVAL:
                    return B_BAD_THREAD_ID;
                default:
                    return B_FROM_POSIX_ERROR(errno);
                }
            }
            *exit_value = info->exit;
            return info->dirty ? B_INTERRUPTED : B_OK;
        }
        info++;
    }
    return B_BAD_THREAD_ID;
}

status_t kill_thread(thread_id thread)
{
    return _signal_thread(thread, SIGTERM);
}

status_t resume_thread(thread_id thread)
{
    return _signal_thread(thread, SIGCONT);
}

status_t suspend_thread(thread_id thread)
{
    return _signal_thread(thread, SIGSTOP);
}
