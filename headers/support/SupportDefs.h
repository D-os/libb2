/******************************************************************************
/
/	File:			SupportDefs.h
/
/	Description:	Common type definitions.
/
/	Copyright 1993-98, Be Incorporated
/
******************************************************************************/

#ifndef _SUPPORT_DEFS_H
#define _SUPPORT_DEFS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

//#include <BuildDefaults.h>

/*---------------------------------------------------------------*/
/*----- Android types to os::support mapping --------------------*/
#include <binder/IInterface.h>
#include <binder/Parcelable.h>
#include <binder/Status.h>
#include <utils/Errors.h>
#include <utils/KeyedVector.h>
#include <utils/Mutex.h>
#include <utils/RefBase.h>
#include <utils/SortedVector.h>
#include <utils/Vector.h>

namespace os {
namespace support {
using android::OK;
using android::NO_ERROR;
using android::UNKNOWN_ERROR;
using android::NO_MEMORY;
using android::INVALID_OPERATION;
using android::BAD_VALUE;
using android::BAD_TYPE;
using android::NAME_NOT_FOUND;
using android::PERMISSION_DENIED;
using android::NO_INIT;
using android::ALREADY_EXISTS;
using android::DEAD_OBJECT;
using android::FAILED_TRANSACTION;
using android::BAD_INDEX;
using android::NOT_ENOUGH_DATA;
using android::WOULD_BLOCK;
using android::TIMED_OUT;
using android::UNKNOWN_TRANSACTION;
using android::FDS_NOT_ALLOWED;
using android::UNEXPECTED_NULL;

using android::wp;
using android::sp;
using android::Mutex;
using android::IBinder;
using android::IInterface;
using android::interface_cast;
using android::TextOutput;
using android::Vector;
using android::KeyedVector;
using android::SortedVector;
using android::Parcelable;
using android::Parcel;
using android::binder::Status;
}
} // namespace os::support

///*-------------------------------------------------------------*/
///*----- Shorthand type formats --------------------------------*/

//typedef	signed char				int8;
//typedef unsigned char			uint8;
//typedef volatile signed char   	vint8;
//typedef volatile unsigned char	vuint8;

//typedef	short					int16;
//typedef unsigned short			uint16;
//typedef volatile short			vint16;
//typedef volatile unsigned short	vuint16;

//typedef	long					int32;
//typedef unsigned long			uint32;
//typedef volatile long			vint32;
//typedef volatile unsigned long	vuint32;

//typedef	long long					int64;
//typedef unsigned long long			uint64;
//typedef volatile long long			vint64;
//typedef volatile unsigned long long	vuint64;

//typedef volatile long			vlong;
//typedef volatile int			vint;
//typedef volatile short			vshort;
//typedef volatile char			vchar;

//typedef volatile unsigned long	vulong;
//typedef volatile unsigned int	vuint;
//typedef volatile unsigned short	vushort;
//typedef volatile unsigned char	vuchar;

//typedef unsigned char			uchar;
//typedef unsigned short          unichar;

/*-------------------------------------------------------------*/
/*----- Descriptive formats -----------------------------------*/
using android::status_t;
//typedef int32_t					status_t;
//typedef int64_t					bigtime_t;
typedef int64_t  nsecs_t;
typedef uint32_t type_code;
typedef uint32_t perform_code;

// Nanosecond units
#define B_ONE_NANOSECOND ((nsecs_t)1)
#define B_ONE_MICROSECOND (B_ONE_NANOSECOND * 1000)
#define B_ONE_MILLISECOND (B_ONE_MICROSECOND * 1000)
#define B_ONE_SECOND (B_ONE_MILLISECOND * 1000)

// Conversions to nsecs_t (nanoseconds)
#define B_US2NS(us) ((nsecs_t)(us)*1000)
#define B_MS2NS(ms) ((nsecs_t)(ms)*1000000)
#define B_S2NS(s) ((nsecs_t)(s)*1000000000)
#define B_MICROSECONDS_TO_NANOSECONDS(us) B_US2NS(us)
#define B_MILLISECONDS_TO_NANOSECONDS(ms) B_MS2NS(ms)
#define B_SECONDS_TO_NANOSECONDS(s) B_S2NS(s)

// Conversions from nsecs_t (nanoseconds)
#define B_NS2US(ns) ((int64_t)(ns) / 1000)
#define B_NS2MS(ns) ((int64_t)(ns) / 1000000)
#define B_NS2S(ns) ((int64_t)(ns) / 1000000000)
#define B_NANOSECONDS_TO_MICROSECONDS(ns) B_NS2US(ns)
#define B_NANOSECONDS_TO_MILLISECONDS(ns) B_NS2MS(ns)
#define B_NANOSECONDS_TO_SECONDS(ns) B_NS2S(ns)

/* System-wide constants */

#define B_OS_NAME_LENGTH 32

#define B_INFINITE_TIMEOUT 9223372036854775807LL

#define B_MICROSECONDS(n) (B_ONE_MICROSECOND * n)
#define B_MILLISECONDS(n) (B_ONE_MILLISECOND * n)
#define B_SECONDS(n) (B_ONE_SECOND * n)

/*-----------------------------------------------*/
/*----- Empty string ("") -----------------------*/
#ifdef __cplusplus
extern const char* B_EMPTY_STRING;
#endif

/*-----------------------------------------------------*/
/*----- min and max comparisons -----------------------*/
/*----- min() and max() won't work in C++ -------------*/
#define min_c(a, b) ((a) > (b) ? (b) : (a))
#define max_c(a, b) ((a) > (b) ? (a) : (b))

#ifndef __cplusplus
#ifndef min
#define min(a, b) ((a) > (b) ? (b) : (a))
#endif
#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif
#endif

/*-------------------------------------------------------------*/

//!     Locking operation status result.
struct lock_status_t
{
    void (*unlock_func)(void* data); //!< unlock function, NULL if lock failed
    union
    {
        status_t error; //!< error if "unlock_func" is NULL
        void*    data;  //!< locked object if "unlock_func" is non-NULL
    } value;

#ifdef __cplusplus
    //!     Constructor for successfully holding a lock.
    inline lock_status_t(void (*f)(void*), void* d)
    {
        unlock_func = f;
        value.data  = d;
    }
    //!     Constructor for failing to lock.
    inline lock_status_t(status_t e)
    {
        unlock_func = NULL;
        value.error = e;
    }

    //!     Did the lock operation succeed?
    inline bool is_locked() const { return (unlock_func != NULL); }
    //!     B_OK if the lock is held, else an error code.
    inline status_t status() const { return is_locked() ? (status_t)::os::support::OK : value.error; }
    //!     Call to release the lock.  May only be called once.
    inline void unlock() const
    {
        if (unlock_func) unlock_func(value.data);
    }

    //!     Conversion operator for status code, synonym for status().
    inline operator status_t() const { return status(); }
#endif
};

/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/
#ifndef STUB
#define STUB LOG_ALWAYS_FATAL("STUBBED @ %s:%d", __FILE__, __LINE__)
#endif

#endif /* _SUPPORT_DEFS_H */
