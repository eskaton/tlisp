#ifndef _ISTREAM_H_
#define _ISTREAM_H_

#include <stdbool.h>

#define DFLT_BUF_SZ 4096
#define PB_BUF_SZ 2

typedef struct {
   const char *file;
   int fd;
   size_t buf_sz;
   char *buf;
   int buf_pos;
   size_t buf_len;
   char pb_buf[PB_BUF_SZ]; // pushback buffer
   int pb_buf_pos;
   bool eof;
} istream;

istream *istream_create(int fd);
istream *istream_createf(const char *file);
bool istream_pushback(istream *s, char c);
bool istream_eof(istream *s);
char istream_read(istream *s);

#endif //_ISTREAM_H_
