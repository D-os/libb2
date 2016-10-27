/*
** Copyright 2013-2014, The Android Open Source Project
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#define _GNU_SOURCE /* asprintf for x86 host */
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/cdefs.h>
#include <sys/ioctl.h>

#include <cutils/list.h>
#include <log/log.h>
#include <log/logger.h>

#define __LOGGERIO     0xAE

#define LOGGER_GET_LOG_BUF_SIZE    _IO(__LOGGERIO, 1) /* size of log */
#define LOGGER_GET_LOG_LEN         _IO(__LOGGERIO, 2) /* used log len */
#define LOGGER_GET_NEXT_ENTRY_LEN  _IO(__LOGGERIO, 3) /* next entry len */
#define LOGGER_FLUSH_LOG           _IO(__LOGGERIO, 4) /* flush log */
#define LOGGER_GET_VERSION         _IO(__LOGGERIO, 5) /* abi version */
#define LOGGER_SET_VERSION         _IO(__LOGGERIO, 6) /* abi version */

typedef char bool;
#define false (const bool)0
#define true (const bool)1

/* https://www.kernel.org/doc/Documentation/ABI/testing/dev-kmsg */
#define LOG_FILE "/dev/kmsg"
/*
        The output format consists of a prefix carrying the syslog
        prefix including priority and facility, the 64 bit message
        sequence number and the monotonic timestamp in microseconds,
        and a flag field. All fields are separated by a ','.

        Future extensions might add more comma separated values before
        the terminating ';'. Unknown fields and values should be
        gracefully ignored.

        The human readable text string starts directly after the ';'
        and is terminated by a '\n'. Untrusted values derived from
        hardware or other facilities are printed, therefore
        all non-printable characters and '\' itself in the log message
        are escaped by "\x00" C-style hex encoding.

        A line starting with ' ', is a continuation line, adding
        key/value pairs to the log message, which provide the machine
        readable context of the message, for reliable processing in
        userspace.

        Example:
        7,160,424069,-;pci_root PNP0A03:00: host bridge
*/

/* timeout in milliseconds */
#define LOG_TIMEOUT_FLUSH 5
#define LOG_TIMEOUT_NEVER -1

#define logger_for_each(logger, logger_list) \
    for (logger = node_to_item((logger_list)->node.next, struct logger, node); \
         logger != node_to_item(&(logger_list)->node, struct logger, node); \
         logger = node_to_item((logger)->node.next, struct logger, node))

#ifndef __unused
#define __unused __attribute__((unused))
#endif

static int accessmode(int mode)
{
    if ((mode & ANDROID_LOG_ACCMODE) == ANDROID_LOG_WRONLY) {
        return W_OK;
    }
    if ((mode & ANDROID_LOG_ACCMODE) == ANDROID_LOG_RDWR) {
        return R_OK | W_OK;
    }
    return R_OK;
}

struct logger_list {
    struct listnode node;
    int mode;
    unsigned int tail;
    pid_t pid;
    unsigned int queued_lines;
    int timeout_ms;
    int error;
    bool flush;
    bool valid_entry; /* valiant(?) effort to deal with memory starvation */
    struct log_msg entry;
};

struct log_list {
    struct listnode node;
    struct log_msg entry; /* Truncated to event->len() + 1 to save space */
};

struct logger {
    struct listnode node;
    struct logger_list *top;
    int fd;
    log_id_t id;
    short *revents;
    struct listnode log_list;
};

/* android_logger_alloc unimplemented, no use case */
/* android_logger_free not exported */
static void android_logger_free(struct logger *logger)
{
    if (!logger) {
        return;
    }

    while (!list_empty(&logger->log_list)) {
        struct log_list *entry = node_to_item(
            list_head(&logger->log_list), struct log_list, node);
        list_remove(&entry->node);
        free(entry);
        if (logger->top->queued_lines) {
            logger->top->queued_lines--;
        }
    }

    if (logger->fd >= 0) {
        close(logger->fd);
    }

    list_remove(&logger->node);

    free(logger);
}

log_id_t android_logger_get_id(struct logger *logger)
{
    return logger->id;
}

/* worker for sending the command to the logger */
static int logger_ioctl(struct logger *logger, int cmd, int mode)
{
    char *n;
    int  f, ret;

    if (!logger || !logger->top) {
        return -EFAULT;
    }

    if (((mode & ANDROID_LOG_ACCMODE) == ANDROID_LOG_RDWR)
            || (((mode ^ logger->top->mode) & ANDROID_LOG_ACCMODE) == 0)) {
        return ioctl(logger->fd, cmd);
    }

    /* We go here if android_logger_list_open got mode wrong for this ioctl */
    ret = access(LOG_FILE, accessmode(mode));
    if (ret) {
        free(n);
        return ret;
    }

    f = open(n, mode);
    free(n);
    if (f < 0) {
        return f;
    }

    ret = ioctl(f, cmd);
    close (f);

    return ret;
}

