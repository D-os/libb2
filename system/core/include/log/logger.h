/*
**
** Copyright 2007-2014, The Android Open Source Project
**
** This file is dual licensed.  It may be redistributed and/or modified
** under the terms of the Apache 2.0 License OR version 2 of the GNU
** General Public License.
*/

#ifndef _LIBS_LOG_LOGGER_H
#define _LIBS_LOG_LOGGER_H

#include <fcntl.h>
#include <stdint.h>
#ifdef __linux__
#include <time.h> /* clockid_t definition */
#endif

#include <log/log.h>
#include <log/log_read.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The maximum size of a log entry which can be read from the
 * kernel logger driver. An attempt to read less than this amount
 * may result in read() returning EINVAL.
 */
#define LOGGER_ENTRY_MAX_LEN		1024

struct logger_entry_v4 {
    uint16_t    len;       /* length of the payload */
    uint16_t    hdr_size;  /* sizeof(struct logger_entry_v4) */
    int32_t     pid;       /* generating process's pid */
    uint32_t    tid;       /* generating process's tid */
    uint32_t    sec;       /* seconds since Epoch */
    uint32_t    nsec;      /* nanoseconds */
    uint32_t    lid;       /* log id of the payload, bottom 4 bits currently */
    uint32_t    uid;       /* generating process's uid */
    char        msg[0];    /* the entry's payload */
} __attribute__((__packed__));

/*
 * The maximum size of the log entry payload that can be
 * written to the logger. An attempt to write more than
 * this amount will result in a truncated log entry.
 */
#define LOGGER_PREFIX_MAX_LEN       32
#define LOGGER_PAYLOAD_MAX_LEN      (LOGGER_ENTRY_MAX_LEN - LOGGER_PREFIX_MAX_LEN*2)

#define NS_PER_SEC 1000000000ULL

struct log_msg {
    uint16_t    len;        /* length of the payload */
    uint8_t     priority;   /* syslog priority */
    uint8_t     facility;   /* syslog facility */
    uint64_t    sequence;   /* message sequence number */
    uint64_t    timestamp;  /* monotonic timestamp in microseconds */
    char        buf[LOGGER_ENTRY_MAX_LEN * 2]; /* x2 to get space for prefixes and \-escapes */
#ifdef __cplusplus
    /* Matching log_time operators */
    bool operator== (const log_msg &T) const
    {
        return (sequence == T.sequence);
    }
    bool operator!= (const log_msg &T) const
    {
        return !(*this == T);
    }
    bool operator< (const log_msg &T) const
    {
        return (sequence < T.sequence);
    }
    bool operator>= (const log_msg &T) const
    {
        return !(*this < T);
    }
    bool operator> (const log_msg &T) const
    {
        return (sequence > T.sequence);
    }
    bool operator<= (const log_msg &T) const
    {
        return !(*this > T);
    }
    char *msg()
    {
        return (char *) buf;
    }
#endif
};

struct logger;

log_id_t android_logger_get_id(struct logger *logger);

int android_logger_clear(struct logger *logger);
long android_logger_get_log_size(struct logger *logger);
int android_logger_set_log_size(struct logger *logger, unsigned long size);
long android_logger_get_log_readable_size(struct logger *logger);
int android_logger_get_log_version(struct logger *logger);

struct logger_list;

ssize_t android_logger_get_statistics(struct logger_list *logger_list,
                                      char *buf, size_t len);
ssize_t android_logger_get_prune_list(struct logger_list *logger_list,
                                      char *buf, size_t len);
int android_logger_set_prune_list(struct logger_list *logger_list,
                                  char *buf, size_t len);

#define ANDROID_LOG_RDONLY   O_RDONLY
#define ANDROID_LOG_WRONLY   O_WRONLY
#define ANDROID_LOG_RDWR     O_RDWR
#define ANDROID_LOG_ACCMODE  O_ACCMODE
#define ANDROID_LOG_NONBLOCK O_NONBLOCK
#define ANDROID_LOG_WRAP     0x40000000 /* Block until buffer about to wrap */
#define ANDROID_LOG_WRAP_DEFAULT_TIMEOUT 7200 /* 2 hour default */
#define ANDROID_LOG_PSTORE   0x80000000

struct logger_list *android_logger_list_alloc(int mode,
                                              unsigned int tail,
                                              pid_t pid);
struct logger_list *android_logger_list_alloc_time(int mode,
                                                   log_time start,
                                                   pid_t pid);
void android_logger_list_free(struct logger_list *logger_list);
/* In the purest sense, the following two are orthogonal interfaces */
int android_logger_list_read(struct logger_list *logger_list,
                             struct log_msg *log_msg);

/* Multiple log_id_t opens */
struct logger *android_logger_open(struct logger_list *logger_list,
                                   log_id_t id);
#define android_logger_close android_logger_free
/* Single log_id_t open */
struct logger_list *android_logger_list_open(log_id_t id,
                                             int mode,
                                             unsigned int tail,
                                             pid_t pid);
#define android_logger_list_close android_logger_list_free

#ifdef __cplusplus
}
#endif

#endif /* _LIBS_LOG_LOGGER_H */
