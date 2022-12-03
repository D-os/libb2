#ifndef _DEBUG_H
#define _DEBUG_H

#include <OS.h>
#include <SupportDefs.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
extern bool _rtDebugFlag;

bool _debugFlag();
bool _setDebugFlag(bool);

int _debugPrintf(const char *, ...);
int _sPrintf(const char *, ...);
int _xdebugPrintf(const char *, ...);
int _debuggerAssert(const char *, int, const char *);
#ifdef __cplusplus
}
#endif

#if DEBUG
#define SET_DEBUG_ENABLED(FLAG) _setDebugFlag(FLAG)
#define IS_DEBUG_ENABLED() _debugFlag()

#define SERIAL_PRINT(ARGS) _sPrintf ARGS
#define PRINT(ARGS) _debugPrintf ARGS
#define PRINT_OBJECT(OBJ)      \
	if (_rtDebugFlag) {        \
		PRINT(("%s\t", #OBJ)); \
		(OBJ).PrintToStream(); \
	}                          \
	((void)0)
#define TRACE() _debugPrintf("File: %s, Line: %d, Thread: %d\n", \
							 __FILE__, __LINE__, find_thread(NULL))

#define SERIAL_TRACE() _sPrintf("File: %s, Line: %d, Thread: %d\n", \
								__FILE__, __LINE__, find_thread(NULL))

#define DEBUGGER(MSG) \
	if (_rtDebugFlag) debugger(MSG)
#if !defined(ASSERT)
#define ASSERT(E) (!(E) ? _debuggerAssert(__FILE__, __LINE__, #E) \
						: (int)0)
#endif

#define ASSERT_WITH_MESSAGE(expr, msg)                  \
	(!(expr) ? _debuggerAssert(__FILE__, __LINE__, msg) \
			 : (int)0)

#define TRESPASS() DEBUGGER("Should not be here");

#define DEBUG_ONLY(arg) arg

#else /* DEBUG == 0 */
#define SET_DEBUG_ENABLED(FLAG) (void)0
#define IS_DEBUG_ENABLED() (void)0

#define SERIAL_PRINT(ARGS) (void)0
#define PRINT(ARGS) (void)0
#define PRINT_OBJECT(OBJ) (void)0
#define TRACE() (void)0
#define SERIAL_TRACE() (void)0

#define DEBUGGER(MSG) (void)0
#if !defined(ASSERT)
#define ASSERT(E) (void)0
#endif
#define ASSERT_WITH_MESSAGE(expr, msg) \
	(void)0
#define TRESPASS() (void)0
#define DEBUG_ONLY(x)
#endif

#define STATIC_ASSERT static_assert

#endif /* _DEBUG_H */
