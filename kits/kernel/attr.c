#include <OS.h>
#include <KernelKit.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/xattr.h>

#define XATTR_SIZE_READ_RETRIES 8

// prefix attribute name with user. namespace
#define GEN_ATTRIBUTE_NAME \
    char *name; \
    asprintf(&name, "user.%s", attribute);

struct _attr_dir {
    char    *curr;
    char    *end;
    char    *list;
    struct dirent value;
};

DIR *fs_fopen_attr_dir(int fd)
{
    if (fd < 0) {
        errno = B_FILE_ERROR;
        return NULL;
    }

    char *buf = NULL;
    ssize_t len, read;
    unsigned retries = XATTR_SIZE_READ_RETRIES;
    while (--retries) {
        if (buf) free(buf);
        len = flistxattr(fd, NULL, 0);
        if (len < 0) return NULL;
        buf = malloc(len);
        if (!buf) return NULL;
        read = flistxattr(fd, buf, len);
        if (read < 0) {
            free(buf);
            errno = B_FILE_ERROR;
            return NULL;
        }
        if (read == len) {
            break;
        }
    }
    if (retries) {
        struct _attr_dir *ret = malloc(sizeof(struct _attr_dir));
        ret->list = ret->curr = buf;
        ret->end = ret->list + read;
        return (DIR *)ret;
    }
    return NULL;
}

void fs_rewind_attr_dir(DIR *dir)
{
    if (!dir) {
        errno = B_FILE_ERROR;
    }

    ((struct _attr_dir *)dir)->curr = 0;
}

int fs_close_attr_dir(DIR *dir)
{
    if (!dir) {
        errno = B_FILE_ERROR;
        return -1;
    }

    free(((struct _attr_dir *)dir)->list);
    free(dir);
    return 0;
}

struct dirent *fs_read_attr_dir(DIR *dir)
{
    if (!dir) {
        errno = B_FILE_ERROR;
        return NULL;
    }
    struct _attr_dir *attr_dir = (struct _attr_dir *)dir;

    bool read = false;
    while (!read) {
        if (attr_dir->curr >= attr_dir->end) {
            errno = B_ENTRY_NOT_FOUND;
            return NULL;
        }

        // skip non user. attributes
        if (strncmp(attr_dir->curr, "user.", sizeof("user.")-1) == 0) {
            read = (snprintf(attr_dir->value.d_name, sizeof(attr_dir->value.d_name),
                             // remove user. namespace
                             "%s", attr_dir->curr + sizeof("user.")-1)
                    ) > 0;
        }

        // move to next item
        while(*(attr_dir->curr++));
    }
    return &attr_dir->value;
}

int fs_stat_attr(int fd, const char *attribute, struct attr_info *attrInfo)
{
    if (fd < 0) {
        errno = B_FILE_ERROR;
        return fd;
    }
    if (!attribute || attribute[0] == '\0') {
        errno = B_BAD_VALUE;
        return -1;
    }
    if (!attrInfo) {
        errno = B_BAD_VALUE;
        return -1;
    }

    int ret = 0;
    GEN_ATTRIBUTE_NAME;

    char *buf = NULL;
    unsigned retries = XATTR_SIZE_READ_RETRIES;
    while (--retries) {
        if (buf) free(buf);
        attrInfo->size = fgetxattr(fd, name, NULL, 0);
        if (attrInfo->size < 1) {
            errno = B_ENTRY_NOT_FOUND;
            ret = -1;
            break;
        }
        buf = malloc(attrInfo->size);
        if (fgetxattr(fd, name, buf, attrInfo->size) < 1) {
            errno = B_ENTRY_NOT_FOUND;
            ret = -1;
        }
        else {
            attrInfo->type = *((typeof(attrInfo->type)*)buf);
            attrInfo->size -= sizeof(attrInfo->type);
        }
    }

    free(buf);
    free(name);
    return ret;
}

ssize_t fs_read_attr(int fd, const char *attribute, uint32 type,
                     off_t pos, void *buffer, size_t readBytes)
{
    if (fd < 0) {
        errno = B_FILE_ERROR;
        return fd;
    }
    if (!attribute || attribute[0] == '\0') {
        errno = B_BAD_VALUE;
        return -1;
    }
    if (pos < 0) {
        errno = B_BAD_VALUE;
        return -1;
    }

    int ret = 0;
    GEN_ATTRIBUTE_NAME;

    size_t size = pos + readBytes + sizeof(type);
    char *buf = malloc(size);
    size_t read = 0;
    if ((read = fgetxattr(fd, name, buf, size)) < 1) {
        errno = B_ENTRY_NOT_FOUND;
        ret = -1;
    }
    else if (sizeof(type) + pos + readBytes > read) {
        errno = B_BAD_VALUE;
        ret = -1;
    }
    else {
        memcpy(buffer, buf + sizeof(type) + pos, readBytes);
    }

    free(buf);
    free(name);
    return ret;
}

ssize_t fs_write_attr(int fd, const char *attribute, uint32 type,
                     off_t pos, const void *buffer, size_t count)
{
    if (fd < 0) {
        errno = B_FILE_ERROR;
        return fd;
    }
    if (!attribute || attribute[0] == '\0') {
        errno = B_BAD_VALUE;
        return -1;
    }
    if (pos != 0) {
        errno = B_BAD_VALUE;
        return -1;
    }

    GEN_ATTRIBUTE_NAME;

    // encode type as first data bytes
    size_t size = count + sizeof(type);
    char *value = malloc(size);
    *((typeof(type) *)value) = type;
    memcpy(value + sizeof(type), buffer, size);
    int ret = fsetxattr(fd, name, value, size, 0);

    free(name);
    free(value);

    return ret < 0 ? ret : (ssize_t)count;
}

int fs_remove_attr(int fd, const char *attribute)
{
    if (fd < 0) {
        errno = B_FILE_ERROR;
        return fd;
    }
    if (!attribute || attribute[0] == '\0') {
        errno = B_BAD_VALUE;
        return -1;
    }

    int ret = 0;
    GEN_ATTRIBUTE_NAME;
    if (fremovexattr(fd, name) < 0) {
        switch (errno) {
        case ENODATA:
            errno = B_ENTRY_NOT_FOUND;
            break;
        case ENOTSUP:
            errno = B_NOT_ALLOWED;
        default:
            errno = B_FILE_ERROR;
        }
        ret = -1;
    }

    free(name);
    return ret;
}


/******************* kernel stubs *******************/
int _kern_open_attr_dir(int fd, const char *path,
                        bool traverseLeafLink) {STUB; return 0;}
status_t _kern_remove_attr(int fd, const char *name) {STUB; return 0;}
status_t _kern_rename_attr(int fromFile, const char *fromName,
                        int toFile, const char *toName) {STUB; return 0;}
