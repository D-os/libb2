#include <OS.h>

#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "private.h"

status_t kill_team(team_id team)
{
    if (kill(team, SIGKILL) != 0) {
        switch (errno) {
        case ESRCH:
            return B_BAD_TEAM_ID;
        case EPERM:
            return B_PERMISSION_DENIED;
        default:
            return B_ERROR;
        }
    }
    return B_OK;
}

status_t _get_team_info(team_id id, team_info *info, size_t size)
{
    memset(info, 0, size);

    /* It's dangerous to go alone! Take this.
     *                proc(5)                */

    char statfile[B_FILE_NAME_LENGTH];
    snprintf(statfile, B_FILE_NAME_LENGTH, "/proc/%d/stat", id);
    FILE *f = fopen(statfile, "r");
    if (f == NULL) return B_BAD_TEAM_ID;

    long num_threads;
    int items = fscanf(f, "%d %*s %*c %*d %*d %*d %*d %*d %*u %*u "
                          "%*u %*u %*u %*u %*u %*d %*d %*d %*d %ld",
                       &info->team, &num_threads);
    fclose(f);
    if (items < 2) return B_BAD_VALUE;
    info->thread_count = (int32)num_threads;

    struct stat statst = {};
    if (stat(statfile, &statst) != 0) return B_BAD_TEAM_ID;
    info->uid = statst.st_uid;
    info->gid = statst.st_gid;

    snprintf(statfile, B_FILE_NAME_LENGTH, "/proc/%d/cmdline", id);
    int fd = open(statfile, O_RDONLY);
    if (fd < 0) return B_BAD_TEAM_ID;
    ssize_t count;
    char buf[1];
    unsigned i = 0;
    while ((count = read(fd, &buf, 1)) > 0) {
        if (*buf == '\0') info->argc++;
        if (i < 64) info->args[i] = (*buf == '\0') ? ' ' : *buf;
        i++;
    }
    close(fd);
    if (count < 0) return B_BAD_VALUE;

    return B_OK;
}
