/*
 * Copyright 2004-2010 Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 *
 * Author:
 *		Erik Jaesler (erik@cgsoftware.com)
 */
#ifndef _SUPPORT_DEFS_H
#define _SUPPORT_DEFS_H


#include <BeBuild.h>
#include <Errors.h>

#include <inttypes.h>
#include <sys/types.h>

/* Linux specific stuff */
#include <limits.h>
#define B_FILE_NAME_LENGTH  PATH_MAX
typedef unsigned long	haiku_build_addr_t;
#define addr_t			haiku_build_addr_t
/* from inttypes.h */
#define __HAIKU_PRI_PREFIX_32
#define __HAIKU_PRI_PREFIX_64   __PRI64_PREFIX
#define __HAIKU_PRI_PREFIX_ADDR __PRIPTR_PREFIX
#define __HAIKU_PRI_PREFIX_GENERIC_ADDR __HAIKU_PRI_PREFIX_ADDR


/* fixed-size integer types */
typedef	int8_t				int8;
typedef u_int8_t			uint8;
typedef	int16_t				int16;
typedef u_int16_t			uint16;
typedef	int32_t				int32;
typedef u_int32_t			uint32;
typedef	int64_t				int64;
typedef u_int64_t			uint64;

/* shorthand types */
typedef volatile int8			vint8;
typedef volatile uint8			vuint8;
typedef volatile int16			vint16;
typedef volatile uint16			vuint16;
typedef volatile int32			vint32;
typedef volatile uint32			vuint32;
typedef volatile int64			vint64;
typedef volatile uint64			vuint64;

typedef volatile long			vlong;
typedef volatile int			vint;
typedef volatile short			vshort;
typedef volatile char			vchar;

typedef volatile unsigned long	vulong;
typedef volatile unsigned int	vuint;
typedef volatile unsigned short	vushort;
typedef volatile unsigned char	vuchar;

typedef unsigned char			uchar;
typedef unsigned short			unichar;

/* descriptive types */
typedef int32					status_t;
typedef int64					bigtime_t;
typedef int64					nanotime_t;
typedef uint32					type_code;
typedef uint32					perform_code;

//typedef __haiku_phys_addr_t		phys_addr_t;
//typedef phys_addr_t				phys_size_t;

//typedef	__haiku_generic_addr_t	generic_addr_t;
//typedef	generic_addr_t			generic_size_t;


/* printf()/scanf() format strings for [u]int* types */
#define B_PRId8			"d"
#define B_PRIi8			"i"
#define B_PRId16		"d"
#define B_PRIi16		"i"
#define B_PRId32		__HAIKU_PRI_PREFIX_32 "d"
#define B_PRIi32		__HAIKU_PRI_PREFIX_32 "i"
#define B_PRId64		__HAIKU_PRI_PREFIX_64 "d"
#define B_PRIi64		__HAIKU_PRI_PREFIX_64 "i"
#define B_PRIu8			"u"
#define B_PRIo8			"o"
#define B_PRIx8			"x"
#define B_PRIX8			"X"
#define B_PRIu16		"u"
#define B_PRIo16		"o"
#define B_PRIx16		"x"
#define B_PRIX16		"X"
#define B_PRIu32		__HAIKU_PRI_PREFIX_32 "u"
#define B_PRIo32		__HAIKU_PRI_PREFIX_32 "o"
#define B_PRIx32		__HAIKU_PRI_PREFIX_32 "x"
#define B_PRIX32		__HAIKU_PRI_PREFIX_32 "X"
#define B_PRIu64		__HAIKU_PRI_PREFIX_64 "u"
#define B_PRIo64		__HAIKU_PRI_PREFIX_64 "o"
#define B_PRIx64		__HAIKU_PRI_PREFIX_64 "x"
#define B_PRIX64		__HAIKU_PRI_PREFIX_64 "X"

