#ifndef _BUILTIN_H_
#define _BUILTIN_H_

#include "alloc.h"

#define defun(f) ref fun_##f(ref args)

// functions
defun(add);
defun(sub);
defun(mult);
defun(div);
defun(mod);
defun(sqrt);

defun(lval);
defun(gval);
defun(car);
defun(cdr);
defun(undef);
defun(set);
defun(setg);
defun(eval);
defun(quit);
defun(lt);
defun(le);
defun(eq);
defun(ne);
defun(ge);
defun(gt);
defun(not);
defun(print);
defun(cons);
defun(rplaca);
defun(rplacd);
defun(inc);
defun(dec);
defun(print_heap_stats);
defun(get_gc_stats);

// special functions
defun(defun);
defun(quote);
defun(if);
defun(loop);
defun(progn);
defun(let);
defun(and);
defun(or);

#endif //_BUILTIN_H_
