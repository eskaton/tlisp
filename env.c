#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "symbol.h"
#include "mstring.h"
#include "env.h"
#include "binding.h"
#include "print.h"
#include "list.h"
#include "ctlstack.h"

#define GLOBAL_FRAME -1

extern ctlstack *ctlstack_ptr;

env *envp;

unsigned long
symbol_hash(ref sym) {
   register unsigned long result = 0;
   size_t sz = sizeof(sym);
   register long key = (long)sym;

   while (--sz) {
      result = result*31 + (key&0xFF);
      key >>= 8;
   } 

   return result;
}

bool
symbol_equals(void *key1, void *key2) {
   DEBUG(DEBUG_ENV, "%u == %u?\n", (long)key1, (long)key2);
   if (key1 == key2) {
      return true; 
   }

   return false;
}

void
env_init() {
   envp = (env *)malloc(sizeof(env)); 
   envp->frame = 0;
   envp->globals = htab_create(&symbol_hash, &symbol_equals);
   DEBUG(DEBUG_ENV, "globals htab is at %p\n", envp->globals);
}

void env_push_frame() {
   ctlstack_push_frame(ctlstack_ptr);
}

void 
env_pop_frame() {
   ctlstack_pop_frame(ctlstack_ptr);
}

void
env_bind_global(ref sym, ref value) {
   binding *bind = htab_get(envp->globals, sym);

   if (bind) {
      binding_set(bind, value, GLOBAL_FRAME); 
   } else {
      bind = binding_create(sym, value, GLOBAL_FRAME); 
      htab_put(envp->globals, sym, bind);
   }
}

binding *
env_get_global(ref sym) {
   return htab_get(envp->globals, sym);
}

void
env_unbind_global(ref sym) {
   htab_entry *e = htab_delete(envp->globals, sym); 
   if (e) {
      binding *bind = e->value;
      binding_destroy(bind);
      htab_entry_destroy(e);
   }
}

void
env_bind_local(ref sym, ref value) {
   ctlstack_bind(ctlstack_ptr, sym, value);
}

void
env_rebind_local(ref sym, ref value) {
   Binding *bind = ctlstack_lookup(ctlstack_ptr, sym);

   if (bind) {
      bind->value = value;
   } else {
      error("Variable %s doesn't exist", mstring_value(symbol_name(sym)));
   }
}

Binding *
env_get_local(ref sym) {
   return ctlstack_lookup(ctlstack_ptr, sym);
}

ref 
env_get_function(ref sym) {
   binding *bind = htab_get(envp->globals, sym);

   if (bind) {
      bframe *bf = (bframe *)stack_peek(bind->values);
      return bf->value;
   } else {
      ; // TODO: local functions 
   }

   return NULL;
}

void
print_env() {
   printf("globals:\n");

   htab_it *it = htab_it_create(envp->globals);

   while (htab_it_has_next(it)) {
      htab_entry *ent = htab_it_get_next(it); 
      binding *bind = (binding *)ent->value;
      bframe *bf = (bframe *)stack_peek(bind->values);
      printf("%s: ", mstring_value(symbol_name(bind->sym)));
      print(bf->value);
   }

   htab_it_destroy(it);
}