#define B_SCNd8			"hhd"
#define B_SCNi8			"hhi"
#define B_SCNd16		"hd"
#define B_SCNi16		"hi"
#define B_SCNd32		__HAIKU_PRI_PREFIX_32 "d"
#define B_SCNi32		__HAIKU_PRI_PREFIX_32 "i"
#define B_SCNd64		__HAIKU_PRI_PREFIX_64 "d"
#define B_SCNi64		__HAIKU_PRI_PREFIX_64 "i"
#define B_SCNu8			"hhu"
#define B_SCNo8			"hho"
#define B_SCNx8			"hhx"
#define B_SCNu16		"hu"
#define B_SCNo16		"ho"
#define B_SCNx16		"hx"
#define B_SCNu32		__HAIKU_PRI_PREFIX_32 "u"
#define B_SCNo32		__HAIKU_PRI_PREFIX_32 "o"
#define B_SCNx32		__HAIKU_PRI_PREFIX_32 "x"
#define B_SCNu64		__HAIKU_PRI_PREFIX_64 "u"
#define B_SCNo64		__HAIKU_PRI_PREFIX_64 "o"
#define B_SCNx64		__HAIKU_PRI_PREFIX_64 "x"

/* printf()/scanf() format strings for some standard types */
/* size_t */
#define B_PRIuSIZE		__HAIKU_PRI_PREFIX_ADDR "u"
#define B_PRIoSIZE		__HAIKU_PRI_PREFIX_ADDR "o"
#define B_PRIxSIZE		__HAIKU_PRI_PREFIX_ADDR "x"
#define B_PRIXSIZE		__HAIKU_PRI_PREFIX_ADDR "X"

#define B_SCNuSIZE		__HAIKU_PRI_PREFIX_ADDR "u"
#define B_SCNoSIZE		__HAIKU_PRI_PREFIX_ADDR "o"
#define B_SCNxSIZE		__HAIKU_PRI_PREFIX_ADDR "x"

/* ssize_t */
#define B_PRIdSSIZE		__HAIKU_PRI_PREFIX_ADDR "d"
#define B_PRIiSSIZE		__HAIKU_PRI_PREFIX_ADDR "i"

#define B_SCNdSSIZE		__HAIKU_PRI_PREFIX_ADDR "d"
#define B_SCNiSSIZE		__HAIKU_PRI_PREFIX_ADDR "i"

/* addr_t */
#define B_PRIuADDR		__HAIKU_PRI_PREFIX_ADDR "u"
#define B_PRIoADDR		__HAIKU_PRI_PREFIX_ADDR "o"
#define B_PRIxADDR		__HAIKU_PRI_PREFIX_ADDR "x"
#define B_PRIXADDR		__HAIKU_PRI_PREFIX_ADDR "X"

#define B_SCNuADDR		__HAIKU_PRI_PREFIX_ADDR "u"
#define B_SCNoADDR		__HAIKU_PRI_PREFIX_ADDR "o"
#define B_SCNxADDR		__HAIKU_PRI_PREFIX_ADDR "x"

/* phys_addr_t */
#define B_PRIuPHYSADDR	__HAIKU_PRI_PREFIX_PHYS_ADDR "u"
#define B_PRIoPHYSADDR	__HAIKU_PRI_PREFIX_PHYS_ADDR "o"
#define B_PRIxPHYSADDR	__HAIKU_PRI_PREFIX_PHYS_ADDR "x"
#define B_PRIXPHYSADDR	__HAIKU_PRI_PREFIX_PHYS_ADDR "X"

#define B_SCNuPHYSADDR	__HAIKU_PRI_PREFIX_PHYS_ADDR "u"
#define B_SCNoPHYSADDR	__HAIKU_PRI_PREFIX_PHYS_ADDR "o"
#define B_SCNxPHYSADDR	__HAIKU_PRI_PREFIX_PHYS_ADDR "x"

/* generic_addr_t */
#define B_PRIuGENADDR	__HAIKU_PRI_PREFIX_GENERIC_ADDR "u"
#define B_PRIoGENADDR	__HAIKU_PRI_PREFIX_GENERIC_ADDR "o"
#define B_PRIxGENADDR	__HAIKU_PRI_PREFIX_GENERIC_ADDR "x"
#define B_PRIXGENADDR	__HAIKU_PRI_PREFIX_GENERIC_ADDR "X"

