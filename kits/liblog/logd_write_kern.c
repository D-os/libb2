/*
 * Copyright (C) 2007-2014 The Android Open Source Project
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

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <log/log.h>
#include <log/logger.h>

#define LOG_BUF_SIZE 1024

#define log_open(pathname, flags) open(pathname, (flags) | O_CLOEXEC)
#define log_writev(filedes, vector, count) writev(filedes, vector, count)
#define log_close(filedes) close(filedes)

static int __write_to_log_init(log_id_t, struct iovec *vec, size_t nr);
static int (*write_to_log)(log_id_t, struct iovec *vec, size_t nr) = __write_to_log_init;

static pthread_mutex_t log_init_lock = PTHREAD_MUTEX_INITIALIZER;

#ifndef __unused
#define __unused  __attribute__((__unused__))
#endif

static int log_fd = -1;

static int __write_to_log_null(log_id_t log_fd __unused, struct iovec *vec __unused,
                               size_t nr __unused)
{
    return -1;
}

static int __write_to_log_kernel(log_id_t log_id, struct iovec *vec, size_t nr)
{
    ssize_t ret;

    do {
        ret = log_writev(log_fd, vec, nr);
        if (ret < 0) {
            ret = -errno;
        }
    } while (ret == -EINTR);

    return ret;
}

static int __write_to_log_init(log_id_t log_id, struct iovec *vec, size_t nr)
{
    pthread_mutex_lock(&log_init_lock);

    if (write_to_log == __write_to_log_init) {
        log_fd = log_open("/dev/kmsg", O_WRONLY);

        write_to_log = __write_to_log_kernel;

        if (log_fd < 0) {
            log_close(log_fd);
            log_fd = -1;
            write_to_log = __write_to_log_null;
        }
    }

    pthread_mutex_unlock(&log_init_lock);

    return write_to_log(log_id, vec, nr);
}

int __android_log_is_loggable(int prio, const char *tag __unused, int def)
{
    int logLevel = def;
    return logLevel >= 0 && prio >= logLevel;
}

int __android_log_write(int prio, const char *tag, const char *msg)
{
    return __android_log_buf_write(LOG_ID_MAIN, prio, tag, msg);
}

int __android_log_buf_write(int bufID, int prio, const char *tag, const char *msg)
{
    struct iovec vec[7];
    size_t els, tag_len, msg_len = strlen(msg);
    if (msg_len > LOGGER_PAYLOAD_MAX_LEN) msg_len = LOGGER_PAYLOAD_MAX_LEN;

    char marker[8];
    snprintf(marker, 7, "%d", (bufID << 3) + (prio & 07));
    marker[7] = '\0';

    vec[0].iov_base   = (void *) "<";
    vec[0].iov_len    = 1;
    vec[1].iov_base   = marker;
    vec[1].iov_len    = strlen(marker);
    vec[2].iov_base   = (void *) ">";
    vec[2].iov_len    = 1;
    if (tag && tag[0] != '\0') {
        tag_len = strlen(tag);
        if (tag_len > LOGGER_PREFIX_MAX_LEN - 2) tag_len = LOGGER_PREFIX_MAX_LEN - 2;
        vec[3].iov_base   = (void *) tag;
        vec[3].iov_len    = tag_len;
        vec[4].iov_base   = (void *) ": ";
        vec[4].iov_len    = 2;
        vec[5].iov_base   = (void *) msg;
        vec[5].iov_len    = msg_len;
        vec[6].iov_base   = (void *) "\n";
        vec[6].iov_len    = 1;
        els = 7;
    } else {
        vec[3].iov_base   = (void *) msg;
        vec[3].iov_len    = msg_len;
        vec[4].iov_base   = (void *) "\n";
        vec[4].iov_len    = 1;
        els = 5;
    }

    return write_to_log(bufID, vec, els);
}

int __android_log_vprint(int prio, const char *tag, const char *fmt, va_list ap)
{
    char buf[LOG_BUF_SIZE];

    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);

    return __android_log_write(prio, tag, buf);
}

int __android_log_print(int prio, const char *tag, const char *fmt, ...)
{
    va_list ap;
    char buf[LOG_BUF_SIZE];

    va_start(ap, fmt);
    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
    va_end(ap);

    return __android_log_write(prio, tag, buf);
}

int __android_log_buf_print(int bufID, int prio, const char *tag, const char *fmt, ...)
{
    va_list ap;
    char buf[LOG_BUF_SIZE];

    va_start(ap, fmt);
    vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
    va_end(ap);

    return __android_log_buf_write(bufID, prio, tag, buf);
}

void __android_log_assert(const char *cond, const char *tag,
                          const char *fmt, ...)
{
    char buf[LOG_BUF_SIZE];

    if (fmt) {
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
        va_end(ap);
    } else {
        /* Msg not provided, log condition.  N.B. Do not use cond directly as
         * format string as it could contain spurious '%' syntax (e.g.
         * "%d" in "blocks%devs == 0").
         */
        if (cond)
            snprintf(buf, LOG_BUF_SIZE, "Assertion failed: %s", cond);
        else
            strcpy(buf, "Unspecified assertion failed");
    }

    __android_log_write(ANDROID_LOG_FATAL, tag, buf);
    abort(); /* abort so we have a chance to debug the situation */
    /* NOTREACHED */
}
