#include "ctlstack.h"
#include "object.h"
#include "number.h"
#include "mstring.h"
#include "symbol.h"
#include "cons.h"
#include "binding.h"
#include "function.h"
#include "utils.h"

static char *object_names[] = {
   "number_type",
   "string_type",
   "symbol_type",
   "cons_type",
   "fbuiltin_type",
   "sfbuiltin_type",
   "fuser_type",
   "binding_type",
   "fwdptr_type"
};

size_t
object_size(ref o) {
   size_t sz = 0;

   switch(get_type(o)) {
      case fwdptr_type:
         sz = fwdptr_size(o);
         break;
      case number_type:
         sz = number_size(o);
         break;
      case string_type: 
         sz = mstring_size(o);
         break;
      case fbuiltin_type: // fall through
      case sfbuiltin_type: // fall through
      case fuser_type:
         sz = function_size(o);
         break;
      case symbol_type:
         sz = symbol_size(o);
         break;
      case cons_type:
         sz = cons_size(o);
         break;
      case binding_type:
         sz = binding_size(o);
         break;
      default:
         heap_dump_raw();
         ctlstack_dump_raw();
         error("object_size(): unhandled type %d @ %p", get_type(o), o);
   }

   return get_blk_size(sz);
}

char *
get_type_str(ref r) {
   if (deref(r, object_hdr)->type <= fwdptr_type-1) {
      return object_names[deref(r, object_hdr)->type-1];
   }
   return "unknown object";
}

