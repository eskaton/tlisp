#ifndef _DEBUG_H
#define _DEBUG_H

#define DEBUG_ALLOC  0x01
#define DEBUG_CONS   0x02
#define DEBUG_ENV    0x04
#define DEBUG_EVAL   0x08
#define DEBUG_HTAB   0x10
#define DEBUG_READER 0x20
#define DEBUG_SYMBOL 0x40
#define DEBUG_MAIN   0x80

#define DEBUG_MODE 0

#define DEBUG(mod, fmt, args...) if(DEBUG_MODE & mod) debug(__FILE__, __LINE__, __func__, fmt, args)

void debug(const char *file, int line, const char *func, const char *fmt, ...);

#endif //_DEBUG_H
