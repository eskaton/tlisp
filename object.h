#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <assert.h>
#include "alloc.h"
#include "utils.h"

#define MIN_TYPE 1 // id of first object type

typedef enum {
   number_type = MIN_TYPE,
   string_type,
   symbol_type,
   cons_type,
   fbuiltin_type,
   sfbuiltin_type,
   fuser_type,
   binding_type,
   fwdptr_type // must be the last entry
} object_type;

typedef struct {
   object_type type;
} object_hdr;

typedef struct {
   object_hdr hdr; 
   ref ptr;
} fwdptr;

#define deref(r, type) ((type *)(r))
#define deref_fwdptr(r) (deref(r, fwdptr))
#define set_type(r, t) (((object_hdr *)(r))->type = 0xFEAD0000 | (t))
#define get_type(r) (deref(r, object_hdr)->type & 0xFF)
#define has_obj_sig(obj) (((deref(obj, object_hdr)->type) >> 16) == 0xFEAD)
#define is_obj(obj) ((obj) != NULL && has_obj_sig(obj) && \
      get_type(obj) >= MIN_TYPE && get_type(obj) < fwdptr_type)
#define is_fwdptr(obj) ((obj) != NULL && has_obj_sig(obj) && \
      get_type(obj) == fwdptr_type)
#define assert_obj(obj, t) (assert_type(obj, t))
#define fwdptr_size(r) (sizeof(fwdptr))

size_t object_size(ref o);
char *get_type_str(ref r);

#endif // _OBJECT_H_