int android_logger_clear(struct logger *logger)
{
    return logger_ioctl(logger, LOGGER_FLUSH_LOG, ANDROID_LOG_WRONLY);
}

/* returns the total size of the log's ring buffer */
long android_logger_get_log_size(struct logger *logger)
{
    return logger_ioctl(logger, LOGGER_GET_LOG_BUF_SIZE, ANDROID_LOG_RDWR);
}

int android_logger_set_log_size(struct logger *logger __unused,
                                unsigned long size __unused)
{
    return -ENOTSUP;
}

/*
 * returns the readable size of the log's ring buffer (that is, amount of the
 * log consumed)
 */
long android_logger_get_log_readable_size(struct logger *logger)
{
    return logger_ioctl(logger, LOGGER_GET_LOG_LEN, ANDROID_LOG_RDONLY);
}

/*
 * returns the logger version
 */
int android_logger_get_log_version(struct logger *logger)
{
    int ret = logger_ioctl(logger, LOGGER_GET_VERSION, ANDROID_LOG_RDWR);
    return (ret < 0) ? 1 : ret;
}

/*
 * returns statistics
 */
static const char unsupported[] = "18\nNot Supported\n\f";

ssize_t android_logger_get_statistics(struct logger_list *logger_list __unused,
                                      char *buf, size_t len)
{
    strncpy(buf, unsupported, len);
    return -ENOTSUP;
}

ssize_t android_logger_get_prune_list(struct logger_list *logger_list __unused,
                                      char *buf, size_t len)
{
    strncpy(buf, unsupported, len);
    return -ENOTSUP;
}

int android_logger_set_prune_list(struct logger_list *logger_list __unused,
                                  char *buf, size_t len)
{
    static const char unsupported_error[] = "Unsupported";
    strncpy(buf, unsupported, len);
    return -ENOTSUP;
}

struct logger_list *android_logger_list_alloc(int mode,
                                              unsigned int tail,
                                              pid_t pid)
{
    struct logger_list *logger_list;

    logger_list = calloc(1, sizeof(*logger_list));
    if (!logger_list) {
        return NULL;
    }
    list_init(&logger_list->node);
    logger_list->mode = mode;
    logger_list->tail = tail;
    logger_list->pid = pid;
    return logger_list;
}

struct logger_list *android_logger_list_alloc_time(int mode,
                                                   log_time start __unused,
                                                   pid_t pid)
{
    return android_logger_list_alloc(mode, 0, pid);
}

/* android_logger_list_register unimplemented, no use case */
/* android_logger_list_unregister unimplemented, no use case */

/* Open the named log and add it to the logger list */
struct logger *android_logger_open(struct logger_list *logger_list,
                                   log_id_t id)
{
    struct listnode *node;
    struct logger *logger;

    if (!logger_list || (id >= LOG_ID_MAX)) {
        goto err;
    }

    logger_for_each(logger, logger_list) {
        if (logger->id == id) {
            goto ok;
        }
    }

    logger = calloc(1, sizeof(*logger));
    if (!logger) {
        goto err;
    }

    if (access(LOG_FILE, accessmode(logger_list->mode))) {
        goto err;
    }

    logger->fd = open(LOG_FILE, logger_list->mode & (ANDROID_LOG_ACCMODE | ANDROID_LOG_NONBLOCK));
    if (logger->fd < 0) {
        goto err;
    }

    logger->id = id;
    list_init(&logger->log_list);
    list_add_tail(&logger_list->node, &logger->node);
    logger->top = logger_list;
    logger_list->timeout_ms = LOG_TIMEOUT_FLUSH;
    goto ok;

err_logger:
    free(logger);
err:
    logger = NULL;
ok:
    return logger;
}

/* Open the single named log and make it part of a new logger list */
struct logger_list *android_logger_list_open(log_id_t id,
                                             int mode,
                                             unsigned int tail,
                                             pid_t pid)
{
    struct logger_list *logger_list = android_logger_list_alloc(mode, tail, pid);
    if (!logger_list) {
        return NULL;
    }

    if (!android_logger_open(logger_list, id)) {
        android_logger_list_free(logger_list);
        return NULL;
    }

    return logger_list;
}

/* prevent memory starvation when backfilling */
static unsigned int queue_threshold(struct logger_list *logger_list)
{
    return (logger_list->tail < 64) ? 64 : logger_list->tail;
}

