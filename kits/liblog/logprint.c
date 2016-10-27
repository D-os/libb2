/*
**
** Copyright 2006-2014, The Android Open Source Project
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

#define _GNU_SOURCE /* for asprintf */

#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/param.h>

#include <log/logd.h>
#include <log/logprint.h>

/* open coded fragment, prevent circular dependencies */
#define WEAK static

typedef struct FilterInfo_t {
    char *mTag;
    android_LogPriority mPri;
    struct FilterInfo_t *p_next;
} FilterInfo;

struct AndroidLogFormat_t {
    android_LogPriority global_pri;
    FilterInfo *filters;
    AndroidLogPrintFormat format;
    bool colored_output;
    bool usec_time_output;
    bool printable_output;
};

/*
 *  gnome-terminal color tags
 *    See http://misc.flogisoft.com/bash/tip_colors_and_formatting
 *    for ideas on how to set the forground color of the text for xterm.
 *    The color manipulation character stream is defined as:
 *      ESC [ 3 8 ; 5 ; <color#> m
 */
#define ANDROID_COLOR_BLUE     75
#define ANDROID_COLOR_DEFAULT 231
#define ANDROID_COLOR_GREEN    40
#define ANDROID_COLOR_ORANGE  166
#define ANDROID_COLOR_RED     196
#define ANDROID_COLOR_YELLOW  226

static FilterInfo * filterinfo_new(const char * tag, android_LogPriority pri)
{
    FilterInfo *p_ret;

    p_ret = (FilterInfo *)calloc(1, sizeof(FilterInfo));
    p_ret->mTag = strdup(tag);
    p_ret->mPri = pri;

    return p_ret;
}

/* balance to above, filterinfo_free left unimplemented */

static int colorFromPri (android_LogPriority pri)
{
    switch (pri) {
        case ANDROID_LOG_VERBOSE:       return ANDROID_COLOR_DEFAULT;
        case ANDROID_LOG_DEBUG:         return ANDROID_COLOR_BLUE;
        case ANDROID_LOG_INFO:          return ANDROID_COLOR_GREEN;
        case ANDROID_LOG_WARN:          return ANDROID_COLOR_ORANGE;
        case ANDROID_LOG_ERROR:         return ANDROID_COLOR_RED;
        case ANDROID_LOG_FATAL:         return ANDROID_COLOR_RED;
//        case ANDROID_LOG_SILENT:        return ANDROID_COLOR_DEFAULT;

        case ANDROID_LOG_DEFAULT:
        case ANDROID_LOG_UNKNOWN:
        default:                        return ANDROID_COLOR_DEFAULT;
    }
}

static android_LogPriority filterPriForTag(
        AndroidLogFormat *p_format, const char *tag)
{
    FilterInfo *p_curFilter;

    for (p_curFilter = p_format->filters
            ; p_curFilter != NULL
            ; p_curFilter = p_curFilter->p_next
    ) {
        if (0 == strcmp(tag, p_curFilter->mTag)) {
            if (p_curFilter->mPri == ANDROID_LOG_DEFAULT) {
                return p_format->global_pri;
            } else {
                return p_curFilter->mPri;
            }
        }
    }

    return p_format->global_pri;
}

/**
 * returns 1 if this log line should be printed based on its priority
 * and tag, and 0 if it should not
 */
int android_log_shouldPrintLine (
        AndroidLogFormat *p_format, const char *tag, android_LogPriority pri)
{
    return pri >= filterPriForTag(p_format, tag);
}

AndroidLogFormat *android_log_format_new()
{
    AndroidLogFormat *p_ret;

    p_ret = calloc(1, sizeof(AndroidLogFormat));

    p_ret->global_pri = ANDROID_LOG_VERBOSE;
    p_ret->format = FORMAT_BRIEF;
    p_ret->colored_output = false;
    p_ret->usec_time_output = false;
    p_ret->printable_output = false;

    return p_ret;
}

