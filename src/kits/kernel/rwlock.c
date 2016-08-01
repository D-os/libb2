#include <stddef.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include "private.h"

const static unsigned _rwlock_OPEN = 1;
const static unsigned _rwlock_WLOCKED = 0;

void _rwlock_unlock(unsigned *_rwlock_lock)
{
    unsigned current, wanted;
    do {
        current = *_rwlock_lock;
        if (current == _rwlock_OPEN) return;
        if (current == _rwlock_WLOCKED) {
            wanted = _rwlock_OPEN;
        } else {
            wanted = current - 1;
        }
    } while (cmpxchg(_rwlock_lock, current, wanted) != current);
    syscall(SYS_futex, _rwlock_lock, FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0);
}

void _rwlock_rlock(unsigned *_rwlock_lock)
{
    unsigned current;
    while ((current = *_rwlock_lock) == _rwlock_WLOCKED || cmpxchg(_rwlock_lock, current, current + 1) != current) {
        while (syscall(SYS_futex, _rwlock_lock, FUTEX_WAIT_PRIVATE, current, NULL, NULL, 0) != 0) {
            cpu_relax();
            if (*_rwlock_lock >= _rwlock_OPEN) break;
        }
        // will be able to acquire rlock no matter what unlock woke us
    }
}

void _rwlock_wlock(unsigned *_rwlock_lock)
{
    unsigned current;
    while ((current = cmpxchg(_rwlock_lock, _rwlock_OPEN, _rwlock_WLOCKED)) != _rwlock_OPEN) {
        while (syscall(SYS_futex, _rwlock_lock, FUTEX_WAIT_PRIVATE, current, NULL, NULL, 0) != 0) {
            cpu_relax();
            if (*_rwlock_lock == _rwlock_OPEN) break;
        }
        if (*_rwlock_lock != _rwlock_OPEN) {
            // in rlock - won't be able to acquire lock - wake someone else
            syscall(SYS_futex, _rwlock_lock, FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0);
        }
    }
}
