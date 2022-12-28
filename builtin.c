#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "utils.h"
#include "builtin.h"
#include "eval.h"
#include "cons.h"
#include "number.h"
#include "symbol.h"
#include "mstring.h"
#include "function.h"
#include "env.h"
#include "print.h"
#include "collect.h"

extern ref nil;
extern ref T;

ref 
get_next_arg(ref *args) {
   if (cons_empty(*args)) {
      error("Too few arguments");
   }
   
   ref arg = cons_car(*args);
   *args = cons_next(*args);

   return arg;
}

inline void
check_too_many_args(ref args) {
   if (args != nil) {
      error("Too many arguments"); 
   }
}

inline void
ensure_type(ref obj, object_type type) {
   if (get_type(obj) != type) { 
      error("Invalid argument type");
   }
}

#define mpone(name, op) \
ref fun_##name(ref args) { \
   ref num = get_next_arg(&args); \
   ensure_type(num, number_type); \
   return number_create(number_value(num) op 1); }

mpone(inc, +);
mpone(dec, -);

ref 
fun_add(ref args) {
   long double num = 0;

   while (args != nil) {
      ref obj = get_next_arg(&args);

      if (get_type(obj) == number_type) {
         num += number_value(obj);
      } else {
         // TODO: coercion ?
         error("Invalid argument, number expected"); 
      }
   } 

   return number_create(num);
}

ref 
fun_sub(ref args) {
   long double num = 0;
   int arg_count = 0;

   while (args != nil) {
      ref obj = get_next_arg(&args);

      if (get_type(obj) == number_type) {
         if (arg_count) {
            num -= number_value(obj);
         } else {
            num = number_value(obj);
         }
         arg_count++;
      }
   } 

   if (arg_count == 0) {
      error("Too few arguments");
   } else if (arg_count == 1) {
      num = -num; 
   }

   return number_create(num);
}

ref 
fun_mult(ref args) {
   long double num = 1;

   while (args != nil) {
      ref obj = get_next_arg(&args);

      if (get_type(obj) == number_type) {
         num *= number_value(obj);
      }
   } 

   return number_create(num);
}

ref 
fun_div(ref args) {
   long double num = 0;
   int arg_count = 0;

   while (args != nil) {
      ref obj = get_next_arg(&args);

      if (get_type(obj) == number_type) {
         if (arg_count) {
            num /= number_value(obj);
         } else {
            num = number_value(obj);
         }
         arg_count++;
      }
   } 

   if (arg_count == 0) {
      error("Too few arguments");
   } else if (arg_count == 1) {
      num = 1 / num; 
   }

   return number_create(num);
}

ref 
fun_sqrt(ref args) {
   ref num = get_next_arg(&args);
   check_too_many_args(args);
   ensure_type(num, number_type);
   return number_create(sqrt(number_value(num)));
}

ref 
fun_mod(ref args) {
   ref dividend = get_next_arg(&args);
   ref divisor = get_next_arg(&args);
   check_too_many_args(args);
   ensure_type(dividend, number_type);
   ensure_type(divisor, number_type);
   return number_create((long long)number_value(dividend) % (long long)number_value(divisor));
}

ref 
fun_lval(ref args) {
   ref var = get_next_arg(&args);
   //TODO: error handling
   Binding *bind = env_get_local(var);

   if (!bind) {
      error("Variable %s unbound", mstring_value(symbol_name(var))); 
   }

   return bind->value;
}

ref 
fun_gval(ref args) {
   ref var = get_next_arg(&args);
   //TODO: error handling
   binding *bind = env_get_global(var);

   if (!bind) {
      error("Variable %s unbound", mstring_value(symbol_name(var))); 
   }

   return binding_get(bind);
}

ref 
fun_car(ref args) {
   ref list = get_next_arg(&args);
   ensure_type(list, cons_type);
   return cons_car(list);
}

ref 
fun_cdr(ref args) {
   ref list = get_next_arg(&args);
   ensure_type(list, cons_type);
   return cons_cdr(list);
}

ref 
fun_undef(ref args) {
   ref name = get_next_arg(&args);
   ensure_type(name, symbol_type);
   env_unbind_global(name);
   return nil;
}

ref 
fun_set(ref args) {
   ref name = get_next_arg(&args);
   ref value = get_next_arg(&args);
   ensure_type(name, symbol_type);
   env_rebind_local(name, value);
   return value;
}

ref 
fun_setg(ref args) {
   ref name = get_next_arg(&args);
   ref value = get_next_arg(&args);
   ensure_type(name, symbol_type);
   env_bind_global(name, value);
   return value;
}

ref 
fun_eval(ref args) {
   return eval(get_next_arg(&args));
}

ref 
fun_quit(ref args) {
   exit(0);
}

