#include <OS.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <utils/CallStack.h>

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

	android::CallStack::CallStackUPtr stack(new android::CallStack());
	stack->update();
	stack->dump(2, 1);
	dprintf(2, "%s\n", message);
	abort();
}

int _debuggerAssert(const char *file, int line, const char *message)
{
	debug_printf("%" PRId32 ": ASSERT: %s:%d %s\n",
				 find_thread(NULL), file, line, message);
	abort();
}
}
