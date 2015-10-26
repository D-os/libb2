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

static const int STACK_SIZE = 256 * 1024;

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

static int _thread_wrapper(void *arg)
{
    _thread_info *info = (_thread_info *)arg;

    prctl(PR_SET_NAME, (unsigned long) info->name, 0, 0, 0);

    /* threads start in suspended state */ //FIXME! potential for race with wait_for_thread()
    syscall(SYS_tgkill, getpid(), info->tid, SIGSTOP);

    info->exit = info->func(info->data);

    munmap(info->stack_base, STACK_SIZE);

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
    _thread_info *info = _find_thread_info(0);

    if (info == NULL) {
        return B_NO_MORE_THREADS;
    }

    info->dirty = 1;
    strncpy(info->name, name, B_OS_NAME_LENGTH);
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
    info->priority = priority; /* FIXME: not implemented */
    info->func = func;
    info->data = data;

    int tid = clone(_thread_wrapper, info->stack_end,
                    CLONE_THREAD | CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_IO | CLONE_SIGHAND |
                    CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID, /* FIXME: CLONE_STOPPED */
                    info, NULL, NULL, &info->tid);
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

status_t wait_for_thread(thread_id thread, status_t* exit_value)
{
    _thread_info *info = _find_thread_info(thread);

    if (info == NULL) {
        return B_BAD_THREAD_ID;
    }

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
    assert(exit_value != NULL);
    *exit_value = info->exit;
    return info->dirty ? B_INTERRUPTED : B_OK;
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
    return _signal_thread(thread, SIGKILL);
}

status_t resume_thread(thread_id thread)
{
    return _signal_thread(thread, SIGCONT);
}

status_t suspend_thread(thread_id thread)
{
    return _signal_thread(thread, SIGSTOP);
}

void exit_thread(status_t status)
{
    _thread_info *info = _find_thread_info(syscall(SYS_gettid));
    assert(info != NULL);
    info->exit = status;
    info->dirty = 0;
    kill_thread(info->tid);
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
