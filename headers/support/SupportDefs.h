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

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

//#include <BuildDefaults.h>

/*---------------------------------------------------------------*/
/*----- Android types to os::support mapping --------------------*/
#include <utils/Errors.h>
#include <utils/Mutex.h>
#include <utils/RefBase.h>
#include <binder/IInterface.h>
#include <binder/Parcelable.h>

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
    using android::Parcelable;
    using android::Parcel;
} }	// namespace os::support

using android::status_t;


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
//typedef int32_t					status_t;
//typedef int64_t					bigtime_t;
typedef int64_t					nsecs_t;
typedef uint32_t				type_code;
typedef uint32_t				perform_code;


/*-----------------------------------------------*/
/*----- Empty string ("") -----------------------*/
#ifdef __cplusplus
extern const char *B_EMPTY_STRING;
#endif


/*-----------------------------------------------------*/
/*----- min and max comparisons -----------------------*/
/*----- min() and max() won't work in C++ -------------*/
#define min_c(a,b) ((a)>(b)?(b):(a))
#define max_c(a,b) ((a)>(b)?(a):(b))

#ifndef __cplusplus
#ifndef min
#define min(a,b) ((a)>(b)?(b):(a))
#endif
#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif
#endif


/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/
#ifndef STUB
#define STUB LOG_ALWAYS_FATAL("STUBBED @ %s:%d", __FILE__, __LINE__)
#endif

#endif /* _SUPPORT_DEFS_H */
