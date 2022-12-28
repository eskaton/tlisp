#ifndef _COLLECT_H_
#define _COLLECT_H_

#include <sys/time.h>

#include "alloc.h"
#include "list.h"

typedef struct {
   struct timeval elapsed;
   uint32_t runs;
   uint32_t bytes;
} GcStats;

void init_gc(heap *h);
void start_gc();
GcStats *gc_get_stats();

#endif // _COLLECT_H_
