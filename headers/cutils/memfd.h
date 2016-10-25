#ifndef _CUTILS_MEMFD_H
#define _CUTILS_MEMFD_H


#include <unistd.h>
#include <sys/syscall.h>
#include <linux/memfd.h>

/* From <linux/fcntl.h> */
#define F_LINUX_SPECIFIC_BASE	1024
/*
 * Set/Get seals
 */
#define F_ADD_SEALS	(F_LINUX_SPECIFIC_BASE + 9)
#define F_GET_SEALS	(F_LINUX_SPECIFIC_BASE + 10)

/*
 * Types of seals
 */
#define F_SEAL_SEAL	0x0001	/* prevent further seals from being set */
#define F_SEAL_SHRINK	0x0002	/* prevent file from shrinking */
#define F_SEAL_GROW	0x0004	/* prevent file from growing */
#define F_SEAL_WRITE	0x0008	/* prevent writes */
/* (1U << 31) is reserved for signed error codes */


#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#define memfd_create(...)	(int)syscall(SYS_memfd_create, __VA_ARGS__)

#endif	/* _CUTILS_MEMFD_H */
