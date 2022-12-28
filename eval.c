#include "utils.h"
#include "cons.h"
#include "symbol.h"
#include "mstring.h"
#include "function.h"
#include "env.h"
#include "eval.h"
#include "debug.h"

extern ref nil;
extern ref nilsym;

ref 
eval_function(ref fun, ref args) {
   ref arg_names = fuser_get_args(fun);
   ref body = fuser_get_body(fun);

   if (get_type(arg_names) == cons_type) {
      if (args == nil) {
         error("Invalid nunber of arguments");
      }

      while (arg_names != nil && args != nil) {
         env_bind_local(cons_car(arg_names), cons_car(args));
         arg_names = cons_next(arg_names);
         args = cons_next(args);
      }

      if (args != arg_names) { // one variable is not nil
         error("Invalid nunber of arguments");
      }

   } else if (arg_names != nil) {
      DEBUG(DEBUG_EVAL, "Invalid argument declaration, type=%d\n", get_type(arg_names));
      error("Invalid argument declaration");
   }

   if (get_type(body) == cons_type) {
      body = cons_car(body);
   }

   return eval(body);
}

ref 
call(ref fun, ref args) {
   ref ret;

   env_push_frame();

   switch (get_type(fun)) {
      case sfbuiltin_type:
         ret = deref_sfbuiltin(fun)->f.ptr(args);
         break;
      case fbuiltin_type:
         ret = deref_fbuiltin(fun)->f.ptr(args);
         break;
      default:
         ret = eval_function(fun, args); 
         break;
   }

   env_pop_frame();

   return ret;
}

ref 
eval_list(ref list) {
   ref sym = cons_car(list);

   if (sym == nil || get_type(sym) != symbol_type) {
      error("Symbol expected"); 
   }

   ref fun = env_get_function(sym);

   // TODO: check whether args must be eval'd, i.e. fun isn't a macro

   if (!fun) {
      print_env();
      error("Unknown function called: %s", mstring_value(symbol_name(sym))); 
   }

   switch (get_type(fun)) {
      case sfbuiltin_type:
         return call(fun, cons_cdr(list)); 
      case fbuiltin_type:
      case fuser_type: {
         ref args = NULL;
         ref args_pos = NULL;
         ref arg = cons_cdr(list);

         while (arg != nil) {
            if (get_type(arg) == cons_type) {
               args_pos = cons_append(args_pos, eval(cons_car(arg))); 
               if (!args) {
                  args = args_pos; 
               }
               arg = cons_cdr(arg);
            } else {
               arg = nil; 
            }
         }

         if (!args) {
            args = nil;
         }

         return call(fun, args);
      }
      default:
         error("Invalid function");
   }

   return nil;
}

ref 
eval(ref obj) {
   switch (get_type(obj)) {
      case symbol_type:
         if (obj == nilsym) {
            return nil; 
         }
      case number_type:
      case string_type:
      case fbuiltin_type:
         return obj;
      case cons_type:
         return eval_list(obj);
      default:
         error("Eval error: unknown object of type: %d", get_type(obj));
   }

   return nil;
}
