#include <OS.h>

#include <sched.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <signal.h>

static const int STACK_SIZE = 256 * 1024;

struct _thread_info {
    char			name[B_OS_NAME_LENGTH];
    void			*stack;
    thread_func		func;
    void			*data;
};

static int _thread_wrapper(void *arg)
{
    struct _thread_info *info = (struct _thread_info *)arg;
    status_t exit;

    prctl(PR_SET_NAME, (unsigned long) info->name, 0, 0, 0);

    /* threads start in suspended state */
    pid_t tgid = getpid();
    pid_t tid = syscall(SYS_gettid);
    syscall(SYS_tgkill, tgid, tid, SIGSTOP);

    exit = info->func(info->data);

    free(info->stack);
    free(info);
    return exit;
}

thread_id spawn_thread(thread_func func, const char *name, int32 priority, void *data)
{
    (void)priority; /* FIXME: not implemented */

    struct _thread_info *info = malloc(sizeof(struct _thread_info));
    strncpy(info->name, name, B_OS_NAME_LENGTH);
    info->stack = malloc(STACK_SIZE);
    if (!info->stack) {
        return B_NO_MEMORY;
    }
    info->func = func;
    info->data = data;

    int tid = clone(_thread_wrapper, info->stack + STACK_SIZE,
                    CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_IO | CLONE_SIGHAND,
                    data);
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
