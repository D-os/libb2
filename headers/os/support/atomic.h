#ifndef _SUPPORT_ATOMIC_H
#define _SUPPORT_ATOMIC_H

#ifdef __cplusplus
#include <atomic>
using namespace std;
#else
#include <stdatomic.h>
#endif

#define atomic_fetch_inc(arg)	atomic_fetch_add((arg), 1)
#define atomic_fetch_dec(arg)	atomic_fetch_sub((arg), 1)

#endif
