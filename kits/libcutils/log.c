#include <cutils/log.h>
#include <cutils/klog.h>
#include <stdlib.h>
#include <string.h>

void __log_write(int facility, int level, const char *tag, const char *msg)
{
    struct iovec vec[7];
    int n = 0;
    size_t marker_len, tag_len, msg_len = strlen(msg);
    if (msg_len > LOGGER_PAYLOAD_MAX_LEN) msg_len = LOGGER_PAYLOAD_MAX_LEN;

    char marker[8];
    marker_len = snprintf(marker, 7, "%d", (facility << 3) + (level & 07));

    vec[n].iov_base   = (void *) "<";
    vec[n++].iov_len  = 1;
    vec[n].iov_base   = marker;
    vec[n++].iov_len  = marker_len;
    vec[n].iov_base   = (void *) ">";
    vec[n++].iov_len  = 1;
    if (tag && tag[0] != '\0') {
        tag_len = strlen(tag);
        if (tag_len > LOGGER_PREFIX_MAX_LEN - 2) tag_len = LOGGER_PREFIX_MAX_LEN - 2;
        vec[n].iov_base   = (void *) tag;
        vec[n++].iov_len  = tag_len;
        vec[n].iov_base   = (void *) ": ";
        vec[n++].iov_len  = 2;
    }
    vec[n].iov_base   = (void *) msg;
    vec[n++].iov_len  = msg_len;
    vec[n].iov_base   = (void *) "\n";
    vec[n++].iov_len  = 1;

    klog_writev(level, vec, n);
}

void __log_vprint(int level, const char *tag, const char *fmt, va_list ap)
{
    char buf[LOGGER_PREFIX_MAX_LEN + LOGGER_PAYLOAD_MAX_LEN];

    vsnprintf(buf, sizeof(buf), fmt, ap);

    __log_write(0, level, tag, buf);
}

void __log_print(int level, const char *tag, const char *fmt, ...)
{
    va_list ap;
    char buf[LOGGER_PREFIX_MAX_LEN + LOGGER_PAYLOAD_MAX_LEN];

    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    __log_write(0, level, tag, buf);
}

void __log_assert(const char *cond, const char *tag,
                          const char *fmt, ...)
{
    char buf[LOGGER_PREFIX_MAX_LEN + LOGGER_PAYLOAD_MAX_LEN];

    if (fmt) {
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(buf, sizeof(buf), fmt, ap);
        va_end(ap);
    } else {
        /* Msg not provided, log condition.  N.B. Do not use cond directly as
         * format string as it could contain spurious '%' syntax (e.g.
         * "%d" in "blocks%devs == 0").
         */
        if (cond)
            snprintf(buf, sizeof(buf), "Assertion failed: %s", cond);
        else
            strcpy(buf, "Unspecified assertion failed");
    }

    __log_write(0, LOG_EMERG, tag, buf);
    abort(); /* abort so we have a chance to debug the situation */
    /* NOTREACHED */
}

int __log_is_loggable(int prio, const char *tag, int def)
{
    int logLevel = def;
    return logLevel >= 0 && prio >= logLevel;
}
