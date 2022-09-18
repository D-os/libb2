#ifndef _SUPPORT_DEFS_H
#define _SUPPORT_DEFS_H

#ifndef _SYS_TYPES_H
typedef unsigned long  ulong;
typedef unsigned int   uint;
typedef unsigned short ushort;
#endif /* _SYS_TYPES_H */

#include <Errors.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

/// Shorthand type formats

typedef signed char			   int8;
typedef unsigned char		   uint8;
typedef volatile signed char   vint8;
typedef volatile unsigned char vuint8;

typedef short					int16;
typedef unsigned short			uint16;
typedef volatile short			vint16;
typedef volatile unsigned short vuint16;

typedef long				   int32;
typedef unsigned long		   uint32;
typedef volatile long		   vint32;
typedef volatile unsigned long vuint32;

typedef long long					int64;
typedef unsigned long long			uint64;
typedef volatile long long			vint64;
typedef volatile unsigned long long vuint64;

typedef volatile long  vlong;
typedef volatile int   vint;
typedef volatile short vshort;
typedef volatile char  vchar;

typedef volatile unsigned long	vulong;
typedef volatile unsigned int	vuint;
typedef volatile unsigned short vushort;
typedef volatile unsigned char	vuchar;

typedef unsigned char  uchar;
typedef unsigned short unichar;

/// Descriptive formats
typedef int32  status_t;
typedef int64  bigtime_t;
typedef uint32 type_code;
typedef uint32 perform_code;

/// Empty string ("")
#ifdef __cplusplus
extern const char *B_EMPTY_STRING;
#endif

/// min and max comparisons
/// min() and max() won't work in C++
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

#endif /* _SUPPORT_DEFS_H */
