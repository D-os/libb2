#include <OS.h>

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <syscall.h>
#include <assert.h>

#define atomic_inc(P) __sync_add_and_fetch((P), 1)
#define atomic_dec(P) __sync_add_and_fetch((P), -1)

int number;
sem_id sem;

int32 worker(void *data)
{
    thread_info t;
    get_thread_info(find_thread(NULL), &t);
    printf("[%s] in thread %d:%lu\n", t.name, getpid(), find_thread(NULL));
    for (int i = 10; i > 0; i--) {
        status_t ret = acquire_sem(sem);
        if (ret != B_NO_ERROR) {
            printf("[%s] error acquire_sem: %d\n", t.name, ret);
            return EXIT_FAILURE;
        }
        //printf("[%s] yum!\n", t.name);
        atomic_dec(&number);
    }
    printf("[%s] done!\n", t.name);
    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    char buf[255];

    number = 0;

    printf("[main] %d:%lu creating semaphore\n", getpid(), find_thread(NULL));
    if ((sem = create_sem(number, "number")) < B_NO_ERROR) {
        printf("[main] failed creating semaphore: %ld\n", (ulong)sem);
        return EXIT_FAILURE;
    }

    printf("[main] %d:%lu spawning 99 workers\n", getpid(), find_thread(NULL));
    for (int i = 1; i <= 99; i++) {
        sprintf(buf, "worker%02d", i);
        thread_id thread = spawn_thread(worker, buf, 0, NULL);
        if (thread < 0) {
            printf("[main] error spawning thread %d: %lu\n", i, thread);
            return EXIT_FAILURE;
        }
        resume_thread(thread);
    }

    /* 99 workers * each eats 10 = 990
     * 90 nudges * each putting 11 = 990
     */

    snooze(100000);
    printf("[main] nudgeing 90x\n");
    for (int i=0; i < 90; i++) {
        snooze(10000);
        number++;
        status_t ret = release_sem(sem);
        if (ret != B_NO_ERROR) {
            printf("[main] error release_sem: %d\n", ret);
            return EXIT_FAILURE;
        }
        snooze(10000);
        number += 10;
        ret = release_sem_etc(sem, 10, 0);
        if (ret != B_NO_ERROR) {
            printf("[main] error release_sem_etc: %d\n", ret);
            return EXIT_FAILURE;
        }
    }
    snooze(100000);
    printf("[main] done - number is %d\n", number);
    assert(number == 0);

    thread_info info;
    int32 cookie = 0;

    while (get_next_thread_info(0, &cookie, &info) == B_OK) {
        printf("still running thread: %s\n", info.name);
        if (info.state == B_THREAD_WAITING) {
            printf("  waiting on semaphore %lu\n", info.sem);
        }
    }

    return EXIT_SUCCESS;
}
