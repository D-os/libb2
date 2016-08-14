#include <OS.h>

#include <stdlib.h>
#include <stddef.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <poll.h>
#include <string.h>
#include <errno.h>

#include "private.h"
#include "utlist.h"
#include "rwlock.h"

#define MAX_WRITE_RETRIES   50
#define MAX_WRITE_SNOOZE    10000

typedef struct _port_info_struct {
    int     fd;
    char    name[B_OS_NAME_LENGTH];
    size_t  namelen;
    int32   capacity;
    bool    bound;
    struct _port_info_struct *next;
    struct _port_info_struct *prev;
} _port_info;

/* head of linked list of all _port_info */
static _port_info *_ports = NULL;
RWLOCK(_ports)

static socklen_t _fill_sockaddr(struct sockaddr_un *address, _port_info *info)
{
    memset(address, 0, sizeof(struct sockaddr_un));
    address->sun_family = AF_UNIX;
    address->sun_path[0] = '\0';
    socklen_t len = min_c(info->namelen, sizeof(address->sun_path) - 1);
    strncpy(&address->sun_path[1], info->name, len);
    return offsetof(struct sockaddr_un, sun_path) + 1 + len;
}

/* WARNING! you need to lock _threads in caller function! */
static _port_info *_find_port_info(port_id port)
{
    _port_info *info;
    DL_SEARCH_SCALAR(_ports, info, fd, port);
    return info;
}

static _port_info *_bound_port_info(port_id port)
{
    _port_info *info = _find_port_info(port);
    if (info && !info->bound) {
        _ports_unlock();
        _ports_wlock();
        _port_info *info = _find_port_info(port);
        if (!info->bound) {
            struct sockaddr_un address;
            socklen_t addrlen = _fill_sockaddr(&address, info);

            if (bind(info->fd, (struct sockaddr*)&address, addrlen) < 0) {
                return NULL;
            }

            info->bound = true;
        }
    }
    return info;
}

#define _port_fd_check_recv \
    if (flags & B_RELATIVE_TIMEOUT) {   \
        recvflags |= MSG_DONTWAIT;      \
                                        \
        struct timespec tm;             \
        tm.tv_sec = timeout / 1000000;  \
        tm.tv_nsec = (timeout % 1000000) * 1000;            \
        struct pollfd pfd = { .fd = fd, .events = POLLIN }; \
        int ret = ppoll(&pfd, 1, &tm, NULL);    \
        if (ret == 0) return B_TIMED_OUT;       \
        if (ret < 0) {          \
            switch (errno) {    \
            case EINTR:         \
                return B_INTERRUPTED;   \
            case ENOMEM:                \
                return B_NO_MEMORY;     \
            }   \
        }       \
    }

ssize_t port_buffer_size_etc(port_id port, uint32 flags, bigtime_t timeout)
{
    int fd = -1;
    _ports_rlock();
    _port_info *info = _bound_port_info(port);
    if (info) fd = info->fd;
    _ports_unlock();
    if (fd < 0) return B_BAD_PORT_ID;

    int recvflags = MSG_PEEK | MSG_TRUNC;
    _port_fd_check_recv;

    ssize_t size = recv(fd, NULL, 0, recvflags);

    if (size < 0) {
        return B_BAD_PORT_ID;
    }

    return size;
}

ssize_t port_buffer_size(port_id port)
{
    return port_buffer_size_etc(port, 0, 0);
}

ssize_t port_count(port_id port)
{
    int fd = -1;
    _ports_rlock();
    _port_info *info = _bound_port_info(port);
    if (info) fd = info->fd;
    _ports_unlock();
    if (fd < 0) return B_BAD_PORT_ID;

    ssize_t count = 0;
    int ov = 0;
    ssize_t size;

    while ((size = recv(fd, NULL, 0, MSG_DONTWAIT | MSG_PEEK | MSG_TRUNC)) > 0) {
        ov += size;
        setsockopt(fd, SOL_SOCKET, SO_PEEK_OFF, &ov, sizeof(ov));
        count++;
    }

    size = -1;
    setsockopt(fd, SOL_SOCKET, SO_PEEK_OFF, &size, sizeof(size));

    return count;
}

ssize_t read_port_etc(port_id port, int32 *code, void *buffer,
                      size_t bufferSize, uint32 flags, bigtime_t timeout)
{
    int fd = -1;
    _ports_rlock();
    _port_info *info = _bound_port_info(port);
    if (info) fd = info->fd;
    _ports_unlock();
    if (fd < 0) return B_BAD_PORT_ID;

    struct msghdr msg = {};

    struct iovec iov[2] = {};
    msg.msg_iov = &iov[0];
    msg.msg_iovlen = count_of(iov);

    iov[0].iov_base = code;
    iov[0].iov_len = sizeof(*code);
    iov[1].iov_base = buffer;
    iov[1].iov_len = bufferSize;

    int recvflags = MSG_CMSG_CLOEXEC;
    _port_fd_check_recv;

    ssize_t read = recvmsg(fd, &msg, recvflags);
    if (read < 0) {
        switch (errno) {
        case EBADF:
        case ECONNREFUSED:
        case EFAULT:
        case ENOTCONN:
        case ENOTSOCK:
            return B_BAD_PORT_ID;
        case EAGAIN: //case EWOULDBLOCK:
            return B_WOULD_BLOCK;
        case EINTR:
            return B_INTERRUPTED;
        case EINVAL:
        case EMSGSIZE:
            return B_BAD_VALUE;
        case ENOMEM:
            return B_NO_MEMORY;
        default:
            return B_FROM_POSIX_ERROR(errno);
        }
    }

    return read - sizeof(*code);
}

