#include <KernelKit.h>
#include <SupportKit.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    setbuf(stdout, NULL); // do not buffer

    DIR *d;
    dirent *ent;
    attr_info info;
    int fd;
    char *buffer;
    int ret = EXIT_SUCCESS;

    char temp[] = "/tmp/attrXXXXXX";
    fd = mkostemp(temp, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Error creating temp file: %d %s\n", errno, strerror(errno));
        return EXIT_FAILURE;
    }

    fprintf(stdout, "Using '%s' for attribute test file\n", temp);

    ssize_t written = fs_write_attr(fd, "test", B_STRING_TYPE, 0, ".", 1);
    if (written < 0) {
        fprintf(stderr, "Error writing attribute 'test': %d %s\n", errno, strerror(errno));
        ret = EXIT_FAILURE;
        goto exit;
    }
    written = fs_write_attr(fd, "test", B_STRING_TYPE, 0, "--test", 6);
    if (written < 0) {
        fprintf(stderr, "Error overwriting attribute 'test': %d %s\n", errno, strerror(errno));
        ret = EXIT_FAILURE;
        goto exit;
    }
    written = fs_write_attr(fd, "fooBar", B_INT64_TYPE, 0, "12345678", 8);
    if (written < 0) {
        fprintf(stderr, "Error writing attribute 'fooBar': %d %s\n", errno, strerror(errno));
        ret = EXIT_FAILURE;
        goto exit;
    }
    written = fs_write_attr(fd, "user.:/%!mode", B_ANY_TYPE, 0, "\0\0\0\0\0", 5);
    if (written < 0) {
        fprintf(stderr, "Error writing attribute 'user.mode': %d %s\n", errno, strerror(errno));
        ret = EXIT_FAILURE;
        goto exit;
    }
    written = fs_remove_attr(fd, "fooBar");
    if (written < 0) {
        fprintf(stderr, "Error removing attribute 'fooBar': %d %s\n", errno, strerror(errno));
        ret = EXIT_FAILURE;
        goto exit;
    }

    d = fs_fopen_attr_dir(fd);
    if (d) {
        while ((ent = fs_read_attr_dir(d))) {
            if (fs_stat_attr(fd, ent->d_name, &info) < 0) {
                fprintf(stderr, "Error stating attribute '%s': %d\n", ent->d_name, errno);
                ret = EXIT_FAILURE;
                goto exit;
            }
            buffer = (char *) malloc((size_t) info.size);
            if (!buffer) {
                fprintf(stderr, "No memory size %zd: %d\n", info.size, errno);
                ret = EXIT_FAILURE;
                goto exit;
            }

            off_t offset = 2;
            off_t len = info.size - offset;
            if (fs_read_attr(fd, ent->d_name, info.type, offset, buffer, (size_t)len) < 0) {
                fprintf(stderr, "Error reading attribute '%s' type " F_WHAT_FORMAT " size %zd: %d\n",
                        ent->d_name, F_WHAT_VALUES(info.type), info.size, errno);
                ret = EXIT_FAILURE;
                goto exit;
            }
            fprintf(stdout, "%s:" F_WHAT_FORMAT ":%zd:%.*s\n", ent->d_name, F_WHAT_VALUES(info.type), info.size, (int)len, buffer);
            free(buffer);
        }
        fs_close_attr_dir(d);
    } else {
        fprintf(stderr, "Error writing attribute 'test': %d\n", errno);
        ret = EXIT_FAILURE;
        goto exit;
    }

exit:
    close(fd);

    char *dump_cmd;
    asprintf(&dump_cmd, "getfattr -d %s", temp);
    fprintf(stderr, "'%s' exited: %d\n", dump_cmd, system(dump_cmd));

    unlink(temp);
    return ret;
}
