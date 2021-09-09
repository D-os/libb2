#include <image.h>
#include <OS.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dlfcn.h>

int main(int argc, char **argv)
{
    setbuf(stdout, NULL); // do not buffer

    char *pid;
    asprintf(&pid, "%d", find_thread(NULL));

    const char *arg_v[] = { "/usr/bin/pstree", "-U", pid, NULL };

    thread_id exec_thread = load_image(count_of(arg_v) - 1, arg_v, (const char **)environ);
    fprintf(stderr, "loaded image '%s' to thread: %d\n", *arg_v, exec_thread);

    int32 return_value;
    status_t st = wait_for_thread(exec_thread, &return_value);
    if (st != B_OK) {
        fprintf(stderr, "wait_for_thread failed: %d\n", st);
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "thread exited: %d\n", return_value);

    image_id addon = load_add_on("/usr/libexec/abrt-gdb-exploitable"); // just some random library
    if (addon == B_ERROR) {
        fprintf(stderr, "load_addon failed: %ld %s\n", addon, dlerror());
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "loaded addon image: %ld\n", addon);

    void *init, *fini;
    get_image_symbol(addon, "_init", B_SYMBOL_TYPE_TEXT, &init);
    get_image_symbol(addon, "_fini", B_SYMBOL_TYPE_TEXT, &fini);
    fprintf(stderr, "addon image symbols: %p %p\n", init, fini);

    return return_value;
}