void android_log_format_free(AndroidLogFormat *p_format)
{
    FilterInfo *p_info, *p_info_old;

    p_info = p_format->filters;

    while (p_info != NULL) {
        p_info_old = p_info;
        p_info = p_info->p_next;

        free(p_info_old);
    }

    free(p_format);
}



int android_log_setPrintFormat(AndroidLogFormat *p_format,
        AndroidLogPrintFormat format)
{
    switch (format) {
    case FORMAT_MODIFIER_COLOR:
        p_format->colored_output = true;
        return 0;
    case FORMAT_MODIFIER_TIME_USEC:
        p_format->usec_time_output = true;
        return 0;
    case FORMAT_MODIFIER_PRINTABLE:
        p_format->printable_output = true;
        return 0;
    default:
        break;
    }
    p_format->format = format;
    return 1;
}

/**
 * Returns FORMAT_OFF on invalid string
 */
AndroidLogPrintFormat android_log_formatFromString(const char * formatString)
{
    static AndroidLogPrintFormat format;

    if (strcmp(formatString, "brief") == 0) format = FORMAT_BRIEF;
    else if (strcmp(formatString, "process") == 0) format = FORMAT_PROCESS;
    else if (strcmp(formatString, "tag") == 0) format = FORMAT_TAG;
    else if (strcmp(formatString, "thread") == 0) format = FORMAT_THREAD;
    else if (strcmp(formatString, "raw") == 0) format = FORMAT_RAW;
    else if (strcmp(formatString, "time") == 0) format = FORMAT_TIME;
    else if (strcmp(formatString, "threadtime") == 0) format = FORMAT_THREADTIME;
    else if (strcmp(formatString, "long") == 0) format = FORMAT_LONG;
    else if (strcmp(formatString, "color") == 0) format = FORMAT_MODIFIER_COLOR;
    else if (strcmp(formatString, "usec") == 0) format = FORMAT_MODIFIER_TIME_USEC;
    else if (strcmp(formatString, "printable") == 0) format = FORMAT_MODIFIER_PRINTABLE;
    else format = FORMAT_OFF;

    return format;
}

/**
 * Splits a wire-format buffer into an AndroidLogEntry
 * entry allocated by caller. Pointers will point directly into buf
 *
 * Returns 0 on success and -1 on invalid wire format (entry will be
 * in unspecified state)
 */
int android_log_processLogBuffer(struct logger_entry *buf,
                                 AndroidLogEntry *entry)
{
    entry->tv_sec = buf->sec;
    entry->tv_nsec = buf->nsec;
    entry->pid = buf->pid;
    entry->tid = buf->tid;

    /*
     * format: <priority:1><tag:N>\0<message:N>\0
     *
     * tag str
     *   starts at buf->msg+1
     * msg
     *   starts at buf->msg+1+len(tag)+1
     *
     * The message may have been truncated by the kernel log driver.
     * When that happens, we must null-terminate the message ourselves.
     */
    if (buf->len < 3) {
        /*
         * An well-formed entry must consist of at least a priority
         * and two null characters
         */
        fprintf(stderr, "+++ LOG: entry too small\n");
        return -1;
    }

    int msgStart = -1;
    int msgEnd = -1;

    int i;
    char *msg = buf->msg;
    struct logger_entry_v2 *buf2 = (struct logger_entry_v2 *)buf;
    if (buf2->hdr_size) {
        msg = ((char *)buf2) + buf2->hdr_size;
    }
    for (i = 1; i < buf->len; i++) {
        if (msg[i] == '\0') {
            if (msgStart == -1) {
                msgStart = i + 1;
            } else {
                msgEnd = i;
                break;
            }
        }
    }

    if (msgStart == -1) {
        fprintf(stderr, "+++ LOG: malformed log message\n");
        return -1;
    }
    if (msgEnd == -1) {
        /* incoming message not null-terminated; force it */
        msgEnd = buf->len - 1;
        msg[msgEnd] = '\0';
    }

    entry->priority = msg[0];
    entry->tag = msg + 1;
    entry->message = msg + msgStart;
    entry->messageLen = msgEnd - msgStart;

    return 0;
}