static bool low_queue(struct listnode *node)
{
    /* low is considered less than 2 */
    return list_head(node) == list_tail(node);
}

/* Flush queues in sequential order, one at a time */
static int android_logger_list_flush(struct logger_list *logger_list,
                                     struct log_msg *log_msg)
{
    int ret = 0;
    struct log_list *firstentry = NULL;

    while ((ret == 0)
            && (logger_list->flush
                || (logger_list->queued_lines > logger_list->tail))) {
        struct logger *logger;

        /* Merge sort */
        bool at_least_one_is_low = false;
        struct logger *firstlogger = NULL;
        firstentry = NULL;

        logger_for_each(logger, logger_list) {
            struct listnode *node;
            struct log_list *oldest = NULL;

            /* kernel logger channels not necessarily time-sort order */
            list_for_each(node, &logger->log_list) {
                struct log_list *entry = node_to_item(node,
                                                      struct log_list, node);
                if (!oldest
                        || entry->entry.sequence < oldest->entry.sequence) {
                    oldest = entry;
                }
            }

            if (!oldest) {
                at_least_one_is_low = true;
                continue;
            } else if (low_queue(&logger->log_list)) {
                at_least_one_is_low = true;
            }

            if (!firstentry
                    || oldest->entry.sequence < firstentry->entry.sequence) {
                firstentry = oldest;
                firstlogger = logger;
            }
        }

        if (!firstentry) {
            break;
        }

        /* when trimming list, tries to keep one entry behind in each bucket */
        if (!logger_list->flush
                && at_least_one_is_low
                && (logger_list->queued_lines < queue_threshold(logger_list))) {
            break;
        }

        /* within tail?, send! */
        if ((logger_list->tail == 0)
                || (logger_list->queued_lines <= logger_list->tail)) {
            ret = sizeof(firstentry->entry) - sizeof(firstentry->entry.buf) + firstentry->entry.len;
            memcpy(log_msg, &firstentry->entry, ret);
        }

        /* next entry */
        list_remove(&firstentry->node);
        free(firstentry);
        if (logger_list->queued_lines) {
            logger_list->queued_lines--;
        }
    }

    /* Flushed the list, no longer in tail mode for continuing content */
    if (logger_list->flush && !firstentry) {
        logger_list->tail = 0;
    }
    return ret;
}

/* Read from the selected logs */
int android_logger_list_read(struct logger_list *logger_list,
                             struct log_msg *log_msg)
{
    struct logger *logger;
    nfds_t nfds;
    struct pollfd *p, *pollfds = NULL;
    int error = 0, ret = 0;

    memset(log_msg, 0, sizeof(struct log_msg));

    if (!logger_list) {
        return -ENODEV;
    }

    if (!(accessmode(logger_list->mode) & R_OK)) {
        logger_list->error = EPERM;
        goto done;
    }

    nfds = 0;
    logger_for_each(logger, logger_list) {
        ++nfds;
    }
    if (nfds <= 0) {
        error = ENODEV;
        goto done;
    }

    /* Do we have anything to offer from the buffer or state? */
    if (logger_list->valid_entry) { /* implies we are also in a flush state */
        goto flush;
    }

    ret = android_logger_list_flush(logger_list, log_msg);
    if (ret) {
        goto done;
    }

    if (logger_list->error) { /* implies we are also in a flush state */
        goto done;
    }

    /* Lets start grinding on metal */
    pollfds = calloc(nfds, sizeof(struct pollfd));
    if (!pollfds) {
        error = ENOMEM;
        goto flush;
    }

    p = pollfds;
    logger_for_each(logger, logger_list) {
        p->fd = logger->fd;
        p->events = POLLIN;
        logger->revents = &p->revents;
        ++p;
    }

