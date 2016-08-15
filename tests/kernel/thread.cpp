#include <KernelKit.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int32 receiver(void *data)
{
    thread_id sender;
    int32 code;
    char buf[512];

    printf("[thread] in thread %d:%d\n", getpid(), find_thread(NULL));
    code = receive_data(&sender, (void *)buf, sizeof(buf));
    if (code == B_INTERRUPTED) {
        printf("[thread] receive_data interrupted\n");
    }
    else {
        printf("[thread] got message from %d - %d: %s\n", sender,  code, buf);
    }

    printf("[thread] snoozing 2000000\n");
    snooze(2000000);
    printf("[thread] snoozed 2000000 - returning %d\n", code);
    return code;
}

int main(int argc, char **argv) {
    setbuf(stdout, NULL); // do not buffer

    thread_id other_thread;
    int32 code = 63;
    const char *buf = "Hello";

    printf("[main] %d:%d ready to spawn thread\n", getpid(), find_thread(NULL));
    other_thread = spawn_thread(receiver, "receiver", 0, NULL);
    printf("[main] spawned thread %d\n", other_thread);
    printf("[main] sending data to thread %d: %d\n", other_thread,
           send_data(other_thread, code, (void *)buf, strlen(buf)+1));
    printf("[main] resuming thread %d: %d\n", other_thread,
           resume_thread(other_thread));

    printf("[main] snoozing 1000000\n");
    snooze(1000000);
    printf("[main] snoozed 1000000 - waiting for thread\n");
    status_t ret;
    wait_for_thread(other_thread, &ret);
    printf("[main] got thread exit code %d\n", ret);

    return ret;
}
