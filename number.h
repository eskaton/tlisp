#ifndef _NUMBER_H_
#define _NUMBER_H_

#include "object.h"

#define assert_number(r) (assert_obj(r, number_type))
#define deref_number(r) (deref(r, number))
#define number_size(r) (sizeof(number))

#define NUM_TYPE long double

typedef struct {
   object_hdr hdr;
   NUM_TYPE num;
} number;

ref number_create(NUM_TYPE num);
void number_destroy(ref r);
NUM_TYPE number_value(ref r);

#endif // _NUMBER_H_