    while (!ret && !error) {
        int result;

        /* If we oversleep it's ok, i.e. ignore EINTR. */
        result = TEMP_FAILURE_RETRY(
                    poll(pollfds, nfds, logger_list->timeout_ms));

        if (result <= 0) {
            if (result) {
                error = errno;
            } else if (logger_list->mode & ANDROID_LOG_NONBLOCK) {
                error = EAGAIN;
            } else {
                logger_list->timeout_ms = LOG_TIMEOUT_NEVER;
            }

            logger_list->flush = true;
            goto try_flush;
        }

        logger_list->timeout_ms = LOG_TIMEOUT_FLUSH;

        /* Anti starvation */
        if (!logger_list->flush
                && (logger_list->queued_lines > (queue_threshold(logger_list) / 2))) {
            /* Any queues with input pending that is low? */
            bool starving = false;
            logger_for_each(logger, logger_list) {
                if ((*(logger->revents) & POLLIN)
                        && low_queue(&logger->log_list)) {
                    starving = true;
                    break;
                }
            }

            /* pushback on any queues that are not low */
            if (starving) {
                logger_for_each(logger, logger_list) {
                    if ((*(logger->revents) & POLLIN)
                            && !low_queue(&logger->log_list)) {
                        *(logger->revents) &= ~POLLIN;
                    }
                }
            }
        }

        logger_for_each(logger, logger_list) {
            struct log_list *entry;
            char *msg, *dst, *end;
            unsigned long syslog;
            int field = 0;

            if (!(*(logger->revents) & POLLIN)) {
                continue;
            }

            memset(&logger_list->entry, 0, sizeof(struct log_msg));
            /* NOTE: driver guarantees we read exactly one full entry */
            result = read(logger->fd, logger_list->entry.buf, sizeof(logger_list->entry.buf));
            if (result <= 0) {
                if (!result) {
                    error = EIO;
                } else if (errno != EINTR) {
                    error = errno;
                }
                continue;
            }

            msg = logger_list->entry.buf;
            while (*msg != ';') {
                switch (field++) {
                case 0:
                    syslog = strtoul(msg, &msg, 10);
                    logger_list->entry.priority = syslog & 07;
                    logger_list->entry.facility = syslog >> 3;
                    break;
                case 1:
                    logger_list->entry.sequence = strtoull(msg, &msg, 10);
                    break;
                case 2:
                    logger_list->entry.timestamp = strtoull(msg, &msg, 10);
                    break;
                }
                *msg++;
            }
            msg++; /* skip ';' */

            dst = logger_list->entry.buf;
            end = dst + result;
            while (msg < end && *msg != '\n' && *msg != '\0') {
                if (*msg == '\\') {
                    msg += 2; /* skip \x */
                    *dst = 0;
                    if (*msg >= '0' && *msg <= '9') *dst = (*msg - '0') << 4;
                    else if (*msg >= 'a' && *msg <= 'f') *dst = (*msg - 'a' + 10) << 4;
                    msg++;
                    if (*msg >= '0' && *msg <= '9') *dst += (*msg - '0');
                    else if (*msg >= 'a' && *msg <= 'f') *dst += (*msg - 'a' + 10);
                } else
                    *dst = *msg;
                msg++;
                dst++;
            }
            *dst = '\0'; /* we have skipped at least one character (;), so there is a place for terminator */
            logger_list->entry.len = dst - logger_list->entry.buf;

            /* speedup: If not tail, and only one list, send directly */
            if (!logger_list->tail
                    && (list_head(&logger_list->node)
                        == list_tail(&logger_list->node))) {
                ret = result;
                memcpy(log_msg, &logger_list->entry,
                       sizeof(logger_list->entry) - sizeof(logger_list->entry.buf) + logger_list->entry.len);
                break;
            }

            entry = malloc(sizeof(*entry) - sizeof(entry->entry.buf) + logger_list->entry.len + 1);

            if (!entry) {
                logger_list->valid_entry = true;
                error = ENOMEM;
                break;
            }

            logger_list->queued_lines++;

            memcpy(&entry->entry, &logger_list->entry,
                   sizeof(logger_list->entry) - sizeof(logger_list->entry.buf) + logger_list->entry.len);
            entry->entry.buf[entry->entry.len] = '\0';
            list_add_tail(&logger->log_list, &entry->node);
        }

        if (ret <= 0) {
try_flush:
            ret = android_logger_list_flush(logger_list, log_msg);
        }
    }

    free(pollfds);

flush:
    if (error) {
        logger_list->flush = true;
    }

    if (ret <= 0) {
        ret = android_logger_list_flush(logger_list, log_msg);

        if (!ret && logger_list->valid_entry) {
            ret = logger_list->entry.len;

            memcpy(&log_msg, &logger_list->entry,
                   sizeof(struct log_msg));
            logger_list->valid_entry = false;
        }
    }

done:
    if (logger_list->error) {
        error = logger_list->error;
    }
    if (error) {
        logger_list->error = error;
        if (!ret) {
            ret = -error;
        }
    }
    return ret;
}

/* Close all the logs */
void android_logger_list_free(struct logger_list *logger_list)
{
    if (logger_list == NULL) {
        return;
    }

    while (!list_empty(&logger_list->node)) {
        struct listnode *node = list_head(&logger_list->node);
        struct logger *logger = node_to_item(node, struct logger, node);
        android_logger_free(logger);
    }

    free(logger_list);
}
