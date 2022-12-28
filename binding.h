#ifndef _BINDING_H_
#define _BINDING_H_

#include "alloc.h"
#include "stack.h"
#include "object.h"

#define deref_binding(r) (deref(r, Binding))
#define binding_size(r) (sizeof(Binding))

typedef struct {
   object_hdr hdr;
   ref sym;
   ref value;
} Binding;

typedef struct {
   ref sym;
   stack *values;
} binding;

typedef struct {
   int frame;
   ref value;
} bframe;

ref binding_create_new(ref sym, ref value);

binding *binding_create(ref sym, ref value, int frame);
void binding_destroy(binding *bind);
void binding_set(binding *bind, ref value, int frame); 
ref binding_get(binding *bind);
void binding_delete(binding *bind, int frame);
bool binding_has_value(binding *bind);

#endif //_BINDING_H_

