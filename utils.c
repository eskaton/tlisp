#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"
#include "alloc.h"

extern bool dump_opt;

void
error(const char *fmt, ...) {
   char buf[1024];
   va_list args;

   va_start(args, fmt);
   vsnprintf(buf, 1024, fmt, args);
   va_end(args);

   fprintf(stderr, "%s\n", buf);

   if (dump_opt) {
      FILE *dump_file;
      char *file_pfix = "./tlisp.heapdump.";
      char *file_name = malloc(strlen(file_pfix) + (MAX_PID_LEN+1) * sizeof(char));

      sprintf(file_name, "%s%u", file_pfix, getpid());

      if (!(dump_file = fopen(file_name, "a+"))) {
         perror("Couldn't open file for heap dump");
      };

      free(file_name);

      heap_dump_raw_to(dump_file);
   }

   exit(1);
}

