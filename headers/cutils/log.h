/*
 * Copyright (C) 2005-2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//
// C/C++ logging functions.  See the logging documentation for API details.
//
// We'd like these to be available from C code (in case we import some from
// somewhere), so this has a C interface.
//
// The output will be correct when the log file is shared between multiple
// threads and/or multiple processes so long as the operating system
// supports O_APPEND.  These calls have mutex-protected data structures
// and so are NOT reentrant.  Do not use LOG in a signal handler.
//
#ifndef _LIBS_LOG_LOG_H
#define _LIBS_LOG_LOG_H

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <syslog.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

//typedef enum {
//    LOG_FATAL = 0,    /* syslog LOG_EMERG */
//    LOG_ALERT = 1,
//    LOG_CRIT = 2,
//    LOG_ERROR = 3,    /* syslog LOG_ERR */
//    LOG_WARN = 4,     /* syslog LOG_WARNING */
//    LOG_INFO = 5,     /* syslog NOTICE */
//    LOG_DEBUG = 6,    /* syslog INFO */
//    LOG_VERBOSE = 7,  /* syslog DEBUG */
//} log_priority;
typedef int android_LogPriority;

/*
 * The maximum size of the log entry payload that can be
 * written to the logger. An attempt to write more than
 * this amount will result in a truncated log entry.
 */
#define LOGGER_PREFIX_MAX_LEN       32
#define LOGGER_PAYLOAD_MAX_LEN      512

/*
 * Send a simple string to the log.
 */
void __log_write(int facility, int priority, const char *tag, const char *text);

/*
 * Send a formatted string to the log, used like printf(fmt,...)
 */
void __log_print(int priority, const char *tag,  const char *fmt, ...)
#if defined(__GNUC__)
#ifdef __USE_MINGW_ANSI_STDIO
#if __USE_MINGW_ANSI_STDIO
    __attribute__ ((format(gnu_printf, 3, 4)))
#else
    __attribute__ ((format(printf, 3, 4)))
#endif
#else
    __attribute__ ((format(printf, 3, 4)))
#endif
#endif
    ;

/*
 * A variant of __log_print() that takes a va_list to list
 * additional parameters.
 */
void __log_vprint(int priority, const char *tag,
                         const char *fmt, va_list ap);

/*
 * Log an assertion failure and abort the process to have a chance
 * to inspect it if a debugger is attached. This uses the FATAL priority.
 */
void __log_assert(const char *cond, const char *tag,
                          const char *fmt, ...)
#if defined(__GNUC__)
    __attribute__ ((noreturn))
#ifdef __USE_MINGW_ANSI_STDIO
#if __USE_MINGW_ANSI_STDIO
    __attribute__ ((format(gnu_printf, 3, 4)))
#else
    __attribute__ ((format(printf, 3, 4)))
#endif
#else
    __attribute__ ((format(printf, 3, 4)))
#endif
#endif
    ;

/*
 * Use the per-tag properties "log.tag.<tagname>" to generate a runtime
 * result of non-zero to expose a log.
 */
int __log_is_loggable(int prio, const char *tag, int def);

// ---------------------------------------------------------------------

/*
 * Normally we strip ALOGV (VERBOSE messages) from release builds.
 * You can modify this (for example with "#define LOG_NDEBUG 0"
 * at the top of your source file) to change that behavior.
 */
#ifndef LOG_NDEBUG
#ifdef NDEBUG
#define LOG_NDEBUG 1
#else
#define LOG_NDEBUG 0
#endif
#endif

/*
 * This is the local tag used for the following simplified
 * logging macros.  You can change this preprocessor definition
 * before using the other macros to change the tag.
 */
#ifndef LOG_TAG
#define LOG_TAG NULL
#endif

// ---------------------------------------------------------------------

#ifndef __predict_false
#define __predict_false(exp) __builtin_expect((exp) != 0, 0)
#endif

/*
 * Simplified macro to send a verbose log message using the current LOG_TAG.
 */
