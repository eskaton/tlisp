#include <stdio.h>
#include <stdarg.h>

void
debug(const char *file, int line, const char *func, const char *fmt, ...) {
   char buf[1024];
   va_list args;

   va_start(args, fmt);
   vsnprintf(buf, 1024, fmt, args);
   va_end(args);

   fprintf(stderr, "DEBUG: %s:%d: %s(): %s", file, line, func, buf);
}

