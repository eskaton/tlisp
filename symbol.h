#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#include "alloc.h"
#include "object.h"

#define assert_symbol(r) (assert_obj(r, symbol_type))
#define deref_symbol(r) (deref(r, symbol))
#define symbol_size(r) (sizeof(symbol))

typedef struct {
   object_hdr hdr;
   ref name;
} symbol;

void symbol_init();
ref symbol_get(ref key);
ref symbol_name(ref sym);

#endif // _SYMBOL_H_
