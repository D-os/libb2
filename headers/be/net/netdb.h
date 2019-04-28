/*
 * netdb.h
 * Copyright (c) 1995-97 Be, Inc.	All Rights Reserved 
 *
 * BSD network database-like interface
 *
 * Do not expect total BSD compatibility from this interface!
 *
 */
#ifndef _NETDB_H
#define _NETDB_H

#include <BeBuild.h>

#if __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef _SOCKET_H
#include <socket.h>
#endif /* _SOCKET_H */

#define MAXHOSTNAMELEN 64

#define HOST_NOT_FOUND 1
#define TRY_AGAIN 2
#define NO_RECOVERY 3
#define NO_DATA 4

#ifndef h_errno
extern _IMPEXP_NET int *_h_errnop(void);
#define h_errno (*_h_errnop())
#endif /* h_errno */


struct hostent {
	char *h_name;
	char **h_aliases;
	int h_addrtype;
	int h_length;
	char **h_addr_list;
};
#define h_addr h_addr_list[0]

struct servent {
	char *s_name;
	char **s_aliases;
	int s_port;
	char *s_proto;
};	

extern _IMPEXP_NET struct hostent *gethostbyname(const char *hostname);
extern _IMPEXP_NET struct hostent *gethostbyaddr(const char *hostname, int len, int type);
extern _IMPEXP_NET struct servent *getservbyname(const char *name, const char *proto);
extern _IMPEXP_NET void herror(const char *);
extern _IMPEXP_NET unsigned long inet_addr(const char *a_addr);
extern _IMPEXP_NET char *inet_ntoa(struct in_addr addr);


extern _IMPEXP_NET int gethostname(char *hostname, size_t hostlen);

/* BE specific, because of lack of UNIX passwd functions */
extern _IMPEXP_NET int getusername(char *username, size_t userlen);
extern _IMPEXP_NET int getpassword(char *password, size_t passlen);

#if __cplusplus
}
#endif /* __cplusplus */

#endif /* _NETDB_H */
