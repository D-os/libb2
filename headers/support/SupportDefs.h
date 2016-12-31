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

#include <BuildDefaults.h>

/*---------------------------------------------------------------*/
/*----- Pull in all Android types to os::support ----------------*/
#include <utils/Errors.h>

namespace os {
namespace support {
using namespace android;
} }	// namespace os::support


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
