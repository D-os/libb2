/**
 * Haiku syscalls emulation. Provided only to ease direct porting of
 * code from Haiku's implementation of libbe.
 * WARNING: Should not be used by newly written code and application code.
 */
#include "syscalls.h"

#include <StorageDefs.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/syscall.h>
#include <unistd.h>

struct linux_dirent {
	unsigned long  d_ino;	 /* Inode number */
	unsigned long  d_off;	 /* Offset to next linux_dirent */
	unsigned short d_reclen; /* Length of this linux_dirent */
	char		   d_name[]; /* Filename (null-terminated) */
};

#define WRAP_POSIX_CHECK(...) \
    if ((__VA_ARGS__) < 0) return B_FROM_POSIX_ERROR(errno);

#define WRAP_POSIX_CALL(...) \
    if ((__VA_ARGS__) < 0) return B_FROM_POSIX_ERROR(errno); \
    return B_OK;

#define WRAP_POSIX_RETURN(type, ...) \
    type ret = __VA_ARGS__; \
    if (ret < 0) return B_FROM_POSIX_ERROR(errno); \
    return ret;

#define CHECK_BAD_LEAF(path) \
    (path == NULL || path[0] == '\0' || strchr(path, '/') != NULL)

int _kern_dup(int fd)
{
    WRAP_POSIX_RETURN(int, dup(fd));
}

status_t _kern_close(int fd)
{
    WRAP_POSIX_CALL(close(fd));
}

status_t _kern_unlink(int fd, const char *path)
{
    if(CHECK_BAD_LEAF(path)) return B_BAD_VALUE;
    WRAP_POSIX_CALL(unlinkat(fd, path, 0));
}

status_t _kern_rename(int oldDir, const char *oldpath,
                      int newDir, const char *newpath)
{
    WRAP_POSIX_CALL(renameat(oldDir, oldpath, newDir, newpath));
}

status_t _kern_read_stat(int fd, const char *path, bool traverseLink,
                         struct stat *stat, size_t statSize)
{
    if(CHECK_BAD_LEAF(path)) return B_BAD_VALUE;
    int flags = 0;
    if (!traverseLink) {
        flags |= AT_SYMLINK_NOFOLLOW;
    }
    WRAP_POSIX_CALL(fstatat(fd, path, stat, flags));
}

extern status_t _kern_write_stat(int fd, const char *path,
								 bool traverseLink, const struct stat *stat,
								 size_t statSize, int statMask)
{
	abort();
}

status_t _kern_read_link(int fd, const char *path,
                         char *buffer, size_t *_bufferSize)
{
    if(CHECK_BAD_LEAF(path)) return B_BAD_VALUE;
    ssize_t written = readlinkat(fd, path, buffer, *_bufferSize);
    if (written < 0) {
        return B_FROM_POSIX_ERROR(errno);
    }

    *_bufferSize = (size_t)written;
    return B_OK;
}

/*
	Using SYS_getdents syscall directly, as this allows opening a directory
	using its file descriptor.
*/
ssize_t _kern_read_dir(int fd, struct dirent *buffer, size_t bufferSize, uint32 maxCount)
{
    intptr_t buf[(2 * sizeof(struct linux_dirent) + B_FILE_NAME_LENGTH + 2 * sizeof(char)) / sizeof(intptr_t) + 1];
    struct linux_dirent *dirent = (struct linux_dirent *)buf;
    size_t read = 0;
    while (maxCount--) {
        size_t buf_s = sizeof(struct linux_dirent);
        int nread = 0;
        while (buf_s <= sizeof(buf)) {
            nread = (int) syscall(SYS_getdents, fd, dirent, buf_s);
            if (nread >= 0 || errno != EINVAL) break;
            buf_s += sizeof(struct linux_dirent);
        }
        if (nread == 0) break;
        if (nread < 0) return B_FROM_POSIX_ERROR(errno);
        if (bufferSize < read + dirent->d_reclen) return B_BAD_VALUE;
        buffer->d_ino = dirent->d_ino;
        buffer->d_off = dirent->d_off;
        buffer->d_reclen = dirent->d_reclen;
        strncpy(buffer->d_name, dirent->d_name,
                min_c(sizeof(buffer->d_name), dirent->d_reclen - 1 - offsetof(struct linux_dirent, d_name)));
        buffer->d_type = *((unsigned char*)&dirent + dirent->d_reclen - 1);
        read += dirent->d_reclen;
        buffer += dirent->d_reclen;
    }
    return read;
}

status_t _kern_rewind_dir(int fd)
{
    WRAP_POSIX_CALL(lseek(fd, 0, SEEK_SET));
}

status_t _kern_create_symlink(int fd, const char *path,
                              const char *toPath, int mode)
{
    if(CHECK_BAD_LEAF(path)) return B_BAD_VALUE;
    WRAP_POSIX_CALL(symlinkat(toPath, fd, path));
}

