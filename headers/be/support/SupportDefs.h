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

#ifndef _SYS_TYPES_H
typedef unsigned long			ulong;
typedef unsigned int			uint;
typedef unsigned short			ushort;
#endif  /* _SYS_TYPES_H */

#include <BeBuild.h>
#include <sys/types.h>
#include <Errors.h>


/*-------------------------------------------------------------*/
/*----- Shorthand type formats --------------------------------*/

typedef	signed char				int8;
typedef unsigned char			uint8;
typedef volatile signed char   	vint8;
typedef volatile unsigned char	vuint8;

typedef	short					int16;
typedef unsigned short			uint16;
typedef volatile short			vint16;
typedef volatile unsigned short	vuint16;

typedef	long					int32;
typedef unsigned long			uint32;
typedef volatile long			vint32;
typedef volatile unsigned long	vuint32;

typedef	long long					int64;
typedef unsigned long long			uint64;
typedef volatile long long			vint64;
typedef volatile unsigned long long	vuint64;

typedef volatile long			vlong;
typedef volatile int			vint;
typedef volatile short			vshort;
typedef volatile char			vchar;

typedef volatile unsigned long	vulong;
typedef volatile unsigned int	vuint;
typedef volatile unsigned short	vushort;
typedef volatile unsigned char	vuchar;

typedef unsigned char			uchar;
typedef unsigned short          unichar;


/*-------------------------------------------------------------*/
/*----- Descriptive formats -----------------------------------*/
typedef int32					status_t;
typedef int64					bigtime_t;
typedef uint32					type_code;
typedef uint32					perform_code;


/*-----------------------------------------------*/
/*----- Empty string ("") -----------------------*/ 
#ifdef __cplusplus
extern _IMPEXP_BE const char *B_EMPTY_STRING;
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


/*-----------------------------------------------------*/
/*----- Grandfathering --------------------------------*/

#ifndef __cplusplus
typedef unsigned char			bool;
#define false	0
#define true	1
#endif 

#ifndef NULL
#define NULL 	(0)
#endif


/*---------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

/*----- Atomic functions; old value is returned -----------------*/
extern _IMPEXP_ROOT int32	atomic_add(vint32 *value, int32 addvalue);
extern _IMPEXP_ROOT int32	atomic_and(vint32 *value, int32 andvalue);
extern _IMPEXP_ROOT int32	atomic_or(vint32 *value, int32 orvalue);	

/*----- Other stuff ---------------------------------------------*/
extern _IMPEXP_ROOT void *	get_stack_frame(void);

#ifdef __cplusplus
}
#endif

/*-----------------------------------------------------------*/
/*-------- Obsolete or discouraged API ----------------------*/

/* use 'true' and 'false' */
#ifndef FALSE
#define FALSE		0
#endif
#ifndef TRUE
#define TRUE		1
#endif


/*-------------------------------------------------------------*/
/*-------------------------------------------------------------*/

#endif /* _SUPPORT_DEFS_H */
