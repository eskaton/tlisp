#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include "object.h"

#define deref_function(r) (deref(r, function))
#define deref_fbuiltin(r) (deref_function(r))
#define deref_sfbuiltin(r) (deref_function(r))
#define deref_fuser(r) (deref_function(r))
#define fuser_get_args(r) (cons_car(deref_fuser(r)->f.def))
#define fuser_get_body(r) (cons_cdr(deref_fuser(r)->f.def))
#define function_size(r) (sizeof(function))

typedef struct {
   object_hdr hdr;
   union {
      ref (*ptr)(ref );
      ref def;
   } f;
} function;

ref fbuiltin_create(ref (*fptr)(ref ));
ref sfbuiltin_create(ref (*fptr)(ref ));
ref fuser_create(ref def);

#endif // _FUNCTION_H_
