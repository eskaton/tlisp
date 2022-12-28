#include <stdlib.h>

#include "ctlstack.h"
#include "mstring.h"
#include "symbol.h"
#include "hex_dump.h"

ctlstack *ctlstack_ptr;

void
ctlstack_create() {
   ctlstack_ptr = (ctlstack *)calloc(1, sizeof(ctlstack));
   ctlstack_ptr->start = malloc(CTLSTACK_SIZE);
   ctlstack_ptr->size = CTLSTACK_SIZE;
   ctlstack_ptr->frame = 0;
   ctlstack_ptr->sp = ctlstack_ptr->start;
   ctlstack_ptr->bp = ctlstack_ptr->start;
}

void
ctlstack_push_frame(ctlstack *s) {
   *s->sp = 0;
   s->sp++;
   *s->sp = (long)s->bp;
   s->sp++;
   s->bp = s->sp;
   s->frame++;
}

void
ctlstack_pop_frame(ctlstack *s) {
   s->sp = s->bp;
   s->sp--; 
   s->bp = (ref)*s->sp;
   s->sp--;
   s->frame--;
}

void
ctlstack_bind(ctlstack *s, ref sym, ref value) {
   cs_entry *e = (cs_entry *)s->sp;
   ref b = binding_create_new(sym, value);
   s->sp += sizeof(cs_entry) / sizeof(ref);
   e->sym = sym;
   e->binding = b;
}

Binding *
ctlstack_lookup(ctlstack *s, ref sym) {
   ref pos;

   for (pos = s->sp - 2; pos >= s->start; pos -= 2) {
      if ((ref)*pos == sym) {
         return (Binding *)*(pos+1); 
      }
   }

   return NULL;
}

void
ctlstack_bt() {
   ref pos;
   int frame, old_frame = -1;

   printf("control stack @ %p, sp = %p, bp = %p\n", ctlstack_ptr,
         ctlstack_ptr->sp, ctlstack_ptr->bp);

   for (pos = ctlstack_ptr->sp - 2, frame = ctlstack_ptr->frame; 
         pos >= ctlstack_ptr->start; pos -= 2) {
      if (frame != old_frame) {
         printf("<frame %d>\n", frame);
         old_frame = frame;
      }

      printf("%p: %08lx %08lx ", pos, *pos, *(pos+1));

      if (*pos == 0x0) {
         printf("<stack-ptr for frame %d>\n", frame-1); 
         frame--;
      } else {
         printf("%s\n", mstring_value(symbol_name((ref)*pos)));
      }
   }
   
   printf("\n");
}

void
ctlstack_dump_raw_to(FILE *dest) {
   fprintf(dest, "control stack:\n");
   hex_dump_to(dest, ctlstack_ptr->start, ctlstack_ptr->sp - ctlstack_ptr->start,
         (unsigned long)ctlstack_ptr->start);
}

void
ctlstack_dump_raw() {
   ctlstack_dump_raw_to(stderr);
}

