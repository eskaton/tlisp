#ifndef _TYPES_H_
#define _TYPES_H_

#include "object.h"
#include "mstring.h"

typedef struct object {
   object_type type;
   union {
      mstring *str;
      long double num;
      struct cons *cons;
      struct object *(*fptr)(struct object *);
   } value;
} object;

void print_objects();
object *object_create(object_type type);
void object_init();

#endif //_TYPES_H_
