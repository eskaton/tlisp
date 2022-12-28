#include <string.h>

#include "symbol.h"
#include "mstring.h"
#include "htab.h"
#include "debug.h"

htab *symbols;

bool
symbol_key_equals(ref key1, ref key2) {
   char *s_key1 = mstring_value(key1);
   char *s_key2 = mstring_value(key2);

   DEBUG(DEBUG_SYMBOL, "%s == %s?\n", s_key1, s_key2);

   if (!strcmp(s_key1, s_key2)) {
      return true; 
   }

   return false;
}

unsigned long
symbol_key_hash(ref key) {
  register unsigned long result = 0;
  register int c;
  register char *s_key = mstring_value(key);

  DEBUG(DEBUG_SYMBOL, "key: %s@0x%lx\n", s_key, (long)key);

  while ((c = *s_key++)) {
     result = result * 31 + c;
  }

  DEBUG(DEBUG_SYMBOL, "hash: %Lu\n", result);

  return result;
}

void
symbol_init() {
   symbols = htab_create(&symbol_key_hash, &symbol_key_equals);
   DEBUG(DEBUG_SYMBOL, "symbols htab is at %p\n", symbols);
}

ref 
symbol_create() {
   ref r = talloc(sizeof(symbol));
   set_type(r, symbol_type);
   return r;
}

ref 
symbol_get(ref key) {
   assert_string(key);
   ref symref = htab_get(symbols, key);

   if (!symref) {
      symref = symbol_create();
      symbol *sym = deref(symref, symbol);
      sym->name = mstring_copy(key);
      htab_put(symbols, sym->name, symref);
      DEBUG(DEBUG_SYMBOL, "new symbol: %s@0x%lx\n", mstring_value(key), (long)symref);
   } else {
      DEBUG(DEBUG_SYMBOL, "existing symbol: %s@0x%lx\n", mstring_value(key), (long)symref);
   }

   return symref;
}

ref 
symbol_name(ref sym) {
   assert_symbol(sym);
   return deref_symbol(sym)->name;
}
