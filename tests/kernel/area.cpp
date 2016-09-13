#include <KernelKit.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
    setbuf(stdout, NULL); // do not buffer

    void *addr = (void *)(intptr_t)(B_PAGE_SIZE * 0x80);
    size_t size = B_PAGE_SIZE * 0xf;
    printf("before create_area addr: %p\n", addr);
    area_id id = create_area("test", &addr, B_BASE_ADDRESS, size, B_NO_LOCK, B_READ_AREA | B_WRITE_AREA);
    printf(" after create_area addr: %p\tarea_id: %zx\n", addr, id);
    char *first = (char*)addr, *last = first + size - 1;
    printf("> first: %d\tlast: %d\n", *first, *last);
    *first = *last = 0xff;
    printf("< first: %d\tlast: %d\n", *first, *last);

    area_info areaInfo;
    status_t status = get_area_info(id, &areaInfo);
    if (status != B_OK) {
        printf("error get_area_info(%zx): %d\n", id, status);
        exit(EXIT_FAILURE);
    }
    printf("area %zx[%.*s] 0x%zx@%p x%d\n", areaInfo.area, B_OS_NAME_LENGTH, areaInfo.name, areaInfo.size, areaInfo.address, areaInfo.copy_count);

    area_id id2 = clone_area("clone", &addr, B_ANY_ADDRESS, B_READ_AREA, id);
    if (id2 < 0) {
        printf("error clone_area(%zx): %zd\n", id, id2);
        exit(EXIT_FAILURE);
    }
    printf("after clone_area addr: %p\tarea_id: %zx\n", addr, id2);

    status = get_area_info(id2, &areaInfo);
    if (status != B_OK) {
        printf("error get_area_info(%zx): %d\n", id2, status);
        exit(EXIT_FAILURE);
    }
    printf("area %zx[%.*s] 0x%zx@%p x%d\n", areaInfo.area, B_OS_NAME_LENGTH, areaInfo.name, areaInfo.size, areaInfo.address, areaInfo.copy_count);
    printf("< first: %d\tlast: %d\n", *(char*)areaInfo.address, *((char*)areaInfo.address + areaInfo.size - 1));

    delete_area(id);
}
