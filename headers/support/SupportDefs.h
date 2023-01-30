#ifndef _SUPPORT_DEFS_H
#define _SUPPORT_DEFS_H

#include <Errors.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/param.h>
#include <sys/types.h>

// fixed-size integer types
typedef int8_t	  int8;
typedef u_int8_t  uint8;
typedef int16_t	  int16;
typedef u_int16_t uint16;
typedef int32_t	  int32;
typedef u_int32_t uint32;
typedef int64_t	  int64;
typedef u_int64_t uint64;

/// shorthand types
typedef volatile int8	vint8;
typedef volatile uint8	vuint8;
typedef volatile int16	vint16;
typedef volatile uint16 vuint16;
typedef volatile int32	vint32;
typedef volatile uint32 vuint32;
typedef volatile int64	vint64;
typedef volatile uint64 vuint64;

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
extern const char* B_EMPTY_STRING;
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

#ifdef __cplusplus
/// Pimpl - https://herbsutter.com/gotw/_101/
// We put it here for ease of use.
// We would have to include it in os header files anyway,
// so we might just have it in a common file.
#ifndef PIMPL_H_H
#define PIMPL_H_H

#include <memory>

template <typename T>
class pimpl
{
   private:
	std::unique_ptr<T> m;

   public:
	pimpl();
	template <typename... Args>
	pimpl(Args&&...);
	~pimpl();
	T* operator->() const;
	T& operator*() const;
};

#endif /* PIMPL_H_H */
#endif /* __cplusplus */
