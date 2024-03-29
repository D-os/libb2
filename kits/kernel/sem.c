#include <OS.h>

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>
#include <linux/futex.h>
#include <assert.h>

#include "private.h"

sem_id create_sem(uint32 thread_count, const char * name)
{
    _sem_info *info = malloc(sizeof(_sem_info));

    if (info == NULL) {
        return B_NO_MEMORY;
    }

    COPY_OS_NAME_LENGTH(info->name, name);
    info->count = thread_count;
    info->team = _info->team;

    return (sem_id)info;
}

status_t delete_sem(sem_id sem)
{
    if (((_sem_info *)sem)->team != _info->team) {
        return B_BAD_SEM_ID;
    }

    syscall(SYS_futex, (uint32_t *)sem, FUTEX_WAKE_PRIVATE, INT_MAX, NULL, NULL, 0);

    free((void*)sem);

    return B_NO_ERROR;
}

status_t get_sem_count(sem_id sem, int32* thread_count)
{
    assert(thread_count);
    const _sem_info *info = (_sem_info *)sem;

    return info->count;
}

status_t _get_sem_info(sem_id sem, struct sem_info *info, size_t infoSize)
{
    assert(info);
    const _sem_info *_info = (_sem_info *)sem;

    info->count = _info->count;
    info->latest_holder = _info->latest_holder;
    info->team = _info->team;
    info->sem = sem;
    COPY_OS_NAME_LENGTH(info->name, _info->name);

    return B_NO_ERROR;
}

// http://locklessinc.com/articles/mutex_cv_futex/

status_t acquire_sem_etc(sem_id sem, uint32 count, uint32 flags, bigtime_t microsecond_timeout)
{
    if (microsecond_timeout == 0 && flags & B_RELATIVE_TIMEOUT && *((uint32*)sem) < count) {
        return B_WOULD_BLOCK;
    }

    uint32 i, a, b, c;

    struct timespec *to = NULL;
    struct timespec tm;
    if (flags & (B_ABSOLUTE_TIMEOUT | B_RELATIVE_TIMEOUT)) {
        if (flags & B_RELATIVE_TIMEOUT) {
            if (clock_gettime(CLOCK_REALTIME, &tm) != 0) {
                return B_FROM_POSIX_ERROR(errno);
            }
        }
        else {
            memset(&tm, 0, sizeof(struct timespec));
        }
        tm.tv_sec += microsecond_timeout / 1000000;
        tm.tv_nsec += (microsecond_timeout % 1000000) * 1000;
        if (tm.tv_nsec > 1000000000) {
            tm.tv_nsec -= 1000000000;
            tm.tv_sec++;
        }
        to = &tm;
    }

again:
    /* spin and try to acquire */
    for (i = 0; i < 100; i++)
    {
        a = *((uint32*)sem);
        /* check if there is enough to take */
        if (a >= count) {
            b = a - count;
            c = cmpxchg((uint32*)sem, a, b);
            /* managed to take what we wanted? */
            if (c == a) {
                _info->state = 0;
                _info->sem = 0;

                /* BeBook: Warning:
                 * The lastest_holder field is highly undependable; in some cases,
                 * the kernel doesn't even record the semaphore acquirer. Although
                 * you can use this field as a hint while debugging, you shouldn't
                 * take it too seriously. Love, Mom.
                 */
                ((_sem_info *)sem)->latest_holder = _info->tid;

                return B_NO_ERROR;
            }
        }

        cpu_relax();
    }

    _info->state = B_THREAD_WAITING;
    _info->sem = sem;

    while (((_sem_info *)sem)->count < count)
    {
        /* Wait in the kernel */
        if (syscall(SYS_futex, (uint32_t *)sem, FUTEX_WAIT_PRIVATE, ((_sem_info *)sem)->count, to, NULL, 0) != 0) {
            switch (errno) {
            case ETIMEDOUT:
                return B_TIMED_OUT;
            case EWOULDBLOCK: //case EAGAIN:
                goto again;
            case EACCES:
                return B_BAD_SEM_ID;
            case EFAULT:
            case EINVAL:
            case ENOSYS:
                return B_BAD_VALUE;
            case EINTR:
                return B_INTERRUPTED;
            default:
                return B_FROM_POSIX_ERROR(errno);
            }
        }
    }

    goto again;
}

status_t release_sem_etc(sem_id sem, int32 count, uint32 flags)
{
    if (count < 1) {
        return B_BAD_VALUE;
    }

    int i;

    /* release, and if was not contended then exit */
    i = atomic_xadd((int*)sem, count);
    if (i > 0) {
        /* BeBook:
         * Normally, releasing a semaphore automatically invokes the kernel's scheduler.
         * In other words, when your thread calls release_sem(), you're pretty much
         * guaranteed that some other thread will be switched in immediately afterwards,
         * even if your thread hasn't gotten its fair share of CPU time.
         */
        if (flags & B_DO_NOT_RESCHEDULE) {
            sched_yield();
        }
        return B_NO_ERROR;
    }

    /* Spin and hope someone acquires what we released */
    for (i = 0; i < 200; i++)
    {
        if (*((int*)sem) <= 0)
        {
            return B_NO_ERROR;
        }
        cpu_relax();
    }

    /* we need to wake someone(s) up */
    syscall(SYS_futex, (uint32_t*)sem, FUTEX_WAKE_PRIVATE, count, NULL, NULL, 0);

    return B_NO_ERROR;
}

inline status_t acquire_sem(sem_id sem)
{
    return acquire_sem_etc(sem, 1, 0, 0);
}

inline status_t release_sem(sem_id sem)
{
    return release_sem_etc(sem, 1, 0);
}
