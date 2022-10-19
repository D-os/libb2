#include <OS.h>
#include <utils/CallStack.h>

#include <cinttypes>
#include <csignal>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" {
void debug_vprintf(const char *format, va_list args)
{
	vdprintf(2, format, args);
}

void debug_printf(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	debug_vprintf(format, args);
}

_Noreturn void debugger(const char *message)
{
	// FIXME: launch real debugger:
	// FIXME: open terminal window, attach stdin/out/err and drop to gdb there

	dprintf(2, "<>:< %s\n", message);

	android::CallStack::CallStackUPtr stack(new android::CallStack());
	stack->update(1);
	stack->dump(2, 1);

	abort();
}

int _debuggerAssert(const char *file, int line, const char *message)
{
	debug_printf("%" PRId32 ": ASSERT: %s:%d %s\n",
				 find_thread(NULL), file, line, message);
	abort();
}

static struct sigaction old_sa[NSIG];

static void _abort_handler(int sig, siginfo_t *siginfo, void *ctx)
{
	dprintf(2, "SIGNAL %d @ %p: %s \n", sig, siginfo->si_addr, strsignal(sig));

	dprintf(2, "pid: %d, uid: %d, user %d system %d, stack %p, addr %p-%p\n",
			siginfo->si_pid, siginfo->si_uid, siginfo->si_utime, siginfo->si_stime,
			((ucontext_t *)ctx)->uc_stack.ss_sp, siginfo->si_lower, siginfo->si_upper);

	// TODO: print backtrace

	if (old_sa[sig].sa_flags & SA_SIGINFO && old_sa[sig].sa_sigaction) {
		old_sa[sig].sa_sigaction(sig, siginfo, ctx);
	}
	else if (old_sa[sig].sa_handler) {
		old_sa[sig].sa_handler(sig);
	}
}

static void _register_crash_handlers(void)
{
	struct sigaction handler;

	memset(&handler, 0, sizeof(handler));
	handler.sa_sigaction = &_abort_handler;
	handler.sa_flags	 = SA_SIGINFO;
	if (sigfillset(&handler.sa_mask) != 0) {
		perror("sigfillset");
		exit(EXIT_FAILURE);
	}

	sigaction(SIGILL, &handler, &old_sa[SIGILL]);
	sigaction(SIGABRT, &handler, &old_sa[SIGABRT]);
	sigaction(SIGFPE, &handler, &old_sa[SIGFPE]);
	sigaction(SIGSEGV, &handler, &old_sa[SIGSEGV]);
	sigaction(SIGPIPE, &handler, &old_sa[SIGPIPE]);
	sigaction(SIGBUS, &handler, &old_sa[SIGBUS]);
	sigaction(SIGSTKFLT, &handler, &old_sa[SIGSTKFLT]);
}
__attribute__((section(".init_array"))) void (*p_register_crash_handlers)(void) = &_register_crash_handlers;

}  // extern "C"
