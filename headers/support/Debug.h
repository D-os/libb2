/*
 * Copyright (c) 2005 Palmsource, Inc.
 * 
 * This software is licensed as described in the file LICENSE, which
 * you should have received as part of this distribution. The terms
 * are also available at http://www.openbinder.org/license.html.
 * 
 * This software consists of voluntary contributions made by many
 * individuals. For the exact contribution history, see the revision
 * history and logs, available at http://www.openbinder.org
 */

#ifndef _SUPPORT_DEBUG_H
#define _SUPPORT_DEBUG_H

/*!	@file support/Debug.h
	@ingroup CoreSupportUtilities
	@brief Common debugging routines, macros, and definitions.
*/

// #include <stdarg.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <support/atomic.h>
// #include <support/SupportDefs.h>
// #include <ErrorMgr.h>

#ifndef DEBUG
#define DEBUG 0
#endif

// #ifdef __cplusplus
// namespace palmos {
// namespace support {
// class SString;
// } } // namespace os::support
// #endif	/* __cplusplus */

/*!	@addtogroup CoreSupportUtilities
	@{
*/

/*-------------------------------------------------------------*/
/*!	@name Debugging Macros */
//@{

#if DEBUG
	#define PRINT(ARGS) 			_debugPrintf ARGS
	#define PRINT_OBJECT(OBJ)		if (_rtDebugFlag) {		\
										PRINT(("%s\t", #OBJ));	\
										(OBJ).PrintToStream(); 	\
										} ((void) 0)
	#define TRACE()					_debugPrintf("File: %s, Line: %d, Thread: %d\n", \
										__FILE__, __LINE__, SysCurrentThread())
		
	#define DEBUGGER(MSG)			if (_rtDebugFlag) ErrFatalError(MSG);
	#if !defined(ASSERT)
		#define ASSERT(E)			(!(E) ? ErrFatalError(#E) : (int)0)
	#endif

	#define ASSERT_WITH_MESSAGE(expr, msg) \
									(!(expr) ? ErrFatalError(msg) : (int)0)	
	
	#define VALIDATE(x, recover)	if (!(x)) { ASSERT(x); recover; }
	
	#define TRESPASS()				if (1) { _debugPrintf("Should not be here at File: %s, Line: %d, Thread: %d\n",__FILE__,__LINE__,SysCurrentThread()); DEBUGGER("DO SOMETHING"); }
	
	#define DEBUG_ONLY(arg)			arg

	
#else /* DEBUG == 0 */
	#define PRINT(ARGS)				(void)0
	#define PRINT_OBJECT(OBJ)		(void)0
	#define TRACE()					(void)0
	
	#define DEBUGGER(MSG)			(void)0
	#if !defined(ASSERT)
		#define ASSERT(E)				(void)0
	#endif
	#define ASSERT_WITH_MESSAGE(expr, msg) \
									(void)0
	#define VALIDATE(x, recover)	if (!(x)) { recover; }
	#define TRESPASS()				(void)0
	#define DEBUG_ONLY(x)
#endif


#ifdef __cplusplus

template<bool> struct CompileTimeAssert;
template<> struct CompileTimeAssert<true> {};
#define STATIC_ASSERT(x)		CompileTimeAssert< (x) >()

#else	/* __cplusplus */

// STATIC_ASSERT is a compile-time check that can be used to
// verify static expressions such as: STATIC_ASSERT(sizeof(int64_t) == 8);
#define STATIC_ASSERT(x)								\
	do {												\
        enum { __ASSERT_EXPRESSION__ = 2*(x) - 1 };         \
		struct __staticAssertStruct__ {					\
			char __static_assert_failed__[__ASSERT_EXPRESSION__];	\
		};												\
	} while (false)

#endif /* __cplusplus */

/*!	@} */

#endif /* _SUPPORT_DEBUG_H */
