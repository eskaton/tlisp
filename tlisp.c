#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "alloc.h"
#include "ctlstack.h"
#include "debug.h"
#include "htab.h"
#include "reader.h"
#include "eval.h"
#include "env.h"
#include "binding.h"
#include "builtin.h"
#include "alloc.h"
#include "symbol.h"
#include "mstring.h"
#include "function.h"
#include "cons.h"
#include "print.h"
#include "arch.h"

bool dump_opt = true;
bool gc_log_opt = false;

extern unsigned long *stack_base;

ref nil;
ref nilsym;
ref T;

void
install_symbols() {
   ref key = mstring_createi("T"); 
   T = symbol_get(key);
   env_bind_global(T, T);
   mstring_destroy(key);

   nil = cons_create();
   deref_cons(nil)->car = nil;
   deref_cons(nil)->cdr = nil;
   key = mstring_createi("NIL");
   nilsym = symbol_get(key);
   env_bind_global(nilsym, nil);
   mstring_destroy(key);
}

void
install_fbuiltin(char *name, ref (*fptr)(ref )) {
   ref key = mstring_createi(name); 
   ref sym = symbol_get(key);
   ref fun = fbuiltin_create(fptr);
   env_bind_global(sym, fun);
   mstring_destroy(key);
}

void
install_sfbuiltin(char *name, ref (*fptr)(ref )) {
   ref key = mstring_createi(name); 
   ref sym = symbol_get(key);
   ref fun = sfbuiltin_create(fptr);
   env_bind_global(sym, fun);
   mstring_destroy(key);
}

void
install_builtins() {
   install_fbuiltin("+", &fun_add);
   install_fbuiltin("-", &fun_sub);
   install_fbuiltin("*", &fun_mult);
   install_fbuiltin("/", &fun_div);
   install_fbuiltin("MOD", &fun_mod);
   install_fbuiltin("SQRT", &fun_sqrt);
   install_fbuiltin("LVAL", &fun_lval);
   install_fbuiltin("GVAL", &fun_gval);
   install_fbuiltin("CAR", &fun_car);
   install_fbuiltin("CDR", &fun_cdr);
   install_fbuiltin("UNDEF", &fun_undef);
   install_fbuiltin("SETG", &fun_setg);
   install_fbuiltin("SET", &fun_set);
   install_fbuiltin("EVAL", &fun_eval);
   install_fbuiltin("QUIT", &fun_quit);
   install_fbuiltin("<", &fun_lt);
   install_fbuiltin("<=", &fun_le);
   install_fbuiltin("=", &fun_eq);
   install_fbuiltin("/=", &fun_ne);
   install_fbuiltin(">=", &fun_ge);
   install_fbuiltin(">", &fun_gt);
   install_fbuiltin("NOT", &fun_not);
   install_fbuiltin("PRINT", &fun_print);
   install_fbuiltin("CONS", &fun_cons);
   install_fbuiltin("RPLACA", &fun_rplaca);
   install_fbuiltin("RPLACD", &fun_rplacd);
   install_fbuiltin("1+", &fun_inc);
   install_fbuiltin("1-", &fun_dec);
   install_fbuiltin("PRINT-HEAP-STATS", &fun_print_heap_stats);
   install_fbuiltin("GET-GC-STATS", &fun_get_gc_stats);

   install_sfbuiltin("QUOTE", &fun_quote);
   install_sfbuiltin("DEFUN", &fun_defun);
   install_sfbuiltin("IF", &fun_if);
   install_sfbuiltin("LOOP", &fun_loop);
   install_sfbuiltin("PROGN", &fun_progn);
   install_sfbuiltin("AND", &fun_and);
   install_sfbuiltin("OR", &fun_or);
   install_sfbuiltin("LET", &fun_let);
}

int
main(int argc, char **argv) {
   get_stack_base(stack_base); 

   if( argc != 2 ) {
       printf("usage: %s <file>\n", argv[0]);
       exit(1);
   }

   heap_create();
   ctlstack_create();
   env_init();
   symbol_init();
   install_symbols();
   install_builtins();

   if (argv[1][0] == '-') {
      istream *is = istream_create(0);
      while (!istream_eof(is)) {
         write(0, "> ", 2);
         ref pgm = main_reader(is);
         if (pgm) {
            print(eval(pgm));
         }
      }
   
   } else {
      istream *is = istream_createf(argv[1]);

      if (!is) {
         perror("istream_createf"); 
         exit(1);
      }

      while (!istream_eof(is)) {
         ref pgm = main_reader(is);
         if (pgm) {
            eval(pgm);
         }
      }
   }

   return 0;
}