#ifndef ALOGV
#define __ALOGV(...) ((void)ALOG(LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#if LOG_NDEBUG
#define ALOGV(...) do { if (0) { __ALOGV(__VA_ARGS__); } } while (0)
#else
#define ALOGV(...) __ALOGV(__VA_ARGS__)
#endif
#endif

#ifndef ALOGV_IF
#if LOG_NDEBUG
#define ALOGV_IF(cond, ...)   ((void)0)
#else
#define ALOGV_IF(cond, ...) \
    ( (__predict_false(cond)) \
    ? ((void)ALOG(LOG_DEBUG, LOG_TAG, __VA_ARGS__)) \
    : (void)0 )
#endif
#endif

/*
 * Simplified macro to send a debug log message using the current LOG_TAG.
 */
#ifndef ALOGD
#define ALOGD(...) ((void)ALOG(LOG_INFO, LOG_TAG, __VA_ARGS__))
#endif

#ifndef ALOGD_IF
#define ALOGD_IF(cond, ...) \
    ( (__predict_false(cond)) \
    ? ((void)ALOG(LOG_INFO, LOG_TAG, __VA_ARGS__)) \
    : (void)0 )
#endif

/*
 * Simplified macro to send an info log message using the current LOG_TAG.
 */
#ifndef ALOGI
#define ALOGI(...) ((void)ALOG(LOG_NOTICE, LOG_TAG, __VA_ARGS__))
#endif

#ifndef ALOGI_IF
#define ALOGI_IF(cond, ...) \
    ( (__predict_false(cond)) \
    ? ((void)ALOG(LOG_NOTICE, LOG_TAG, __VA_ARGS__)) \
    : (void)0 )
#endif

/*
 * Simplified macro to send a warning log message using the current LOG_TAG.
 */
#ifndef ALOGW
#define ALOGW(...) ((void)ALOG(LOG_WARNING, LOG_TAG, __VA_ARGS__))
#endif

#ifndef ALOGW_IF
#define ALOGW_IF(cond, ...) \
    ( (__predict_false(cond)) \
    ? ((void)ALOG(LOG_WARNING, LOG_TAG, __VA_ARGS__)) \
    : (void)0 )
#endif

/*
 * Simplified macro to send an error log message using the current LOG_TAG.
 */
#ifndef ALOGE
#define ALOGE(...) ((void)ALOG(LOG_ERR, LOG_TAG, __VA_ARGS__))
#endif

#ifndef ALOGE_IF
#define ALOGE_IF(cond, ...) \
    ( (__predict_false(cond)) \
    ? ((void)ALOG(LOG_ERR, LOG_TAG, __VA_ARGS__)) \
    : (void)0 )
#endif

// ---------------------------------------------------------------------

/*
 * Conditional based on whether the current LOG_TAG is enabled at
 * verbose priority.
 */
#ifndef IF_ALOGV
#if LOG_NDEBUG
#define IF_ALOGV() if (false)
#else
#define IF_ALOGV() IF_ALOG(LOG_DEBUG, LOG_TAG)
#endif
#endif

/*
 * Conditional based on whether the current LOG_TAG is enabled at
 * debug priority.
 */
#ifndef IF_ALOGD
#define IF_ALOGD() IF_ALOG(LOG_INFO, LOG_TAG)
#endif

/*
 * Conditional based on whether the current LOG_TAG is enabled at
 * info priority.
 */
#ifndef IF_ALOGI
#define IF_ALOGI() IF_ALOG(LOG_NOTICE, LOG_TAG)
#endif

/*
 * Conditional based on whether the current LOG_TAG is enabled at
 * warn priority.
 */
#ifndef IF_ALOGW
#define IF_ALOGW() IF_ALOG(LOG_WARNING, LOG_TAG)
#endif

/*
 * Conditional based on whether the current LOG_TAG is enabled at
 * error priority.
 */
#ifndef IF_ALOGE
#define IF_ALOGE() IF_ALOG(LOG_ERR, LOG_TAG)
#endif


