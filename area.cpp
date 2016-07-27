#include <KernelKit.h>
#include <stdio.h>

int main(int argc, char **argv) {
    void *addr = (void *)(intptr_t)(B_PAGE_SIZE * 100);
    size_t size = B_PAGE_SIZE * 100;
    printf("before create_area addr: %p\n", addr);
    area_id id = create_area("test", &addr, B_BASE_ADDRESS, size, B_NO_LOCK, B_READ_AREA | B_WRITE_AREA);
    printf(" after create_area addr: %p\tarea_id: %d\n", addr, id);
    char *first = (char*)addr, *last = first + size - 1;
    printf("> first: %d\tlast: %d\n", *first, *last);
    *first = *last = 0xff;
    printf("< first: %d\tlast: %d\n", *first, *last);
}