/*
 * Extract a 4-byte value from a byte stream.
 */
static inline uint32_t get4LE(const uint8_t* src)
{
    return src[0] | (src[1] << 8) | (src[2] << 16) | (src[3] << 24);
}

/*
 * Extract an 8-byte value from a byte stream.
 */
static inline uint64_t get8LE(const uint8_t* src)
{
    uint32_t low, high;

    low = src[0] | (src[1] << 8) | (src[2] << 16) | (src[3] << 24);
    high = src[4] | (src[5] << 8) | (src[6] << 16) | (src[7] << 24);
    return ((uint64_t) high << 32) | (uint64_t) low;
}


/*
 * Recursively convert binary log data to printable form.
 *
 * This needs to be recursive because you can have lists of lists.
 *
 * If we run out of room, we stop processing immediately.  It's important
 * for us to check for space on every output element to avoid producing
 * garbled output.
 *
 * Returns 0 on success, 1 on buffer full, -1 on failure.
 */
static int android_log_printBinaryEvent(const unsigned char** pEventData,
    size_t* pEventDataLen, char** pOutBuf, size_t* pOutBufLen)
{
    const unsigned char* eventData = *pEventData;
    size_t eventDataLen = *pEventDataLen;
    char* outBuf = *pOutBuf;
    size_t outBufLen = *pOutBufLen;
    unsigned char type;
    size_t outCount;
    int result = 0;

    if (eventDataLen < 1)
        return -1;
    type = *eventData++;
    eventDataLen--;

    switch (type) {
    case EVENT_TYPE_INT:
        /* 32-bit signed int */
        {
            int ival;

            if (eventDataLen < 4)
                return -1;
            ival = get4LE(eventData);
            eventData += 4;
            eventDataLen -= 4;

            outCount = snprintf(outBuf, outBufLen, "%d", ival);
            if (outCount < outBufLen) {
                outBuf += outCount;
                outBufLen -= outCount;
            } else {
                /* halt output */
                goto no_room;
            }
        }
        break;
    case EVENT_TYPE_LONG:
        /* 64-bit signed long */
        {
            uint64_t lval;

            if (eventDataLen < 8)
                return -1;
            lval = get8LE(eventData);
            eventData += 8;
            eventDataLen -= 8;

            outCount = snprintf(outBuf, outBufLen, "%" PRId64, lval);
            if (outCount < outBufLen) {
                outBuf += outCount;
                outBufLen -= outCount;
            } else {
                /* halt output */
                goto no_room;
            }
        }
        break;
    case EVENT_TYPE_FLOAT:
        /* float */
        {
            uint32_t ival;
            float fval;

            if (eventDataLen < 4)
                return -1;
            ival = get4LE(eventData);
            fval = *(float*)&ival;
            eventData += 4;
            eventDataLen -= 4;

            outCount = snprintf(outBuf, outBufLen, "%f", fval);
            if (outCount < outBufLen) {
                outBuf += outCount;
                outBufLen -= outCount;
            } else {
                /* halt output */
                goto no_room;
            }
        }
        break;
    case EVENT_TYPE_STRING:
        /* UTF-8 chars, not NULL-terminated */
        {
            unsigned int strLen;

            if (eventDataLen < 4)
                return -1;
            strLen = get4LE(eventData);
            eventData += 4;
            eventDataLen -= 4;

            if (eventDataLen < strLen)
                return -1;

            if (strLen < outBufLen) {
                memcpy(outBuf, eventData, strLen);
                outBuf += strLen;
                outBufLen -= strLen;
            } else if (outBufLen > 0) {
                /* copy what we can */
                memcpy(outBuf, eventData, outBufLen);
                outBuf += outBufLen;
                outBufLen -= outBufLen;
                goto no_room;
            }
            eventData += strLen;
            eventDataLen -= strLen;
            break;
        }
    case EVENT_TYPE_LIST:
        /* N items, all different types */
        {
            unsigned char count;
            int i;

            if (eventDataLen < 1)
                return -1;

            count = *eventData++;
            eventDataLen--;

            if (outBufLen > 0) {
                *outBuf++ = '[';
                outBufLen--;
            } else {
                goto no_room;
            }

            for (i = 0; i < count; i++) {
                result = android_log_printBinaryEvent(&eventData, &eventDataLen,
                        &outBuf, &outBufLen);
                if (result != 0)
                    goto bail;

                if (i < count-1) {
                    if (outBufLen > 0) {
                        *outBuf++ = ',';
                        outBufLen--;
                    } else {
                        goto no_room;
                    }
                }
            }

            if (outBufLen > 0) {
                *outBuf++ = ']';
                outBufLen--;
            } else {
                goto no_room;
            }
        }
        break;
    default:
        fprintf(stderr, "Unknown binary event type %d\n", type);
        return -1;
    }

bail:
    *pEventData = eventData;
    *pEventDataLen = eventDataLen;
    *pOutBuf = outBuf;
    *pOutBufLen = outBufLen;
    return result;

no_room:
    result = 1;
    goto bail;
}

