#include <stdlib.h>
#include <ctype.h>

#include "mstring.h"
#include "utils.h"

inline ref
mstring_alloc(size_t sz) {
   ref r = talloc(sz);
   set_type(r, string_type);
   return r;
}

ref
mstring_create(void) {
   ref r = mstring_alloc(sizeof(mstring));
   mstring *s = deref_string(r);
   s->str = '\0';
   s->size = CHARS_IN_STRUCT;
   s->len = 0;
   return r;
}

ref
mstring_createi(const char *str) {
   size_t len = strlen(str);
   ref r = mstring_alloc(sizeof(mstring) + MAX(len + 1 - CHARS_IN_STRUCT, 0));
   mstring *s = deref_string(r);
   strcpy(&s->str, str);
   s->len = len;
   s->size = len + 1;
   return r;
}

ref
mstring_copy(ref mstr) {
   assert_obj(mstr, string_type);
   size_t sz = sizeof(mstring) + MAX(deref_string(mstr)->len + 1 - CHARS_IN_STRUCT, 0);
   ref r = mstring_alloc(sz);
   mstring *s = deref_string(r);
   memcpy(s, deref_string(mstr), sz);
   return r;
}

inline char *
mstring_value(ref mstr) {
   assert_obj(mstr, string_type);
   return &deref_string(mstr)->str;
}

inline size_t
mstring_len(ref mstr) {
   assert_obj(mstr, string_type);
   return deref_string(mstr)->len;
}

inline ref
mstring_grow(ref mstr, size_t sz) {
   ref new_mstr = trealloc(mstr, sz);

   if (new_mstr) {
      return new_mstr; 
   }

   error("out of memory");
   return NULL; // unreachable
}

void
mstring_append(ref *mstr, const char *str) {
   assert_obj(*mstr, string_type);
   size_t len = strlen(str);
   mstring *s = deref_string(*mstr);

      if (len > s->size - s->len - 1) {
         *mstr = trealloc(*mstr, sizeof(mstring) + 
         	 MAX(s->len + 1 - CHARS_IN_STRUCT, 0) + len);
         s = deref_string(*mstr);
         strcpy(&s->str + s->len, str); 
         s->len += len;
         s->size += len;
      } else {
         strcpy(&s->str + s->len, str); 
         s->len += len;
      }
}

void
mstring_appendc(ref *mstr, char c) {
   assert_obj(*mstr, string_type);
   mstring *s = deref_string(*mstr);

   if (s->size - s->len - 1 == 0) {
      *mstr = trealloc(*mstr, sizeof(mstring) - CHARS_IN_STRUCT + s->size * 2);
      s = deref_string(*mstr);
      (&s->str)[s->len] = c;
      (&s->str)[s->len+1] = '\0';
      s->len++;
      s->size = s->size * 2;
   } else {
      (&s->str)[s->len++] = c;
      (&s->str)[s->len] = '\0';
   }
}

ref
mstring_toupper(ref mstr) {
   assert_obj(mstr, string_type);
   mstring *s = deref_string(mstr);
   char *cptr = &s->str;

   while (*cptr) {
      *cptr = toupper((int)*cptr); 
      cptr++;
   }

   return mstr;
}

ref
mstring_tolower(ref mstr) {
   assert_obj(mstr, string_type);
   mstring *s = deref_string(mstr);
   char *cptr = &s->str;

   while (*cptr) {
      *cptr = tolower((int)*cptr); 
      cptr++;
   }

   return mstr;
}

void
mstring_destroy(ref mstr) {
   assert_obj(mstr, string_type);
   tfree(mstr);
}

