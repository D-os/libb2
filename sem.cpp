#include <OS.h>

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <syscall.h>

#define atomic_inc(P) __sync_add_and_fetch((P), 1)
#define atomic_dec(P) __sync_add_and_fetch((P), -1)

int number;
sem_id sem;

int32 worker(void *data)
{
    thread_info t;
    get_thread_info(find_thread(NULL), &t);
    printf("[%s] in thread %d:%d\n", t.name, getpid(), (pid_t)syscall(SYS_gettid));
    for (int i = 10; i > 0; i--) {
        status_t ret = acquire_sem(sem);
        if (ret != B_NO_ERROR) {
            printf("[%s] error acquire_sem: %d\n", t.name, ret);
            return EXIT_FAILURE;
        }
        atomic_dec(&number);
    }
    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    char buf[255];

    number = 0;

    printf("[main] %d:%d creating semaphore\n", getpid(), (pid_t)syscall(SYS_gettid));
    if ((sem = create_sem(1, "write")) < B_NO_ERROR) {
        printf("[main] failed creating semaphore: %ld\n", (ulong)sem);
        return EXIT_FAILURE;
    }

    printf("[main] %d:%d spawning 999 workers\n", getpid(), (pid_t)syscall(SYS_gettid));
    for (int i = 1; i <= 999; i++) {
        sprintf(buf, "worker%d", i);
        thread_id thread = spawn_thread(worker, buf, 0, NULL);
        resume_thread(thread);
    }

    printf("[main] nudgeing 90x\n");
    for (int i=0; i < 90; i++) {
        int tmp = number;
        snooze(100000);
        number = tmp + 1;
        release_sem(sem);
        tmp = number;
        snooze(100000);
        number = tmp + 10;
        release_sem_etc(sem, 10, 0);
        tmp = number;
        snooze(100000);
        number = tmp + 100;
        release_sem_etc(sem, 100, 0);
    }
    snooze(1000000);
    printf("[main] done - number is %d\n", number);
    // dump all running threads

    return EXIT_SUCCESS;
}
