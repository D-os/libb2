#include <KernelKit.h>
#include <string.h>

int32 receiver(void *data)
{
   thread_id sender;
   int32 code;
   char buf[512];

   code = receive_data(&sender, (void *)buf, sizeof(buf));

   snooze(1000000);
   suspend_thread(find_thread(NULL));
   return code;
}

int main(int argc, char **argv) {
    thread_id other_thread;
    int32 code = 63;
    const char *buf = "Hello";

    other_thread = spawn_thread(receiver, "receiver", 0, NULL);
    send_data(other_thread, code, (void *)buf, strlen(buf));
    resume_thread(other_thread);

    snooze(100000);
    status_t ret;
    wait_for_thread(other_thread, &ret);

    return ret;
}
