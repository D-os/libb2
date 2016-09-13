#include <KernelKit.h>

#include <stdio.h>

int32 status(void *data)
{
    thread_id thread = find_thread(NULL);
    thread_info th_info;
    if (get_thread_info(thread, &th_info) != B_OK) {
        printf("failed get_thread_info\n");
        exit(EXIT_FAILURE);
    }

    team_info tm_info;
    if (get_team_info(th_info.team, &tm_info) != B_OK) {
        printf("failed get_team_info\n");
        exit(EXIT_FAILURE);
    }

    printf("current thread:\t%d\n", thread);
    printf("   thread team:\t%d\n", th_info.team);
    printf("\n");
    printf("current team:\t%d\n", tm_info.team);
    printf("team threads:\t%d\n", tm_info.thread_count);
    printf("        argc:\t%d\n", tm_info.argc);
    printf("        args:\t%.*s\n", 64, tm_info.args);
    printf("         uid:\t%d\n", tm_info.uid);
    printf("         gid:\t%d\n", tm_info.gid);

    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    setbuf(stdout, NULL); // do not buffer

    thread_id other_thread = spawn_thread(status, "status", 0, NULL);
    printf("spawned thread %d\n", other_thread);
    printf("---------------------------\n");
    status_t ret;
    wait_for_thread(other_thread, &ret);
    printf("---------------------------\n");
    printf("exiting\n");

    return EXIT_SUCCESS;
}
