#include <KernelKit.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int32 reader(void *data)
{
    snooze(10000);
    port_id port = create_port(123, "test");
    if (port < 0) {
        fprintf(stderr, "reader: Failed to create port: %d\n", port);
        return EXIT_FAILURE;
    }
    fprintf(stdout, "reader: port_id: %d\n", port);

    fprintf(stdout, "reader: port message count: %zd\n", port_count(port));

    snooze(10000);
    fprintf(stdout, "reader: port message count: %zd\n", port_count(port));

    ssize_t size = port_buffer_size_etc(port, B_RELATIVE_TIMEOUT, 0);
    if (size == B_TIMED_OUT)
        size = port_buffer_size(port);
    if (size < 0) {
        fprintf(stderr, "reader: Failed to read port buffer size: %zu\n", size);
        return EXIT_FAILURE;
    }
    fprintf(stdout, "reader: port message size: %zd\n", size);

    int32 code;
    char buffer[1024];
    ssize_t read;
    while(true) {
        read = read_port_etc(port, &code, buffer, sizeof(buffer), 0, 0);
        if (read < 0) {
            fprintf(stderr, "reader: Failed to read port: %zd\n", read);
            return EXIT_FAILURE;
        }
        fprintf(stdout, "reader: 0x%x %zd:\"%.*s\"\n", code, read, (int)read, buffer);
    }
}

status_t send_message(port_id port, int32 code, const char *message)
{
    size_t len = strlen(message);
    fprintf(stdout, "writer: 0x%x %zd:\"%s\"\n", code, len, message);
    status_t status = write_port(port, code, message, len);
    if (status != B_OK) {
        fprintf(stderr, "main: Failed to write 0x%x to port: %d\n", code, status);
        return EXIT_FAILURE;
    }
    return status;
}

int main(int argc, char *argv[]) {
    setbuf(stdout, NULL); // do not buffer

    port_id port = create_port(123, "test");
    if (port < 0) {
        fprintf(stderr, "main: Failed to create port: %d\n", port);
        return EXIT_FAILURE;
    }
    fprintf(stdout, "main: port_id: %d\n", port);

    thread_id reader_thread = spawn_thread(reader, "reader", 0, NULL);
    printf("main: starting reader %d: %d\n", reader_thread,
           resume_thread(reader_thread));

    char buf[32];
    for (int32 i = 0; i < 10; i++) {
        sprintf(buf+i, ".");
        send_message(port, i, buf);
    }
    snooze(1000000);

    send_message(port, 0x4321, "testing...");

    send_message(port, 0x1111, "testing... etc");

    fprintf(stdout, "main: close_port: %d\n", close_port(port));

    snooze(1000000);

    fprintf(stdout, "main: delete_port: %d\n", delete_port(port));
}
