/*
 * socket.h
 * Copyright (c) 1995-97 Be, Inc.	All Rights Reserved 
 *
 * BSD socket-like interface
 *
 * Do not expect total BSD compatibility from this interface!
 */
#ifndef _SOCKET_H
#define _SOCKET_H

#include <BeBuild.h>
#include <sys/types.h>
#include <sys/time.h>       /* for timeval/timezone structs & gettimeofday */
#include <ByteOrder.h>

#if __cplusplus
extern "C" {
#endif /* __cplusplus */

#define AF_INET 1


#define INADDR_ANY			0x00000000	
#define INADDR_BROADCAST	0xffffffff
#define INADDR_LOOPBACK		0x7f000001	/* in host order */

#define SOL_SOCKET 1

#define SO_DEBUG 1
#define SO_REUSEADDR 2
#define SO_NONBLOCK 3
#define SO_REUSEPORT 4
#define SO_FIONREAD	5

#define MSG_OOB 0x1

#define SOCK_DGRAM 1
#define SOCK_STREAM 2

#define IPPROTO_UDP 1
#define IPPROTO_TCP 2
#define IPPROTO_ICMP 3

/* 
 * Be extension
 */
#define B_UDP_MAX_SIZE (65536 - 1024) 

struct sockaddr {
	unsigned short sa_family;
	char sa_data[10];
};

struct in_addr {
	unsigned int s_addr;
};

struct sockaddr_in {
	unsigned short sin_family;
	unsigned short sin_port;
	struct in_addr sin_addr;
	char sin_zero[4];
};

/*
 * You can define your own FDSETSIZE if you want more bits
 */

#ifndef FD_SETSIZE
#define FD_SETSIZE 256
#endif /* FD_SETSIZE */

/*
 * Compatibily only: use FD_SETSIZE instead
 */
#ifndef FDSETSIZE
#define FDSETSIZE FD_SETSIZE
#endif /* FDSETSIZE */

#define NFDBITS 32

typedef struct fd_set {
	unsigned mask[FDSETSIZE / NFDBITS];
} fd_set;

#define _FDMSKNO(fd) ((fd) / NFDBITS)
#define _FDBITNO(fd) ((fd) % NFDBITS)
#define FD_ZERO(setp) memset((setp)->mask, 0, sizeof((setp)->mask))
#define FD_SET(fd, setp) ((setp)->mask[_FDMSKNO(fd)] |= (1 << (_FDBITNO(fd))))
#define FD_CLR(fd, setp) ((setp)->mask[_FDMSKNO(fd)] &= ~(1 << (_FDBITNO(fd))))
#define FD_ISSET(fd, setp) ((setp)->mask[_FDMSKNO(fd)] & (1 << (_FDBITNO(fd))))


_IMPEXP_NET int socket(int family, int type, int proto);
_IMPEXP_NET int bind(int fd, const struct sockaddr *addr, int size);
_IMPEXP_NET int getsockname(int fd, struct sockaddr *addr, int *size);
_IMPEXP_NET int getpeername(int fd, struct sockaddr *addr, int *size);
_IMPEXP_NET ssize_t recvfrom(int fd, void *buf, size_t size, int flags,
			 struct sockaddr *from, int *fromlen);
_IMPEXP_NET ssize_t sendto(int fd, const void *buf, size_t size, int flags,
		   const struct sockaddr *to, int tolen);

_IMPEXP_NET ssize_t send(int fd, const void *buf, size_t size, int flags);
_IMPEXP_NET ssize_t recv(int fd, void *buf, size_t size, int flags);


_IMPEXP_NET int connect(int fd, const struct sockaddr *addr, int size);
_IMPEXP_NET int accept(int fd, struct sockaddr *addr, int *size);


_IMPEXP_NET int listen(int fd, int backlog);
_IMPEXP_NET int closesocket(int fd);

_IMPEXP_NET int shutdown(int fd, int how);  /* doesn't work yet */

_IMPEXP_NET int setsockopt(int sd, int prot, int opt, const void *data, unsigned datasize);
_IMPEXP_NET int getsockopt(int sd, int prot, int opt, void *data, int *datasize);

_IMPEXP_NET int select(int nbits, 
		   struct fd_set *rbits, 
		   struct fd_set *wbits, 
		   struct fd_set *ebits, 
		   struct timeval *timeout);


#if __cplusplus
}
#endif /* __cplusplus */

#endif /* _SOCKET_H */
