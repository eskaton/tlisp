#include <stdio.h>
#include <stdlib.h>

#include "binding.h"
#include "mstring.h"
#include "symbol.h"
#include "print.h"

bframe *
bframe_create(ref value, int frame) {
   bframe *bf = (bframe *)malloc(sizeof(bframe));
   bf->frame = frame;
   bf->value = value;
   return bf;
}

void
bframe_destroy(bframe *bf) {
   free(bf);
}

void
bframe_destroy_stack_entry(void *value) {
   bframe_destroy((bframe *)value);
}

binding *
binding_create(ref sym, ref value, int frame) {
   binding *bind = (binding *)malloc(sizeof(binding));
   bind->sym = sym;
   bind->values = stack_create();
   stack_push(bind->values, bframe_create(value, frame));
   return bind;
}

void
binding_destroy(binding *bind) {
   /* TODO: free stack content */
   stack_destroy_free(bind->values, &bframe_destroy_stack_entry);
   free(bind);
}

void 
binding_set(binding *bind, ref value, int frame) {
   bframe *bf = stack_peek(bind->values);

   if (frame == bf->frame) {
      bf->value = value; 
   }

   stack_push(bind->values, bframe_create(value, frame));
}

ref 
binding_get(binding *bind) {
   bframe *bf = stack_peek(bind->values);
   return bf->value;
}

void
binding_delete(binding *bind, int frame) {
   bframe *bf = stack_peek(bind->values);

   if (frame == bf->frame) {
      stack_pop(bind->values);
      bframe_destroy(bf);
   }

}

bool
binding_has_value(binding *bind) {
   bframe *bf = stack_peek(bind->values);
   return bf != NULL;
}

void
print_binding(binding *bind) {
   if (bind) {
      bframe *bf = stack_peek(bind->values);
      printf("%s(%u) = ", mstring_value(symbol_name(bind->sym)), bf->frame);
      print(bf->value);
   } else {
      printf("null\n"); 
   }
}

ref
binding_create_new(ref sym, ref value) {
   ref r = talloc(sizeof(Binding));
   set_type(r, binding_type);
   deref_binding(r)->sym = sym;
   deref_binding(r)->value = value;
   return r;
}

