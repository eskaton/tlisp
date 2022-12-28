#include "function.h"

inline ref 
function_alloc(object_type type) {
   ref r = talloc(sizeof(function));
   set_type(r, type);
   return r;
}

ref 
fbuiltin_create(ref (*fptr)(ref )) {
   ref r = function_alloc(fbuiltin_type);
   deref_fbuiltin(r)->f.ptr = fptr;
   return r;
}

ref 
sfbuiltin_create(ref (*fptr)(ref )) {
   ref r = function_alloc(sfbuiltin_type);
   deref_sfbuiltin(r)->f.ptr = fptr;
   return r;
}

ref 
fuser_create(ref def) {
   ref r = function_alloc(fuser_type);
   deref_fuser(r)->f.def = def;
   return r;
}
