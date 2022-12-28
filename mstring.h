#ifndef _MSTRING_H_
#define _MSTRING_H_

#include <strings.h>
#include <string.h>

#include "object.h"

#define CHARS_IN_STRUCT 4

#define assert_string(r) (assert_obj(r, string_type))
#define deref_string(r) (deref(r, mstring))
#define mstring_size(r) (MAX(deref(r, mstring)->size - CHARS_IN_STRUCT, 0) + \
	sizeof(mstring))

typedef struct {
   object_hdr hdr;
   size_t size;
   size_t len;
   char str;
} mstring;

ref mstring_create(void);
ref mstring_createi(const char *str);
ref mstring_copy(ref mstr);
char *mstring_value(ref mstr);
size_t mstring_len(ref mstr);
void mstring_append(ref *mstr, const char *str);
void mstring_appendc(ref *mstr, char c);
ref mstring_toupper(ref mstr);
ref mstring_tolower(ref mstr);
void mstring_destroy(ref mstr);

#endif //_MSTRING_H_
