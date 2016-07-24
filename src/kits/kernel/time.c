#include <OS.h>

#include <time.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>

#include "private.h"

uint32 real_time_clock()
{
    struct timespec tm;
    if (clock_gettime(CLOCK_REALTIME, &tm) != 0) {
        return 0;
    }
    return tm.tv_sec;
}

bigtime_t real_time_clock_usecs()
{
    struct timespec tm;
    if (clock_gettime(CLOCK_REALTIME, &tm) != 0) {
        return 0;
    }
    return tm.tv_sec * 1000000 + tm.tv_nsec / 1000;
}

bigtime_t system_time()
{
    struct timespec tm;
    if (clock_gettime(CLOCK_MONOTONIC, &tm) != 0) {
        return 0;
    }
    return tm.tv_sec * 1000000 + tm.tv_nsec / 1000;
}

static status_t _snooze(bigtime_t microseconds, int flags)
{
    struct timespec tm;
    tm.tv_sec = microseconds / 1000000;
    tm.tv_nsec = (microseconds % 1000000) * 1000;

    _info->state = B_THREAD_ASLEEP;

    int ret = clock_nanosleep(CLOCK_MONOTONIC, flags, &tm, NULL);

    _info->state = 0;

    if (ret != 0) {
        switch (ret) {
        case EINTR:
            return B_INTERRUPTED;
        default:
            return B_FROM_POSIX_ERROR(ret);
        }
    }
    return B_OK;
}

status_t snooze(bigtime_t amount)
{
    return _snooze(amount, 0);
}

status_t snooze_until(bigtime_t time, int timeBase)
{
    assert(timeBase == B_SYSTEM_TIMEBASE);
    return _snooze(time, TIMER_ABSTIME);
}
