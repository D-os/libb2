/*
 * Copyright 2001-2011, Haiku.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		Ingo Weinhold (bonefish@users.sf.net)
 */

//!	Global library initialization/termination routines.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <execinfo.h>

#include <AppMisc.h>
#include <LooperList.h>
#include <MessagePrivate.h>
//#include <RosterPrivate.h>
#include <TokenSpace.h>

//extern void __initialize_locale_kit();


// debugging
#ifdef DEBUG
#define DBG(x) x
#else
#define DBG(x)
#endif
#define OUT	printf

static void
_crash_sigaction(int signum, siginfo_t *info, void *ucontext)
{
    void *array[64];
    int size;

    fprintf(stderr, "=== SIGNAL %d (%s), ADDR %p\n",
            signum, strsignal(signum), info->si_addr);

    size = backtrace(array, count_of(array));

    /* skip first stack frames (points here, pthread and sigaction) */
    const int before = 4;
    /* skip last frames (_start entries) */
    const int after = 2;
    backtrace_symbols_fd(array + before, size - before - after, STDERR_FILENO);

    /* restore default handler and return to call it */
    signal(signum, SIG_DFL);
}


static void
initialize_forked_child()
{
    DBG(OUT("initialize_forked_child()\n"));

    BMessage::Private::StaticReInitForkedChild();
    BPrivate::gLooperList.InitAfterFork();
    BPrivate::gDefaultTokens.InitAfterFork();
    BPrivate::init_team_after_fork();

    DBG(OUT("initialize_forked_child() done\n"));
}


extern "C" void
initialize_before()
{
    DBG(OUT("initialize_before()\n"));

    BMessage::Private::StaticInit();
//    BRoster::Private::InitBeRoster();

    /* setup crash handler */
    struct sigaction sigact;
    sigact.sa_sigaction = _crash_sigaction;
    sigact.sa_flags = SA_RESTART | SA_SIGINFO;
    int sigs[] = {SIGILL, SIGABRT, SIGBUS, SIGFPE, SIGSEGV};
    for (unsigned i = 0; i < count_of(sigs); i++) {
        if (sigaction(sigs[i], &sigact, (struct sigaction *)NULL) != 0) {
            fprintf(stderr, "error setting signal handler for %d (%s)\n",
                    sigs[i], strsignal(sigs[i]));
            exit(EXIT_FAILURE);
        }
    }

    atfork(initialize_forked_child);

    DBG(OUT("initialize_before() done\n"));
}
__attribute__((section(".init_array"))) void (* p_lib_be_init)(void) = &initialize_before;


extern "C" void
initialize_after()
{
    DBG(OUT("initialize_after()\n"));

//    __initialize_locale_kit();

    DBG(OUT("initialize_after() done\n"));
}


extern "C" void
terminate_after()
{
    DBG(OUT("terminate_after()\n"));

//    BRoster::Private::DeleteBeRoster();
    BMessage::Private::StaticCleanup();
    BMessage::Private::StaticCacheCleanup();

    DBG(OUT("terminate_after() done\n"));
}