/**
 * Convert a binary log entry to ASCII form.
 *
 * For convenience we mimic the processLogBuffer API.  There is no
 * pre-defined output length for the binary data, since we're free to format
 * it however we choose, which means we can't really use a fixed-size buffer
 * here.
 */
int android_log_processBinaryLogBuffer(struct logger_entry *buf,
    AndroidLogEntry *entry, const EventTagMap* map, char* messageBuf,
    int messageBufLen)
{
    size_t inCount;
    unsigned int tagIndex;
    const unsigned char* eventData;

    entry->tv_sec = buf->sec;
    entry->tv_nsec = buf->nsec;
    entry->priority = ANDROID_LOG_INFO;
    entry->pid = buf->pid;
    entry->tid = buf->tid;

    /*
     * Pull the tag out.
     */
    eventData = (const unsigned char*) buf->msg;
    struct logger_entry_v2 *buf2 = (struct logger_entry_v2 *)buf;
    if (buf2->hdr_size) {
        eventData = ((unsigned char *)buf2) + buf2->hdr_size;
    }
    inCount = buf->len;
    if (inCount < 4)
        return -1;
    tagIndex = get4LE(eventData);
    eventData += 4;
    inCount -= 4;

    if (map != NULL) {
        entry->tag = android_lookupEventTag(map, tagIndex);
    } else {
        entry->tag = NULL;
    }

    /*
     * If we don't have a map, or didn't find the tag number in the map,
     * stuff a generated tag value into the start of the output buffer and
     * shift the buffer pointers down.
     */
    if (entry->tag == NULL) {
        int tagLen;

        tagLen = snprintf(messageBuf, messageBufLen, "[%d]", tagIndex);
        entry->tag = messageBuf;
        messageBuf += tagLen+1;
        messageBufLen -= tagLen+1;
    }

    /*
     * Format the event log data into the buffer.
     */
    char* outBuf = messageBuf;
    size_t outRemaining = messageBufLen-1;      /* leave one for nul byte */
    int result;
    result = android_log_printBinaryEvent(&eventData, &inCount, &outBuf,
                &outRemaining);
    if (result < 0) {
        fprintf(stderr, "Binary log entry conversion failed\n");
        return -1;
    } else if (result == 1) {
        if (outBuf > messageBuf) {
            /* leave an indicator */
            *(outBuf-1) = '!';
        } else {
            /* no room to output anything at all */
            *outBuf++ = '!';
            outRemaining--;
        }
        /* pretend we ate all the data */
        inCount = 0;
    }

    /* eat the silly terminating '\n' */
    if (inCount == 1 && *eventData == '\n') {
        eventData++;
        inCount--;
    }

    if (inCount != 0) {
        fprintf(stderr,
            "Warning: leftover binary log data (%zu bytes)\n", inCount);
    }

    /*
     * Terminate the buffer.  The NUL byte does not count as part of
     * entry->messageLen.
     */
    *outBuf = '\0';
    entry->messageLen = outBuf - messageBuf;
    assert(entry->messageLen == (messageBufLen-1) - outRemaining);

    entry->message = messageBuf;

    return 0;
}

