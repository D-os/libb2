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

int32 guard(void *data)
{
    thread_info t;
    get_thread_info(find_thread(NULL), &t);
    printf("[%s] in thread %d:%d - guard %d\n", t.name, getpid(), find_thread(NULL), *((int*)data));
    *((int*)data) = 0xff;
    return EXIT_SUCCESS;
}

int32 worker(void *data)
{
    thread_info t;
    get_thread_info(find_thread(NULL), &t);
    printf("[%s] in thread %d:%d\n", t.name, getpid(), find_thread(NULL));
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
    setbuf(stdout, NULL); // do not buffer

    char buf[255];

    number = 0;

    thread_id guards[20];
    int values[20];

    printf("[main] %d:%d spawning 10/2 guards\n", getpid(), find_thread(NULL));
    for (int i = 0; i < 10; i++) {
        sprintf(buf, "guard%02d", i);
        values[i] = i;
        guards[i] = spawn_thread(guard, buf, 0, &values[i]);
        if (guards[i] < 0) {
            printf("[main] error spawning guard %d: %d\n", i, guards[i]);
            return EXIT_FAILURE;
        }
        if (i % 2) resume_thread(guards[i]); // every other guard is running
    }

    printf("[main] %d:%d creating semaphore\n", getpid(), find_thread(NULL));
    if ((sem = create_sem(number, "number")) < B_NO_ERROR) {
        printf("[main] failed creating semaphore: %ld\n", (ulong)sem);
        return EXIT_FAILURE;
    }

    printf("[main] %d:%d spawning 99 workers\n", getpid(), find_thread(NULL));
    for (int i = 1; i <= 99; i++) {
        sprintf(buf, "worker%02d", i);
        thread_id thread = spawn_thread(worker, buf, 0, NULL);
        if (thread < 0) {
            printf("[main] error spawning thread %d: %d\n", i, thread);
            return EXIT_FAILURE;
        }
        resume_thread(thread);
    }

    printf("[main] %d:%d spawning 10/2 guards\n", getpid(), find_thread(NULL));
    for (int i = 10; i < 20; i++) {
        sprintf(buf, "guard%02d", i);
        values[i] = i;
        guards[i] = spawn_thread(guard, buf, 0, &values[i]);
        if (guards[i] < 0) {
            printf("[main] error spawning guard %d: %d\n", i, guards[i]);
            return EXIT_FAILURE;
        }
        if (i % 2) resume_thread(guards[i]); // every other guard is running
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

    printf("[main] %d:%d resuming 2*5 guards\n", getpid(), find_thread(NULL));
    for (int i = 0; i < 20; i++) {
        if (!(i % 2)) resume_thread(guards[i]); // every other guard
    }

    snooze(10000);
    printf("[main] done\n");
    for (int i = 0; i < 20; i++) {
        if (values[i] != 0xff) {
            printf("incorrect value for guard %d: %d\n", i, values[i]);
        }
    }

    snooze(10000);
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
