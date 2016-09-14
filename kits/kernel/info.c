#include <KernelKit.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/statvfs.h>
#include <sys/sysmacros.h>
#include <stdio.h>
#include <string.h>

long dev_for_path(const char *path)
{
    if (!path || path[0] == '\0')
        return B_BAD_VALUE;
    struct stat sb;
    if (stat(path, &sb) < 0) {
        switch (errno) {
        case EACCES:
        case ENOENT:
        case ENOTDIR:
            return B_ENTRY_NOT_FOUND;
        case EBADF:
        case EINVAL:
            return B_BAD_VALUE;
        case EFAULT:
        case ELOOP:
        case EOVERFLOW:
            return B_FILE_ERROR;
        case ENAMETOOLONG:
            return B_NAME_TOO_LONG;
        case ENOMEM:
            return B_NO_MEMORY;
        }
    }
    return sb.st_dev;
}

long next_dev(int32 *pos)
{
    char buffer[32+NAME_MAX];
    FILE* fp = fopen("/proc/partitions", "r");
    bool header = false;
    unsigned int maj, min;
    int32 line = 0;
    while (fgets(buffer, sizeof(buffer), fp)) {
        if (!header) {
            if (strlen(buffer) < 2) header = true;
            continue;
        }
        if (line++ == *pos) {
            if (sscanf(buffer, "%u %u", &maj, &min) < 2) {
                fclose(fp);
                return B_FILE_ERROR;
            }
            *pos = line;
            return makedev(maj, min);
        }
    }
    fclose(fp);
    return B_BAD_VALUE;
}

int fs_stat_dev(dev_t dev, fs_info *info)
{
    if (!info)
        return B_BAD_VALUE;
    unsigned int maj, min;
    maj = major(dev);
    min = minor(dev);
    // UINT_MAX	is 4294967295 is 10 characters
    char dev_str[2*11];
    sprintf(dev_str, "%u:%u", maj, min);

    FILE* fp = fopen("/proc/self/mountinfo", "r");
    if (!fp) {
        return B_FILE_ERROR;
    }
    // 16 59 0:16 / /sys rw,nosuid,nodev,noexec,relatime shared:6 - sysfs sysfs rw
    char buffer[4*11 + 2*PATH_MAX];
    int n;
    char *token = NULL, *saveptr;
    char *path, *fs, *device;
    while (fgets(buffer, sizeof(buffer), fp)) {
        n = 0;
        // look for 3rd token
        while (n < 3 && (token = strtok_r((n ? NULL : buffer), " \t", &saveptr))) n++;
        if (!token) {
            fclose(fp);
            return B_FILE_ERROR;
        }
        if (strcmp(token, dev_str) != 0) continue; // read next line

        fclose(fp);
        // look for 5th token
        while (n < 5 && (token = strtok_r(NULL, " \t", &saveptr))) n++;
        if (!token) return B_FILE_ERROR;
        path = token;

        while ((token = strtok_r((n ? NULL : buffer), " \t", &saveptr)) && *token != '-');
        if (!token) return B_FILE_ERROR;

        token = strtok_r(NULL, " \t", &saveptr);
        if (!token) return B_FILE_ERROR;
        fs = token;

        token = strtok_r(NULL, " \t", &saveptr);
        if (!token) return B_FILE_ERROR;
        device = token;

        struct statvfs vb;
        if (statvfs(path, &vb) < 0) {
            return B_BAD_VALUE;
        }
        struct stat sb;
        if (stat(path, &sb) < 0) {
            return B_BAD_VALUE;
        }

        uint32 flags = 0;
        if (vb.f_flag & ST_RDONLY) flags |= B_FS_IS_READONLY;
        // FIXME! implement rest of the flags

        memset(info, 0, sizeof(*info));
        info->dev = sb.st_dev;
        info->root = sb.st_ino;
        info->flags = flags;
        info->block_size = vb.f_bsize;
        info->io_size = vb.f_frsize;
        info->total_blocks = vb.f_blocks;
        info->free_blocks = vb.f_bfree;
        info->total_nodes = vb.f_files;
        info->free_nodes = vb.f_ffree;
        strncpy(info->device_name, device, 128);
//        char	volume_name[B_FILE_NAME_LENGTH];	/* volume name */
        strncpy(info->fsh_name, fs, B_OS_NAME_LENGTH);

        return B_OK;
    }
    // dev_t not found
    fclose(fp);
    return B_BAD_VALUE;
}