#define B_SCNuGENADDR	__HAIKU_PRI_PREFIX_GENERIC_ADDR "u"
#define B_SCNoGENADDR	__HAIKU_PRI_PREFIX_GENERIC_ADDR "o"
#define B_SCNxGENADDR	__HAIKU_PRI_PREFIX_GENERIC_ADDR "x"

/* off_t */
#define B_PRIdOFF		B_PRId64
#define B_PRIiOFF		B_PRIi64
#define B_PRIxOFF		B_PRIx64

#define B_SCNdOFF		B_SCNd64
#define B_SCNiOFF		B_SCNi64
#define B_SCNxOFF		B_SCNx64

/* dev_t */
#define B_PRIdDEV		B_PRId32
#define B_PRIiDEV		B_PRIi32

/* ino_t */
#define B_PRIdINO		B_PRId64
#define B_PRIiINO		B_PRIi64

/* time_t */
#define B_PRIdTIME		B_PRId32
#define B_PRIiTIME		B_PRIi32

/* bigtime_t */
#define B_PRIdBIGTIME	B_PRId64
#define B_PRIiBIGTIME	B_PRIi64


/* Printed width of a pointer with the %p format (minus 0x prefix). */
#ifdef B_HAIKU_64_BIT
#	define B_PRINTF_POINTER_WIDTH	16
#else
#	define B_PRINTF_POINTER_WIDTH	8
#endif


/* Empty string ("") */
#ifdef __cplusplus
extern const char *B_EMPTY_STRING;
#endif


/* min and max comparisons */
#ifndef __cplusplus
#	ifndef min
#		define min(a,b) ((a)>(b)?(b):(a))
#	endif
#	ifndef max
#		define max(a,b) ((a)>(b)?(a):(b))
#	endif
#endif

/* min() and max() are functions in C++ */
#define min_c(a,b) ((a)>(b)?(b):(a))
#define max_c(a,b) ((a)>(b)?(a):(b))


/* Grandfathering */
#ifndef __cplusplus
#	include <stdbool.h>
#endif

#ifndef NULL
#	define NULL (0)
#endif


#ifdef __cplusplus
extern "C" {
#endif

/* Other stuff */
extern void*	get_stack_frame(void);

#ifdef __cplusplus
}
#endif

/* Count items in an array, count_of is a common define */
#ifndef count_of
#define count_of(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
/* 0[array] is equivalent to array[0] on plain arrays,
 * but will fail to compile if array happens to be a C++ type that overloads operator[]()
 * The division causes a divide-by-zero operation (that should be caught at compile time
 * since it's a compile-time constant expression) for many (but not all) situations
 * where a pointer is passed as the array parameter. */
#endif


/* Obsolete or discouraged API */

/* use 'true' and 'false' */
#ifndef FALSE
#	define FALSE	0
#endif
#ifndef TRUE
#	define TRUE		1
#endif

#ifdef __cplusplus
/* C++ programming patterns helper macros */

/* Pimped Pimpl
 * https://marcmutz.wordpress.com/translated-articles/pimp-my-pimpl/
 * https://marcmutz.wordpress.com/translated-articles/pimp-my-pimpl-%E2%80%94-reloaded/
 * https://wiki.qt.io/D-Pointer
 * https://techbase.kde.org/Policies/Library_Code_Policy#D-Pointers
 */

template <typename T> static inline T *qGetPtrHelper(T *ptr) { return ptr; }
template <typename Wrapper> static inline typename Wrapper::pointer qGetPtrHelper(const Wrapper &p) { return p.get(); }

#define B_DECLARE_PRIVATE \
    class Private; \
    inline Private* d_func() { return reinterpret_cast<Private *>(qGetPtrHelper(d_ptr)); } \
    inline const Private* d_func() const { return reinterpret_cast<const Private *>(qGetPtrHelper(d_ptr)); } \
    friend class Private;

#define B_DECLARE_PUBLIC(Class) \
    inline Class* q_func() { return static_cast<Class *>(q_ptr); } \
    inline const Class* q_func() const { return static_cast<const Class *>(q_ptr); } \
    friend class Class;

#define B_D Private * const d = d_func()
#define B_Q(Class) Class * const q = q_func()

