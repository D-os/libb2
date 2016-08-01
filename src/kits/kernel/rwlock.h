void _rwlock_unlock(unsigned *_rwlock_lock);
void _rwlock_rlock(unsigned *_rwlock_lock);
void _rwlock_wlock(unsigned *_rwlock_lock);

#define RWLOCK(name) \
    static unsigned name ## _lock = 1; \
    inline static void name ## _unlock() { _rwlock_unlock(& name ## _lock); } \
    inline static void name ## _rlock()  { _rwlock_rlock (& name ## _lock); } \
    inline static void name ## _wlock()  { _rwlock_wlock (& name ## _lock); }
