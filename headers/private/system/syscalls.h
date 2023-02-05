/*
 * Copyright 2004-2015, Haiku, Inc. All rights reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef _SYSTEM_SYSCALLS_H
#define _SYSTEM_SYSCALLS_H

#include <SupportDefs.h>
#include <dirent.h>
#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif

#define __NO_RETURN __attribute__((noreturn))

// // VFS functions
// extern dev_t	_kern_mount(const char *path, const char *device,
// 							const char *fs_name, uint32 flags, const char *args,
// 							size_t argsLength);
// extern status_t _kern_unmount(const char *path, uint32 flags);
// extern status_t _kern_read_fs_info(dev_t device, struct fs_info *info);
// extern status_t _kern_write_fs_info(dev_t device, const struct fs_info *info, int mask);
// extern dev_t	_kern_next_device(int32 *_cookie);
// extern status_t _kern_sync(void);
extern status_t _kern_entry_ref_to_path(int reffd, const char *leaf, char *userPath, size_t pathLength);
// extern status_t _kern_normalize_path(const char *userPath, bool traverseLink, char *buffer);
extern int _kern_open_entry_ref(int reffd, const char *leaf, int openMode, int perms);
extern int _kern_open(int fd, const char *path, int openMode, int perms);
extern int _kern_open_dir_entry_ref(int reffd, const char *leaf);
extern int _kern_open_dir(int fd, const char *path);
extern int _kern_open_parent_dir(int fd, char *name, size_t nameLength);
// extern status_t _kern_fcntl(int fd, int op, size_t argument);
extern status_t _kern_fsync(int fd);
// extern status_t _kern_flock(int fd, int op);
extern off_t	_kern_seek(int fd, off_t pos, int seekType);
extern status_t _kern_create_dir_entry_ref(int reffd, const char *leaf, int perms);
extern status_t _kern_create_dir(int fd, const char *path, int perms);
extern status_t _kern_remove_dir(int fd, const char *path);
extern status_t _kern_read_link(int fd, const char *path, char *buffer, size_t *_bufferSize);
extern status_t _kern_create_symlink(int fd, const char *path, const char *toPath, int mode);
// extern status_t _kern_create_link(int pathFD, const char *path, int toFD,
// 								  const char *toPath, bool traverseLeafLink);
extern status_t _kern_unlink(int fd, const char *path);
extern status_t _kern_rename(int oldDir, const char *oldpath, int newDir, const char *newpath);
// extern status_t _kern_create_fifo(int fd, const char *path, mode_t perms);
// extern status_t _kern_create_pipe(int *fds);
// extern status_t _kern_access(int fd, const char *path, int mode, bool effectiveUserGroup);
// extern ssize_t		_kern_select(int numfds, struct fd_set *readSet,
//                         struct fd_set *writeSet, struct fd_set *errorSet,
//                         bigtime_t timeout, const sigset_t *sigMask);
// extern ssize_t		_kern_poll(struct pollfd *fds, int numFDs,
//                         bigtime_t timeout);

// extern int		_kern_open_attr_dir(int fd, const char *path,
// 									bool traverseLeafLink);
// extern ssize_t	_kern_read_attr(int fd, const char *attribute, off_t pos,
// 								void *buffer, size_t readBytes);
// extern ssize_t	_kern_write_attr(int fd, const char *attribute, uint32 type,
// 								 off_t pos, const void *buffer, size_t readBytes);
// extern status_t _kern_stat_attr(int fd, const char *attribute,
// 								struct attr_info *attrInfo);
// extern int		_kern_open_attr(int fd, const char *path, const char *name,
// 								uint32 type, int openMode);
// extern status_t _kern_remove_attr(int fd, const char *name);
// extern status_t _kern_rename_attr(int fromFile, const char *fromName,
// 								  int toFile, const char *toName);
// extern int		_kern_open_index_dir(dev_t device);
// extern status_t _kern_create_index(dev_t device, const char *name,
// 								   uint32 type, uint32 flags);
// extern status_t _kern_read_index_stat(dev_t device, const char *name,
// 									  struct stat *stat);
// extern status_t _kern_remove_index(dev_t device, const char *name);
// extern status_t _kern_getcwd(char *buffer, size_t size);
// extern status_t _kern_setcwd(int fd, const char *path);
// extern int		_kern_open_query(dev_t device, const char *query,
// 								 size_t queryLength, uint32 flags, port_id port,
// 								 int32 token);

// // file descriptor functions
extern ssize_t _kern_read(int fd, off_t pos, void *buffer, size_t bufferSize);
// extern ssize_t	_kern_readv(int fd, off_t pos, const struct iovec *vecs, size_t count);
extern ssize_t _kern_write(int fd, off_t pos, const void *buffer, size_t bufferSize);
// extern ssize_t	_kern_writev(int fd, off_t pos, const struct iovec *vecs, size_t count);
// extern status_t _kern_ioctl(int fd, uint32 cmd, void *data, size_t length);
extern ssize_t	_kern_read_dir(int fd, struct dirent *buffer, size_t bufferSize, uint32 maxCount);
extern status_t _kern_rewind_dir(int fd);
extern status_t _kern_read_stat(int fd, const char *path, bool traverseLink,
								struct stat *stat, size_t statSize);
extern status_t _kern_write_stat(int fd, const char *path,
								 bool traverseLink, const struct stat *stat,
								 size_t statSize, int statMask);
extern status_t _kern_close(int fd);
extern int		_kern_dup(int fd);
// extern int		_kern_dup2(int ofd, int nfd);
extern status_t _kern_lock_node(int fd);
extern status_t _kern_unlock_node(int fd);
// extern status_t _kern_get_next_fd_info(team_id team, uint32 *_cookie,
// 									   struct fd_info *info, size_t infoSize);

#undef __NO_RETURN

#ifdef __cplusplus
}
#endif

#endif /* _SYSTEM_SYSCALLS_H */