#define numeric_cmp(name, op) \
ref  \
fun_##name(ref args) { \
   if (cons_empty(args)) error("Too few arguments"); \
   ref last_num = NULL; \
   while (args != nil) { \
      ref obj = get_next_arg(&args); \
      if (get_type(obj) == number_type) { \
         if (last_num && !(number_value(last_num) op number_value(obj))) \
            return nil; \
         last_num = obj;  \
      } else { \
         error("Invalid argument, number expected");  \
      } \
   } \
   return T; \
}

numeric_cmp(lt, <)
numeric_cmp(le, <=)
numeric_cmp(eq, ==)
numeric_cmp(ne, !=)
numeric_cmp(ge, >=)
numeric_cmp(gt, >)

ref 
fun_not(ref args) {
   if (cons_empty(args)) {
      error("Too few arguments");
   }

   ref arg = get_next_arg(&args);

   if (arg == nil) {
      return T;
   }

   return nil;
}

ref 
fun_print(ref args) {
   print(get_next_arg(&args));
   return T;
}

ref 
fun_cons(ref args) {
   ref cons = cons_createi(get_next_arg(&args));

   if (!cons_empty(args)) {
      cons_rplacd(cons, get_next_arg(&args));
   }

   check_too_many_args(args);
   return cons;
}

ref 
fun_rplaca(ref args) {
   ref cons = get_next_arg(&args);
   ref obj = get_next_arg(&args);
   check_too_many_args(args);
   return cons_rplaca(cons, obj);
}

ref 
fun_rplacd(ref args) {
   ref cons = get_next_arg(&args);
   ref obj = get_next_arg(&args);
   check_too_many_args(args);
   return cons_rplacd(cons, obj);
}

ref 
fun_print_heap_stats(ref args) {
   if (!cons_empty(args)) {
      error("Too many arguments");
   }
   print_heap_stats();
   return nil;
}

ref
fun_get_gc_stats(ref args) {
   if (!cons_empty(args)) {
      error("Too many arguments");
   }

   GcStats *gc_stats = gc_get_stats();

   ref stats = cons_createi(number_create(gc_stats->elapsed.tv_sec));
   cons_append(cons_append(cons_append(stats, 
               number_create(gc_stats->elapsed.tv_usec)), 
            number_create(gc_stats->runs)), 
         number_create(gc_stats->bytes));
   return stats;
}

/****** secial functions ******/

ref 
fun_defun(ref args) {

   if (get_type(args) != cons_type || cons_len(args) != 3) {
      error("Invalid arguments"); 
   }

   ref name = get_next_arg(&args);
   ensure_type(name, symbol_type);
   ref func = fuser_create(args);
   env_bind_global(name, func);

   return func;
}

ref 
fun_quote(ref args) {
   return get_next_arg(&args);
}

ref 
fun_if(ref args) {
   ref cond = get_next_arg(&args);
   ref body = get_next_arg(&args);
   ref res = eval(cond);

   if (res != nil) {
      return eval(body);
   } else if (args != nil) {
      return eval(get_next_arg(&args)); 
   } 

   return nil;
}

ref 
fun_loop(ref args) {
   ref cond = get_next_arg(&args);
   ref body = get_next_arg(&args);
   ref last_val = nil;

   while (true) {
      ref res = eval(cond);

      if (res == nil) {
         return last_val;
      }

      last_val = eval(body);
   } 
}

ref 
fun_progn(ref args) {
   ref last_val = nil;

   while (args != nil) {
      last_val = eval(get_next_arg(&args));
   } 

   return last_val;
}

ref 
fun_and(ref args) {
   ref res = T;

   while (args != nil) {
      ref obj = get_next_arg(&args);
      res = eval(obj);

      if (res == nil) {
         break; 
      }
   } 

   return res;
}

ref 
fun_or(ref args) {
   ref res = nil;

   while (args != nil) {
      ref obj = get_next_arg(&args);
      res = eval(obj);

      if (res != nil) {
         break; 
      }
   } 

   return res;
}

ref 
fun_let(ref args) {
   ref vars = get_next_arg(&args);
  
   if (get_type(vars) == cons_type && vars != nil) {
      while (vars != nil) {
         ref def = get_next_arg(&vars);
         if (get_type(def) == cons_type) {
            ref name = get_next_arg(&def);
            ref value = eval(cons_car(def));
            ensure_type(name, symbol_type);
            env_bind_local(name, value);
         } else if (get_type(def) == symbol_type) {
            env_bind_local(def, nil);
         } else {
            error("Illegal variable specification");
         }
      }
   }

   ref last_val = nil;

   while (args != nil) {
      last_val = eval(get_next_arg(&args));
   } 

   return last_val;
}
