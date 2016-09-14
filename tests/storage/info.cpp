#include <KernelKit.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    setbuf(stdout, NULL); // do not buffer

    int fd;
    int pos;
    int ret = EXIT_SUCCESS;

    char temp[] = "/tmp/attrXXXXXX";
    fd = mkostemp(temp, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Error creating temp file: %d %s\n", errno, strerror(errno));
        return EXIT_FAILURE;
    }
    close(fd);

    fprintf(stdout, "Using '%s' for info test file\n", temp);
    long dev = dev_for_path(temp);
    if (dev < 0) {
        fprintf(stderr, "Error getting dev for %s: %zd\n", temp, dev);
        ret = EXIT_FAILURE;
        goto exit;
    }

    fs_info info;
    if (fs_stat_dev(dev, &info) < 0) {
        fprintf(stderr, "Error getting stat for %s (%zu): %d %s\n", temp, dev, errno, strerror(errno));
        ret = EXIT_FAILURE;
        goto exit;
    }

    printf("dev:\t%zu\n", info.dev);
    printf("root:\t%zu\n", info.root);
    printf("flags:\t0x%X\n", info.flags);
    printf("block_size:\t%zu\n", info.block_size);
    printf("io_size:\t%zu\n", info.io_size);
    printf("total_blocks:\t%zu\n", info.total_blocks);
    printf("free_blocks:\t%zu\n", info.free_blocks);
    printf("total_nodes:\t%zu\n", info.total_nodes);
    printf("free_nodes:\t%zu\n", info.free_nodes);
    printf("device_name:\t%s\n", info.device_name);
    printf("volume_name:\t%s\n", info.volume_name);
    printf("fsh_name:\t%s\n", info.fsh_name);

    printf("--- All devs: ");
    pos = 0;
    while((dev = next_dev(&pos)) >=0) {
       printf("%ld ", dev);
    }
    printf("\n");

exit:
    unlink(temp);
    return ret;
}
