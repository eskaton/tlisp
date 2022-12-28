#include <stdlib.h>

#include "number.h"

inline ref 
number_alloc() {
   ref r = talloc(sizeof(number));
   set_type(r, number_type);
   return r;
}

ref 
number_create(NUM_TYPE num) {
   ref r = number_alloc();
   number *n = deref_number(r);
   n->num = num;
   return r;
}

void
number_destroy(ref r) {
   assert_number(r);
   tfree(r);
}

inline NUM_TYPE
number_value(ref r) {
   assert_number(r);
   return deref(r, number)->num;
}