// ---------------------------------------------------------------------

/*
 * Simplified macro to send a verbose system log message using the current LOG_TAG.
 */
#ifndef SLOGV
#define __SLOGV(...) \
    ((void)__log_print(LOG_ID_SYSTEM, LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#if LOG_NDEBUG
#define SLOGV(...) do { if (0) { __SLOGV(__VA_ARGS__); } } while (0)
#else
#define SLOGV(...) __SLOGV(__VA_ARGS__)
#endif
#endif

#ifndef SLOGV_IF
#if LOG_NDEBUG
#define SLOGV_IF(cond, ...)   ((void)0)
#else
#define SLOGV_IF(cond, ...) \
    ( (__predict_false(cond)) \
    ? ((void)__log_print(LOG_ID_SYSTEM, LOG_DEBUG, LOG_TAG, __VA_ARGS__)) \
    : (void)0 )
#endif
#endif

/*
 * Simplified macro to send a debug system log message using the current LOG_TAG.
 */
#ifndef SLOGD
#define SLOGD(...) \
    ((void)__log_print(LOG_ID_SYSTEM, LOG_INFO, LOG_TAG, __VA_ARGS__))
#endif

#ifndef SLOGD_IF
#define SLOGD_IF(cond, ...) \
    ( (__predict_false(cond)) \
    ? ((void)__log_print(LOG_ID_SYSTEM, LOG_INFO, LOG_TAG, __VA_ARGS__)) \
    : (void)0 )
#endif

/*
 * Simplified macro to send an info system log message using the current LOG_TAG.
 */
#ifndef SLOGI
#define SLOGI(...) \
    ((void)__log_print(LOG_ID_SYSTEM, LOG_NOTICE, LOG_TAG, __VA_ARGS__))
#endif

#ifndef SLOGI_IF
#define SLOGI_IF(cond, ...) \
    ( (__predict_false(cond)) \
    ? ((void)__log_print(LOG_ID_SYSTEM, LOG_NOTICE, LOG_TAG, __VA_ARGS__)) \
    : (void)0 )
#endif

/*
 * Simplified macro to send a warning system log message using the current LOG_TAG.
 */
#ifndef SLOGW
#define SLOGW(...) \
    ((void)__log_print(LOG_ID_SYSTEM, LOG_WARNING, LOG_TAG, __VA_ARGS__))
#endif

#ifndef SLOGW_IF
#define SLOGW_IF(cond, ...) \
    ( (__predict_false(cond)) \
    ? ((void)__log_print(LOG_ID_SYSTEM, LOG_WARNING, LOG_TAG, __VA_ARGS__)) \
    : (void)0 )
#endif

/*
 * Simplified macro to send an error system log message using the current LOG_TAG.
 */
#ifndef SLOGE
#define SLOGE(...) \
    ((void)__log_print(LOG_ID_SYSTEM, LOG_ERR, LOG_TAG, __VA_ARGS__))
#endif

#ifndef SLOGE_IF
#define SLOGE_IF(cond, ...) \
    ( (__predict_false(cond)) \
    ? ((void)__log_print(LOG_ID_SYSTEM, LOG_ERR, LOG_TAG, __VA_ARGS__)) \
    : (void)0 )
#endif

// ---------------------------------------------------------------------

/*
 * Log a fatal error.  If the given condition fails, this stops program
 * execution like a normal assertion, but also generating the given message.
 * It is NOT stripped from release builds.  Note that the condition test
 * is -inverted- from the normal assert() semantics.
 */
#ifndef LOG_ALWAYS_FATAL_IF
#define LOG_ALWAYS_FATAL_IF(cond, ...) \
    ( (__predict_false(cond)) \
    ? ((void)__log_printAssert(#cond, LOG_TAG, ## __VA_ARGS__)) \
    : (void)0 )
#endif

#ifndef LOG_ALWAYS_FATAL
#define LOG_ALWAYS_FATAL(...) \
    ( ((void)__log_printAssert(NULL, LOG_TAG, ## __VA_ARGS__)) )
#endif

/*
 * Versions of LOG_ALWAYS_FATAL_IF and LOG_ALWAYS_FATAL that
 * are stripped out of release builds.
 */
#if LOG_NDEBUG

#ifndef LOG_FATAL_IF
#define LOG_FATAL_IF(cond, ...) ((void)0)
#endif
#ifndef LOG_FATAL
#define LOG_FATAL(...) ((void)0)
#endif

#else

#ifndef LOG_FATAL_IF
#define LOG_FATAL_IF(cond, ...) LOG_ALWAYS_FATAL_IF(cond, ## __VA_ARGS__)
#endif
#ifndef LOG_FATAL
#define LOG_FATAL(...) LOG_ALWAYS_FATAL(__VA_ARGS__)
#endif

#endif

/*
 * Assertion that generates a log message when the assertion fails.
 * Stripped out of release builds.  Uses the current LOG_TAG.
 */
#ifndef ALOG_ASSERT
#define ALOG_ASSERT(cond, ...) LOG_FATAL_IF(!(cond), ## __VA_ARGS__)
//#define ALOG_ASSERT(cond) LOG_FATAL_IF(!(cond), "Assertion failed: " #cond)
#endif

// ---------------------------------------------------------------------

/*
 * Basic log message macro.
 *
 * Example:
 *  ALOG(LOG_WARNING, NULL, "Failed with error %d", errno);
 *
 * The second argument may be NULL or "" to indicate the "global" tag.
 */
#ifndef ALOG
#define ALOG(priority, tag, ...) \
    LOG_PRINT(priority, tag, __VA_ARGS__)
#endif

/*
 * Log macro that allows you to specify a number for the priority.
 */
#ifndef LOG_PRINT
#define LOG_PRINT(priority, tag, ...) \
    __log_print(priority, tag, __VA_ARGS__)
#endif

/*
 * Log macro that allows you to pass in a varargs ("args" is a va_list).
 */
#ifndef LOG_PRINT_VA
#define LOG_PRINT_VA(priority, tag, fmt, args) \
    __log_vprint(priority, NULL, tag, fmt, args)
#endif

/*
 * Conditional given a desired logging priority and tag.
 */
#ifndef IF_ALOG
#define IF_ALOG(priority, tag) \
    if (__log_testLog(priority, tag))
#endif

/*
 * ===========================================================================
 *
 * The stuff in the rest of this file should not be used directly.
 */

/* XXX Macros to work around syntax errors in places where format string
 * arg is not passed to ALOG_ASSERT, LOG_ALWAYS_FATAL or LOG_ALWAYS_FATAL_IF
 * (happens only in debug builds).
 */

/* Returns 2nd arg.  Used to substitute default value if caller's vararg list
 * is empty.
 */
#define __log_second(dummy, second, ...)     second

/* If passed multiple args, returns ',' followed by all but 1st arg, otherwise
 * returns nothing.
 */
#define __log_rest(first, ...)               , ## __VA_ARGS__

#define __log_printAssert(cond, tag, fmt...) \
    __log_assert(cond, tag, \
        __log_second(0, ## fmt, NULL) __log_rest(fmt))

#define __log_writeLog(prio, tag, text) \
    __log_write(prio, tag, text)

/*
 *    IF_ALOG uses __log_testLog, but IF_ALOG can be overridden.
 *    __log_testLog will remain constant in its purpose as a wrapper
 *        for logging filter policy, and can be subject to
 *        change. It can be reused by the developers that override
 *        IF_ALOG as a convenient means to reimplement their policy.
 */
#if LOG_NDEBUG /* Production */
#define __log_testLog(prio, tag) \
    (__log_is_loggable(prio, tag, LOG_INFO) != 0)
#else
#define __log_testLog(prio, tag) \
    (__log_is_loggable(prio, tag, LOG_DEBUG) != 0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* _LIBS_LOG_LOG_H */