/*
   Some classes do not permit copies to be made of an object. These
   classes contain a private copy constructor and assignment
   operator to disable copying (the compiler gives an error message).
*/
#define D_DISABLE_COPY(Class) \
    Class(const Class &); \
    Class &operator=(const Class &);

#endif

#ifndef va_copy
/* va_copy is not ANSI standard but is always available as __va_copy */
#define va_copy __va_copy
#endif

/* Use the built-in atomic functions, if requested and available. */

#if __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7)


static __inline__ void
atomic_set(int32* value, int32 newValue)
{
    __atomic_store_n(value, newValue, __ATOMIC_RELEASE);
}


static __inline__ int32
atomic_get_and_set(int32* value, int32 newValue)
{
    return __atomic_exchange_n(value, newValue, __ATOMIC_SEQ_CST);
}


static __inline__ int32
atomic_test_and_set(int32* value, int32 newValue, int32 testAgainst)
{
    __atomic_compare_exchange_n(value, &testAgainst, newValue, 1,
        __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    return testAgainst;
}


static __inline__ int32
atomic_add(int32* value, int32 addValue)
{
    return __atomic_fetch_add(value, addValue, __ATOMIC_SEQ_CST);
}


static __inline__ int32
atomic_and(int32* value, int32 andValue)
{
    return __atomic_fetch_and(value, andValue, __ATOMIC_SEQ_CST);
}


static __inline__ int32
atomic_or(int32* value, int32 orValue)
{
    return __atomic_fetch_or(value, orValue, __ATOMIC_SEQ_CST);
}


static __inline__ int32
atomic_get(int32* value)
{
    return __atomic_load_n(value, __ATOMIC_ACQUIRE);
}


static __inline__ void
atomic_set64(int64* value, int64 newValue)
{
    __atomic_store_n(value, newValue, __ATOMIC_RELEASE);
}


static __inline__ int64
atomic_get_and_set64(int64* value, int64 newValue)
{
    return __atomic_exchange_n(value, newValue, __ATOMIC_SEQ_CST);
}


static __inline__ int64
atomic_test_and_set64(int64* value, int64 newValue, int64 testAgainst)
{
    __atomic_compare_exchange_n(value, &testAgainst, newValue, 1,
        __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    return testAgainst;
}


static __inline__ int64
atomic_add64(int64* value, int64 addValue)
{
    return __atomic_fetch_add(value, addValue, __ATOMIC_SEQ_CST);
}


static __inline__ int64
atomic_and64(int64* value, int64 andValue)
{
    return __atomic_fetch_and(value, andValue, __ATOMIC_SEQ_CST);
}


static __inline__ int64
atomic_or64(int64* value, int64 orValue)
{
    return __atomic_fetch_or(value, orValue, __ATOMIC_SEQ_CST);
}


static __inline__ int64
atomic_get64(int64* value)
{
    return __atomic_load_n(value, __ATOMIC_ACQUIRE);
}


#else	/* __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 7) */

#ifdef __cplusplus
extern "C" {
#endif

/* Atomic functions; previous value is returned */
extern void		atomic_set(int32* value, int32 newValue);
extern int32	atomic_get_and_set(int32* value, int32 newValue);
extern int32	atomic_test_and_set(int32 *value, int32 newValue, int32 testAgainst);
extern int32	atomic_add(int32 *value, int32 addValue);
extern int32	atomic_and(int32 *value, int32 andValue);
extern int32	atomic_or(int32 *value, int32 orValue);
extern int32	atomic_get(int32 *value);

extern void		atomic_set64(int64* value, int64 newValue);
extern int64	atomic_get_and_set64(int64* value, int64 newValue);
extern int64	atomic_test_and_set64(int64 *value, int64 newValue, int64 testAgainst);
extern int64	atomic_add64(int64 *value, int64 addValue);
extern int64	atomic_and64(int64 *value, int64 andValue);
extern int64	atomic_or64(int64 *value, int64 orValue);
extern int64	atomic_get64(int64 *value);

#ifdef __cplusplus
}
#endif

#endif


#endif	/* _SUPPORT_DEFS_H */
