#ifndef _CTLSTACK_H_
#define _CTLSTACK_H_

#include "alloc.h"
#include "binding.h"

#define CTLSTACK_SIZE (2UL<<10)

typedef struct {
   ref start;
   size_t size;
   int frame;
   ref sp;
   ref bp;
} ctlstack;

typedef struct {
   ref sym;
   ref binding;
} cs_entry;

void ctlstack_create();
void ctlstack_push_frame(ctlstack *s);
void ctlstack_pop_frame(ctlstack *s);
void ctlstack_bind(ctlstack *s, ref sym, ref value);
Binding *ctlstack_lookup(ctlstack *s, ref sym);
void ctlstack_bt(void);
void ctlstack_dump_raw_to(FILE *dest);
void ctlstack_dump_raw();

#endif // _CTLSTACK_H_
