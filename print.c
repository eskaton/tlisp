#include <stdio.h>
#include <stdbool.h>

#include "mstring.h"
#include "symbol.h"
#include "number.h"
#include "cons.h"
#include "function.h"
#include "print.h"

extern ref nil;

void
object_print(ref obj) {
   if (obj == NULL) {
      printf("null\n");
   }

   switch (get_type(obj)) {
      case fwdptr_type:
         printf("-> %p", deref_fwdptr(obj)->ptr);
         break;
      case string_type:
        printf("\"%s\"", mstring_value(obj)); 
        break;
      case symbol_type:
        printf("%s", mstring_value(symbol_name(obj))); 
        break;
      case number_type:
        printf("%Lg", number_value(obj)); 
        break;
      case cons_type:
        cons_print(obj); 
        break;
      case fbuiltin_type:
        printf("<builtin-function @ 0x%lx>", (long)deref_fbuiltin(obj)->f.ptr); 
        break;
      case sfbuiltin_type:
        printf("<builtin-special-function @ 0x%lx>", (long)deref_sfbuiltin(obj)->f.ptr); 
        break;
      case fuser_type:
        printf("<user-defined-function @ 0x%lx>", (long)deref_fuser(obj)->f.def); 
        break;
      default:
        printf("<unknown 0x%lx>", (long)&obj); 
        break;
   }
}

void 
cons_print(ref c) {
   if (c == nil) {
      printf("NIL");
      return;
   }

   printf("(");

   while (true) {
      object_print(cons_car(c));

      if (cons_cdr(c) == nil) {
         break; 
      }

      if (get_type(cons_cdr(c)) == cons_type) {
         c = cons_cdr(c);
      } else {
         printf(" . "); 
         object_print(cons_cdr(c));
         break;
      }

      printf(" "); 
   }

   printf(")");
}


void
print(ref obj) {
   object_print(obj);
   printf("\n");
}
