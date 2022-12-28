#ifndef _CONS_H_
#define _CONS_H_

#include <stdbool.h>

#include "alloc.h"
#include "object.h"

#define assert_cons(r) (assert_obj(r, cons_type))
#define deref_cons(r) (deref(r, cons))
#define cons_size(r) (sizeof(cons))

typedef struct {
   object_hdr hdr;
   ref car;
   ref cdr;
} cons;

ref cons_create();
ref cons_createi(ref car);
ref cons_append(ref c, ref obj);
ref cons_car(ref c);
ref cons_cdr(ref c);
ref cons_rplaca(ref c, ref obj);
ref cons_rplacd(ref c, ref obj);
ref cons_next(ref c);
long cons_len(ref c);
bool cons_empty(ref c);

#endif //_CONS_H_
