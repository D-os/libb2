#include <OS.h>

#include <stdio.h>
#include <stdarg.h>

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
