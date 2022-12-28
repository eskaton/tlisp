#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <sys/types.h>

#define BYTES_PER_WORD 4
#define BYTES_PER_LINE (4*BYTES_PER_WORD)
char *spc_line = "                                    ";

int 
print_word(FILE *dest, void *buf, size_t len) {
   int i;
   int word_len = 0;

   for (i=0; i<len; i++) {
      fprintf(dest, "%02x", *(uint8_t*)buf++);
      word_len += 2;
   } 

   fprintf(dest, " ");
   
   return word_len+1;
}

void
print_line(FILE *dest, void *buf, size_t len, long addr, char *addr_fmt) {
   int words = len / BYTES_PER_WORD;
   int last_word = len % BYTES_PER_WORD;
   long *pos = buf;
   int i;
   int line_pos = 0;

   fprintf(dest, addr_fmt, addr);
   fprintf(dest, " ");

   for (i=0; i<words; i++) {
      line_pos += print_word(dest, pos, BYTES_PER_WORD);
      pos++;
   }

   if (last_word) {
      line_pos += print_word(dest, pos, last_word);
   }

   fprintf(dest, "%s", &spc_line[line_pos]);
   fprintf(dest, "| ");

   for (i=0; i<len; i++) {
      char c = ((char*)buf)[i];
      if (isprint((int)c)) {
         fprintf(dest, "%c", c); 
      } else {
         fprintf(dest, "."); 
      }
   }

   fprintf(dest, "\n");
}

void 
hex_dump_to(FILE *dest, void *buf, size_t len, long addr) {
   int i;
   int lines = len / BYTES_PER_LINE;
   int last_len = len % BYTES_PER_LINE;
   char *addr_fmt = malloc(20);

   if (addr > 0) {
      sprintf(addr_fmt, "%%p");
   } else {
      int addr_len = ceil(log(len)/log(BYTES_PER_LINE));
      sprintf(addr_fmt, "%%0%dx", addr_len);
   }

   for (i=0; i<lines; i++) {
      print_line(dest, buf, BYTES_PER_LINE, addr, addr_fmt); 
      buf += BYTES_PER_LINE;
      addr += BYTES_PER_LINE;
   }

   if (last_len > 0) {
      print_line(dest, buf, last_len, addr, addr_fmt); 
   }

   free(addr_fmt);
}

void 
hex_dump(void *buf, size_t len, long addr) {
   hex_dump_to(stderr, buf, len, addr);
}

