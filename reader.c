#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>

#include "utils.h"
#include "debug.h"
#include "istream.h"
#include "htab.h"
#include "object.h"
#include "mstring.h"
#include "symbol.h"
#include "number.h"
#include "cons.h"

ref string_reader(istream *is, char c);
ref list_reader(istream *is, char c);
ref unmatched_reader(istream *is, char c);
ref comment_reader(istream *is, char c);
ref dot_reader(istream *is, char c);
ref colon_reader(istream *is, char c);
ref quote_reader(istream *is, char c);
ref main_reader(istream *is);

typedef ref  (*read_fptr)(istream *, char);

extern ref nil;

char macro_chars[] = {'(', ')', '"', '\'', ';', ',', ':', '\0'};

read_fptr macros[] = {
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &string_reader, 
   NULL, NULL, NULL, NULL, &quote_reader, &list_reader, &unmatched_reader, NULL, 
   NULL, &dot_reader, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
   NULL, NULL, NULL, &colon_reader, &comment_reader, NULL, NULL, NULL, NULL, NULL, 
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
   NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

bool
ismacro(char c) {
   int i;

   for (i=0; macro_chars[i]!='\0'; i++) {
      if (c == macro_chars[i]) {
         return true;
      } 
   }

   return false;
}

ref 
read_string(istream *is) {
   ref str = mstring_create();
   char c;

   while (true) {
      c = istream_read(is); 
      DEBUG(DEBUG_READER, "read char 0x%x='%c'\n", c, c);
      if (istream_eof(is) || isspace((int)c) || ismacro(c)) {
         istream_pushback(is, c);
         break; 
      }

      if (c == -1) {
         error("Read error: errno=", errno); 
      }

      mstring_appendc(&str, c);
   }

   DEBUG(DEBUG_READER, "read string %s\n", mstring_value(str));

   return str;
}

ref 
read_list(istream *is, char delim) {
   ref list = NULL;
   ref list_pos = NULL;
   ref tmp = NULL;
   int c;

   while (true) {
      c = istream_read(is);
      DEBUG(DEBUG_READER, "read char 0x%x='%c'\n", c, c);

      if (istream_eof(is)) {
         error("Premature end of input"); 
      }

      if (c == -1) {
         error("Read error: errno=", errno); 
      }

      if (isspace(c)) {
         continue; 
      }

      if (c == delim) {
         break; 
      }

      istream_pushback(is, c);
      tmp = main_reader(is);
      list_pos = cons_append(list_pos, tmp);

      if (list == NULL) {
         list = list_pos; 
      }
   }

   if (list == NULL) {
      return nil;
   }

   return list;
}

ref 
string_reader(istream *is, char chr) {
   ref str = mstring_create();
   char c;

   while ((c = istream_read(is)) != '"') { 
      DEBUG(DEBUG_READER, "read char 0x%x='%c'\n", c, c);

      if (istream_eof(is)) {
         error("Premature end of input"); 
      }

      if (c == '\\') {
         c = istream_read(is); 
         if (istream_eof(is)) {
            error("Premature end of input"); 
         }
      }

      mstring_appendc(&str, c);
   }

   DEBUG(DEBUG_READER, "read string %s\n", mstring_value(str));

   return str;
}

ref 
number_reader(istream *is, char chr) {
   ref obj;
   char *endp = NULL;
   ref nstr = read_string(is);
   long double num = strtod(mstring_value(nstr), &endp);

   if (endp == NULL || endp-mstring_value(nstr) != mstring_len(nstr)) {
      return symbol_get(nstr);
   }

   obj = number_create(num);

   mstring_destroy(nstr);

   DEBUG(DEBUG_READER, "read number %g\n", number_value(obj));

   return obj;
}

ref 
symbol_reader(istream *is, char chr) {
   ref sstr = mstring_toupper(read_string(is));
   ref sym = symbol_get(sstr);
   mstring_destroy(sstr);
   DEBUG(DEBUG_READER, "read symbol %s\n", mstring_value(symbol_name(sym)));
   return sym;
}

ref 
list_reader(istream *is, char chr) {
   switch (chr) {
      case '(':
         return read_list(is, ')');
         break;
      default:
         error("Unsupported list character: %c", chr);
         break; 
   }

   return nil;
}

ref 
unmatched_reader(istream *is, char chr) {
   error("Delimiter '%c' is unmatched", chr);
   return nil;
}

ref 
comment_reader(istream *is, char chr) {
   char c;

   while ((c = istream_read(is)) != '\n') { 
      if (istream_eof(is)) {
         break;
      }
   }

   return nil;
}

ref 
augmenting_reader(istream *is, char chr, char *sym_name) {
   DEBUG(DEBUG_READER, "symbol_name = %s\n", sym_name);
   ref msym_name = mstring_createi(sym_name);
   ref sym = symbol_get(msym_name);
   DEBUG(DEBUG_READER, "symbol = 0x%lx\n", (long)sym);
   ref list = cons_append(NULL, sym);
   mstring_destroy(msym_name);
   cons_append(list, main_reader(is));
   return list;
}

ref 
dot_reader(istream *is, char chr) {
   return augmenting_reader(is, chr, "LVAL");
}

ref 
colon_reader(istream *is, char chr) {
   return augmenting_reader(is, chr, "GVAL");
}

ref 
quote_reader(istream *is, char chr) {
   return augmenting_reader(is, chr, "QUOTE");
}

ref 
main_reader(istream *is) {
   int c;

   while ((c=istream_read(is)) != -1) { 

      if (isspace(c)) {
         continue;
      }

      read_fptr rfp = macros[c];

      if (rfp) {
         if (rfp == &comment_reader) {
            (*rfp)(is, c); 
            continue; 
         }
         return (*rfp)(is, c); 
      }

      if (isdigit(c)) {
         istream_pushback(is, c);
         return number_reader(is, c);
      } else if (c == '-' || c == '+') {
         int cn = istream_read(is);

         istream_pushback(is, cn);

         if (isdigit(cn)) {
            istream_pushback(is, c);
            return number_reader(is, c);
         }
      }

      istream_pushback(is, c);
      return symbol_reader(is, c);
   }

   return NULL;
}
