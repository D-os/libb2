#include <KernelKit.h>

#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <dlfcn.h>

static __thread int _resumed;

static void _cont_sigaction(int signum, siginfo_t *si, void *context)
{
    _resumed = 1;
}

thread_id load_image(int32 argc, const char **argv, const char **environ)
{
    _resumed = 0;
    struct sigaction sc = {};
    sc.sa_sigaction = _cont_sigaction;
    sc.sa_flags = SA_SIGINFO;
    sigaction(SIGCONT, &sc, NULL);

    pid_t pid = fork();
    if (pid < 0) {
        switch (errno) {
        case EAGAIN:
        case ENOMEM:
            return B_NO_MORE_TEAMS;
        default:
            return B_ERROR;
        }
    }

    if (pid > 0)
        return pid;

    /* the child follows */
    setpgid(0, 0); // work in own process group

    // pause immediately - wait for resume
    sigset_t wait_mask;
    sigfillset(&wait_mask);
    sigdelset(&wait_mask, SIGCONT);
    sigdelset(&wait_mask, SIGTERM);
    while (!_resumed)
        sigsuspend(&wait_mask);

    struct sigaction sa = {};
    sa.sa_handler = SIG_IGN;
    sigaction(SIGHUP, &sa, NULL); // do not die with parent

    execve(*argv, (char * const *)argv, (char * const *)environ);
    exit(-1);
}

image_id load_add_on(const char *path)
{
    void *addr = dlopen(path, RTLD_LAZY|RTLD_GLOBAL);
    return addr == NULL ? B_ERROR : (intptr_t)addr;
}

status_t unload_add_on(image_id image)
{
    return dlclose((void *)image) ? B_ERROR : B_OK;
}

status_t get_image_symbol(image_id image, const char *name, int32 symbolType,
                          void **_symbolLocation)
{
    if (image == -1) image = (image_id)RTLD_DEFAULT;
    dlerror();
    *_symbolLocation = dlsym((void*)image, name);
    return dlerror() ? B_BAD_VALUE : B_OK;
}
