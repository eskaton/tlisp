#include <stdlib.h>

#include "cons.h"
#include "debug.h"

extern ref nil;

ref 
cons_create() {
   ref r = talloc(sizeof(cons));
   set_type(r, cons_type);
   deref_cons(r)->car = NULL;
   deref_cons(r)->cdr = NULL;
   return r;
}

ref 
cons_createi(ref car) {
   ref r = talloc(sizeof(cons));
   set_type(r, cons_type);
   deref_cons(r)->car = car;
   deref_cons(r)->cdr = NULL;
   return r;
}

ref 
cons_append(ref c, ref obj) {
   if (c == NULL) {
      c = cons_create();
      deref_cons(c)->car = obj;
      return c;
   } else {
      deref_cons(c)->cdr = cons_createi(obj);
      return deref_cons(c)->cdr;
   }
}

ref 
cons_car(ref c) {
   assert_cons(c);
   return deref_cons(c)->car;
}

ref 
cons_cdr(ref c) {
   assert_cons(c);

   DEBUG(DEBUG_CONS, "cons=0x%lx, cdr=0x%lx\n", c, deref_cons(c)->cdr);

   if (deref_cons(c)->cdr) {
      return deref_cons(c)->cdr; 
   }

   return nil;
}

ref 
cons_rplaca(ref c, ref obj) {
   assert_cons(c);
   deref_cons(c)->car = obj;
   return c;
}

ref 
cons_rplacd(ref c, ref obj) {
   assert_cons(c);
   deref_cons(c)->cdr = obj;
   return c;
}

ref 
cons_next(ref c) {
   assert_cons(c);

   ref next = cons_cdr(c);

   if (next != nil && get_type(next) == cons_type) {
      return next;
   } 
   
   return nil;
}

long
cons_len(ref c) {
   assert_cons(c);
   long len = 0;
   
   if (c == nil) {
      return len;
   }

   if (cons_car(c)) {
      len++;
   }

   while ((c = cons_next(c)) != nil) {
      len++; 
   }

   return len;
}

bool
cons_empty(ref c) {
   if (get_type(c) != cons_type) {
      printf("invalid cons"); 
   }
   assert_cons(c);
   if (c == nil) {
      return true; 
   }
   return false;
}