/*
 * One utf8 character at a time
 *
 * Returns the length of the utf8 character in the buffer,
 * or -1 if illegal or truncated
 *
 * Open coded from libutils/Unicode.cpp, borrowed from utf8_length(),
 * can not remove from here because of library circular dependencies.
 * Expect one-day utf8_character_length with the same signature could
 * _also_ be part of libutils/Unicode.cpp if its usefullness needs to
 * propagate globally.
 */
WEAK ssize_t utf8_character_length(const char *src, size_t len)
{
    const char *cur = src;
    const char first_char = *cur++;
    static const uint32_t kUnicodeMaxCodepoint = 0x0010FFFF;
    int32_t mask, to_ignore_mask;
    size_t num_to_read;
    uint32_t utf32;

    if ((first_char & 0x80) == 0) { /* ASCII */
        return 1;
    }

    /*
     * (UTF-8's character must not be like 10xxxxxx,
     *  but 110xxxxx, 1110xxxx, ... or 1111110x)
     */
    if ((first_char & 0x40) == 0) {
        return -1;
    }

    for (utf32 = 1, num_to_read = 1, mask = 0x40, to_ignore_mask = 0x80;
         num_to_read < 5 && (first_char & mask);
         num_to_read++, to_ignore_mask |= mask, mask >>= 1) {
        if (num_to_read > len) {
            return -1;
        }
        if ((*cur & 0xC0) != 0x80) { /* can not be 10xxxxxx? */
            return -1;
        }
        utf32 = (utf32 << 6) + (*cur++ & 0b00111111);
    }
    /* "first_char" must be (110xxxxx - 11110xxx) */
    if (num_to_read >= 5) {
        return -1;
    }
    to_ignore_mask |= mask;
    utf32 |= ((~to_ignore_mask) & first_char) << (6 * (num_to_read - 1));
    if (utf32 > kUnicodeMaxCodepoint) {
        return -1;
    }
    return num_to_read;
}

/*
 * Convert to printable from message to p buffer, return string length. If p is
 * NULL, do not copy, but still return the expected string length.
 */
static size_t convertPrintable(char *p, const char *message, size_t messageLen)
{
    char *begin = p;
    bool print = p != NULL;

    while (messageLen) {
        char buf[6];
        ssize_t len = sizeof(buf) - 1;
        if ((size_t)len > messageLen) {
            len = messageLen;
        }
        len = utf8_character_length(message, len);

        if (len < 0) {
            snprintf(buf, sizeof(buf),
                     ((messageLen > 1) && isdigit(message[1]))
                         ? "\\%03o"
                         : "\\%o",
                     *message & 0377);
            len = 1;
        } else {
            buf[0] = '\0';
            if (len == 1) {
                if (*message == '\a') {
                    strcpy(buf, "\\a");
                } else if (*message == '\b') {
                    strcpy(buf, "\\b");
                } else if (*message == '\t') {
                    strcpy(buf, "\\t");
                } else if (*message == '\v') {
                    strcpy(buf, "\\v");
                } else if (*message == '\f') {
                    strcpy(buf, "\\f");
                } else if (*message == '\r') {
                    strcpy(buf, "\\r");
                } else if (*message == '\\') {
                    strcpy(buf, "\\\\");
                } else if ((*message < ' ') || (*message & 0x80)) {
                    snprintf(buf, sizeof(buf), "\\%o", *message & 0377);
                }
            }
            if (!buf[0]) {
                strncpy(buf, message, len);
                buf[len] = '\0';
            }
        }
        if (print) {
            strcpy(p, buf);
        }
        p += strlen(buf);
        message += len;
        messageLen -= len;
    }
    return p - begin;
}
