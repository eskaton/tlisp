#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include "istream.h"

istream *
istream_create(int fd) {
   istream *s = malloc(sizeof(istream));
   s->file = "<unknown>";
   s->fd = fd;
   s->buf_sz = DFLT_BUF_SZ;
   s->buf = malloc(s->buf_sz);
   s->buf_pos = 0;
   s->buf_len = 0;
   s->pb_buf_pos = -1;
   s->eof = false;

   return s;
}

istream *
istream_createf(const char *file) {
   istream *s = malloc(sizeof(istream));
   s->file = file;
   s->fd = open(file, O_RDONLY);

   if (s->fd==-1) {
      free(s);
      return NULL;
   }

   s->buf_sz = DFLT_BUF_SZ;
   s->buf = malloc(s->buf_sz);
   s->buf_pos = 0;
   s->buf_len = 0;
   s->pb_buf_pos = -1;
   s->eof = false;

   return s;
}

bool
istream_pushback(istream *s, char c) {
   if (s->pb_buf_pos < PB_BUF_SZ-1) {
      s->pb_buf[++s->pb_buf_pos] = c;
      return true;
   }

   return false;
}

inline bool
istream_eof(istream *s) {
   return s->eof;
}

char
istream_read(istream *s) {
   if (s->eof) {
      return -1; 
   }

   if (s->pb_buf_pos >= 0) {
      return s->pb_buf[s->pb_buf_pos--];
   }

   if (s->buf_len > 0 && s->buf_pos < s->buf_len) {
      return s->buf[s->buf_pos++];
   }

   s->buf_len = read(s->fd, s->buf, s->buf_sz);

   if (s->buf_len == -1) {
      return -1;
   } else if (s->buf_len == 0) {
      s->eof = true; 
      return -1;
   } else {
      s->buf_pos = 0;
      return s->buf[s->buf_pos++];  
   }
}

