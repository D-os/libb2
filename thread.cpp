#include <KernelKit.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>

int32 receiver(void *data)
{
   thread_id sender;
   int32 code;
   char buf[512];

   printf("[thread] in thread %d:%d\n", getpid(), syscall(SYS_gettid));
   code = receive_data(&sender, (void *)buf, sizeof(buf));
   if (code == B_INTERRUPTED) {
       printf("[thread] receive_data interrupted\n");
   }
   else {
       printf("[thread] got message %d: %s\n", code, buf);
   }

   snooze(100000);
   printf("[thread] snoozed 100000 - suspending\n");
   suspend_thread(find_thread(NULL));
   printf("[thread] returning %d\n", code);
   return code;
}

int main(int argc, char **argv) {
    thread_id other_thread;
    int32 code = 63;
    const char *buf = "Hello";

    printf("[main] %d:%d ready to spawn thread\n", getpid(), syscall(SYS_gettid));
    other_thread = spawn_thread(receiver, "receiver", 0, NULL);
    printf("[main] spawned thread %d\n", other_thread);
    send_data(other_thread, code, (void *)buf, strlen(buf));
    printf("[main] sent data - resuming thread %d\n", other_thread);
    resume_thread(other_thread);

    snooze(1000000);
    printf("[main] snoozed 1000000 - waiting for thread\n");
    status_t ret;
    wait_for_thread(other_thread, &ret);
    printf("[main] got thread exit code %d\n", ret);

    return ret;
}