ssize_t read_port(port_id port, int32 *code, void *buffer, size_t bufferSize)
{
    return read_port_etc(port, code, buffer, bufferSize, 0, 0);
}


status_t write_port_etc(port_id port, int32 code, const void *buffer,
                        size_t bufferSize, uint32 flags, bigtime_t timeout)
{
    int fd = -1;
    struct sockaddr_un address;
    struct msghdr msg = {};
    msg.msg_name = (struct sockaddr*)&address;

    _ports_rlock();
    _port_info *info = _find_port_info(port);
    msg.msg_namelen = _fill_sockaddr(&address, info);
    if (info) fd = info->fd;
    _ports_unlock();
    if (fd < 0) return B_BAD_PORT_ID;

    struct iovec iov[2] = {};
    msg.msg_iov = &iov[0];
    msg.msg_iovlen = count_of(iov);

    iov[0].iov_base = &code;
    iov[0].iov_len = sizeof(code);
    iov[1].iov_base = (void *)buffer;
    iov[1].iov_len = bufferSize;

    unsigned retries = MAX_WRITE_RETRIES;
    bigtime_t retrysnooze = MAX_WRITE_SNOOZE;

    int sendflags = MSG_NOSIGNAL;

    if (flags & B_RELATIVE_TIMEOUT) {
        sendflags |= MSG_DONTWAIT;
        if (retrysnooze > timeout / retries) retrysnooze = timeout / retries;
    }

retry:
    if (sendmsg(fd, &msg, sendflags) < 0) {
        if (errno == ECONNREFUSED && --retries) {
            snooze(retrysnooze);
            goto retry;
        }
        if (errno == EWOULDBLOCK && flags & B_RELATIVE_TIMEOUT) {
            if (timeout == 0) return B_WOULD_BLOCK;

            struct timespec tm;
            tm.tv_sec = timeout / 1000000;
            tm.tv_nsec = (timeout % 1000000) * 1000;
            struct pollfd pfd = { .fd = fd, .events = POLLOUT };
            int ret = ppoll(&pfd, 1, &tm, NULL);
            if (ret > 0) goto retry;
            if (ret == 0) return B_TIMED_OUT;
            // ret is -1 -> pass to processing errno
        }

        switch (errno) {
        case EBADF:
        case ECONNRESET:
        case EDESTADDRREQ:
        case EISCONN:
        case ENOTCONN:
        case ENOTSOCK:
        case EPIPE:
            return B_BAD_PORT_ID;
        case EAGAIN: //case EWOULDBLOCK:
            return B_WOULD_BLOCK;
        case EINTR:
            return B_INTERRUPTED;
        case EFAULT:
        case EINVAL:
        case EMSGSIZE:
        case ENOBUFS:
        case EOPNOTSUPP:
            return B_BAD_VALUE;
        case ENOMEM:
            return B_NO_MEMORY;
        default:
            return B_FROM_POSIX_ERROR(errno);
        }
    }
    return B_OK;
}

status_t write_port(port_id port, int32 code, const void *buffer, size_t bufferSize)
{
    return write_port_etc(port, code, buffer, bufferSize, 0, 0);
}

port_id create_port(int32 capacity, const char *name)
{
    if (!name || name[0] == '\0') {
        return B_BAD_VALUE;
    }

    int fd = socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, 0);
    if (fd < 0) {
        return B_NO_MORE_PORTS;
    }

    _port_info *info = calloc(1, sizeof(_port_info));
    info->fd = fd;
    size_t len = strlen(name);
    info->namelen = min_c(len, B_OS_NAME_LENGTH);
    strncpy(info->name, name, info->namelen);
    info->capacity = capacity;

    _ports_wlock();
    DL_APPEND(_ports, info);
    _ports_unlock();

    return info->fd;
}

status_t delete_port(port_id port)
{
    _ports_wlock();
    _port_info *info = _find_port_info(port);
    if (info) {
        DL_DELETE(_ports, info);
    }
    _ports_unlock();
    if (!info) return B_BAD_PORT_ID;

    int fd = info->fd;
    free(info);

    if (close(fd) < 0) {
        switch (errno) {
        case EBADF:
            return B_BAD_PORT_ID;
        case EINTR:
            return B_INTERRUPTED;
        default:
            return B_FROM_POSIX_ERROR(errno);
        }
    }

    return info->fd;
}

status_t close_port(port_id port)
{
    int fd = -1;
    _ports_rlock();
    _port_info *info = _find_port_info(port);
    if (info) fd = info->fd;
    _ports_unlock();
    if (fd < 0) return B_BAD_PORT_ID;

    if (shutdown(fd, SHUT_WR) < 0) {
        switch (errno) {
        case EBADF:
        case ENOTSOCK:
            return B_BAD_PORT_ID;
        default:
            return B_FROM_POSIX_ERROR(errno);
        }
    }
    return B_OK;
}

status_t _get_port_info(port_id port, port_info *portInfo, size_t portInfoSize)
{
    _ports_rlock();
    _port_info *info = _find_port_info(port);
    if (!info) {
        _ports_unlock();
        return B_BAD_PORT_ID;
    }
    memset(portInfo, 0, portInfoSize);
    portInfo->port = port;
    portInfo->team = _info->team;
    strncpy(portInfo->name, info->name, B_OS_NAME_LENGTH);
    portInfo->capacity = info->capacity;
    portInfo->queue_count = (int32) port_count(port);
//    portInfo->total_count =
    return B_OK;
}
