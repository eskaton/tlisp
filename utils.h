#ifndef _UTILS_H_
#define _UTILS_H_

#define MAX_PID_LEN 5

#define assert_type(r, type)                               \
   (((r) != NULL && get_type(r) == (type)) ? (void)0 :     \
     error("Assertion failed: (("#r") != NULL && "         \
        "get_type("#r") == ("#type")), "                   \
        "function %s, file %s, line %u, object 0x%08lx\n", \
        __func__, __FILE__, __LINE__, (r)))

void error(const char *fmt, ...);

#endif //_UTILS_H_