int _kern_open(int fd, const char *path, int openMode, int perms)
{
    if(CHECK_BAD_LEAF(path)) return B_BAD_VALUE;
    WRAP_POSIX_RETURN(int, openat(fd, path, openMode, perms));
}
int _kern_open_entry_ref(int reffd, const char *leaf, int openMode, int perms)
{
    if(CHECK_BAD_LEAF(leaf)) return B_BAD_VALUE;
    return _kern_open(reffd, leaf, openMode, perms);
}

int _kern_open_dir(int fd, const char *path)
{
	if (fd >= 0 && CHECK_BAD_LEAF(path)) return B_BAD_VALUE;
	WRAP_POSIX_RETURN(int, openat(fd, path, O_DIRECTORY | O_CLOEXEC));
}
int _kern_open_dir_entry_ref(int reffd, const char *name)
{
    return _kern_open_dir(reffd, name);
}

int _kern_open_parent_dir(int fd, char *name, size_t nameLength)
{
    struct stat stat;
    if (fstatat(fd, "", &stat, AT_EMPTY_PATH) < 0) {
        return B_FROM_POSIX_ERROR(errno);
    }

    int parentfd = _kern_open_dir(fd, "..");
    if (parentfd < 0) {
        return parentfd;
    }

    struct dirent dirent;
    int size;
    while ((size = (int)_kern_read_dir(parentfd, &dirent, sizeof(dirent), 1)) > 0) {
        if (dirent.d_ino == stat.st_ino) {
            strlcpy(name, dirent.d_name, nameLength);
            break;
        }
    }
    if (size < 0) return size;

    _kern_rewind_dir(parentfd);

    return parentfd;
}

status_t _kern_create_dir(int fd, const char *path, int perms)
{
	if (fd >= 0 && CHECK_BAD_LEAF(path)) return B_BAD_VALUE;
	WRAP_POSIX_CALL(mkdirat(fd, path, (mode_t)perms));
}
status_t _kern_create_dir_entry_ref(int reffd, const char *leaf, int perms)
{
    return _kern_create_dir(reffd, leaf, perms);
}

status_t _kern_remove_dir(int fd, const char *path)
{
	if (fd >= 0 && CHECK_BAD_LEAF(path)) return B_BAD_VALUE;
	WRAP_POSIX_CALL(unlinkat(fd, path, AT_REMOVEDIR));
}

status_t _kern_fsync(int fd)
{
    WRAP_POSIX_CALL(fdatasync(fd));
}

off_t _kern_seek(int fd, off_t pos, int seekType)
{
    WRAP_POSIX_RETURN(off_t, lseek(fd, pos, seekType));
}

ssize_t _kern_read(int fd, off_t pos, void *buffer, size_t bufferSize)
{
    if (pos >= 0) {
        WRAP_POSIX_CHECK(lseek(fd, pos, SEEK_SET));
    }
    WRAP_POSIX_RETURN(ssize_t, read(fd, buffer, bufferSize));
}

ssize_t _kern_write(int fd, off_t pos, const void *buffer, size_t bufferSize)
{
    if (pos >= 0) {
        WRAP_POSIX_CHECK(lseek(fd, pos, SEEK_SET));
    }
    WRAP_POSIX_RETURN(ssize_t, write(fd, buffer, bufferSize));
}


status_t _kern_entry_ref_to_path(int reffd, const char *leaf,
                                 char *userPath, size_t pathLength)
{
    if(CHECK_BAD_LEAF(leaf)) return B_BAD_VALUE;

    struct stat statorig;
    if (fstatat(reffd, "", &statorig, AT_EMPTY_PATH) < 0) return B_BAD_VALUE;

    char *fdlink;
    if (asprintf(&fdlink, "/proc/self/fd/%d", reffd) < 0) return B_NO_MEMORY;

    ssize_t ret = readlink(fdlink, userPath, pathLength);
    free(fdlink);
    if (ret < 0 || (size_t)ret >= pathLength) return B_BAD_VALUE;

    userPath[ret] = '\0';
    struct stat statdest;
    if (fstatat(AT_FDCWD, userPath, &statdest, AT_SYMLINK_NOFOLLOW) < 0)
        return B_BAD_VALUE;
    if (statorig.st_dev != statdest.st_dev || statorig.st_ino != statdest.st_ino)
        return B_BAD_VALUE;

    size_t left = pathLength - (size_t)ret;
    int out = snprintf(userPath + ret, left, "/%s", leaf);
    if (out < 0 || (size_t)out >= left) return B_BAD_VALUE;


    return B_OK;
}

status_t _kern_lock_node(int fd)
{
    if (flock(fd, LOCK_EX | LOCK_NB) < 0) {
        switch (errno) {
        case EBADF:
            return B_FILE_ERROR;
        case EINTR:
        case EWOULDBLOCK:
            return B_BUSY;
        default:
            return  B_FROM_POSIX_ERROR(errno);
        }
    }
    return B_OK;
}

status_t _kern_unlock_node(int fd)
{
    if (flock(fd, LOCK_UN | LOCK_NB) < 0) {
        switch (errno) {
        case EBADF:
            return B_FILE_ERROR;
        case EINTR:
        case EWOULDBLOCK:
            return B_BAD_VALUE;
        default:
            return  B_FROM_POSIX_ERROR(errno);
        }
    }
    return B_OK;
}
