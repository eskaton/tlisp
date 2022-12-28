#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "collect.h"
#include "htab.h"
#include "olist.h"
#include "object.h"
#include "symbol.h"
#include "function.h"
#include "cons.h"
#include "env.h"
#include "utils.h"
#include "debug.h"
#include "arch.h"
#include "hex_dump.h"
#include "ctlstack.h"

static GcStats stats;
static heap *heapptr;
extern env *envp;
extern htab *symbols;
extern ref nil;
extern ref nilsym;
extern ref T;
extern ctlstack *ctlstack_ptr;

unsigned long *stack_base;

extern bool gc_log_opt;
static FILE *gc_log_file;

#define write_log_msg(msg) \
   if (gc_log_opt) write_log(msg, NULL);
#define write_log_msg_args(fmt, args...) \
   if (gc_log_opt) write_log(fmt, args);
#define write_hex_dump(addr, len) \
   if (gc_log_opt) hex_dump_to(gc_log_file, addr, len, (unsigned long)addr);
#define write_heap_dump() \
   if (gc_log_opt) heap_dump_raw_to(gc_log_file);
#define write_ctlstack_dump() \
   if (gc_log_opt) ctlstack_dump_raw_to(gc_log_file);

static void
write_log(const char *fmt, ...) {
   char buf[1024];
   va_list args;

   va_start(args, fmt);
   vsnprintf(buf, 1024, fmt, args);
   va_end(args);

   fprintf(gc_log_file, "%s\n", buf);
}

void
init_gc(heap *h) {
   heapptr = h;
   
   if (gc_log_opt) {
      char *file_pfix = "./tlisp_gc.";
      char *file_name = malloc(strlen(file_pfix) + (MAX_PID_LEN+1) * sizeof(char));
      sprintf(file_name, "%s%u", file_pfix, getpid());
      if (!(gc_log_file = fopen(file_name, "a+"))) {
         perror("Couldn't open gc logfile");
      };
      free(file_name);
   }
}

void
copy_object(ref *o) {
   if (!*o) {
      // NULL reference encountered
      return; 
   } 

   if (get_type(*o) == fwdptr_type) {
      *o = deref_fwdptr(*o)->ptr; 
      if (get_type(*o) == fwdptr_type) {
         printf("gc: still a pointer: %p\n", *o); 
      }
   } else {
      write_log_msg_args("gc: copying %p ...", *o);
      ref new_o = (ref)heapptr->to_free_ptr;
      size_t sz = object_size(*o);
      memcpy(new_o, *o, sz);
      write_hex_dump(*o, sz);
      set_type(*o, fwdptr_type);
      deref_fwdptr(*o)->ptr = new_o;
      *o = new_o;
      heapptr->to_free_ptr += sz;
      write_log_msg_args("gc: copied %u bytes to %p", sz, *o);
   }
}

void
copy_binding_values(stack *values) {
   stack_it *it = stack_it_create(values);

   while (stack_it_has_next(it)) {
      bframe *bf = stack_it_get_next(it); 
      copy_object(&bf->value);
   }

   stack_it_destroy(it);
}

void
copy_bindings(htab *ht) {
   htab_it *it = htab_it_create(ht);

   while (htab_it_has_next(it)) {
      htab_entry *ent = htab_it_get_next(it); 
      binding *bind = (binding *)ent->value;
      copy_object((ref *)&ent->key);
      copy_object((ref *)&bind->sym);
      copy_binding_values(bind->values);
   }

   htab_it_destroy(it);
   htab_rehash(ht);
}

void
copy_symtab() {
   htab_it *it = htab_it_create(symbols);

   while (htab_it_has_next(it)) {
      htab_entry *ent = htab_it_get_next(it); 
      copy_object((ref *)&ent->key);
      copy_object((ref *)&ent->value);
   }

   htab_it_destroy(it);
   htab_rehash(symbols);
}

void
copy_cstack_roots(unsigned long *top, unsigned long *bottom) {
   if (top < bottom) {
      error("Stack grows upward. This is not supported!"); 
   }

   write_log_msg_args("gc: copying c-stack %p - %p...", bottom, top);

   unsigned long *stack_pos = top;

   while (stack_pos >= bottom) {
      ref maybe_ptr = (ref)*stack_pos;
      if (is_from_ptr(maybe_ptr) ) {
         write_log_msg_args("gc: pointer to from space %p", maybe_ptr);
         if ((is_obj(maybe_ptr) || is_fwdptr(maybe_ptr))) {
            copy_object((ref *)stack_pos);
         } else {
            write_log_msg_args("gc: pointer to middle of object %p", maybe_ptr);
         }
      }
      stack_pos--;
   }
}
#ifdef __x86_64__
#define copy_reg_root(ptr, reg)                                   \
   asm ("movq %%"#reg",%0" : "=r" (ptr));                         \
   if (is_from_ptr(ptr) && (is_obj(ptr) || is_fwdptr(ptr))) {     \
      write_log_msg_args("gc: updating "#reg" from %p...", ptr);  \
      copy_object(&ptr);                                          \
      write_log_msg_args("gc: to %p", ptr);                       \
      asm ("movq %0,%%"#reg : "=m" (ptr));                        \
   }
#else
#define copy_reg_root(ptr, reg)                                   \
   asm ("movl %%"#reg",%0" : "=r" (ptr));                         \
   if (is_from_ptr(ptr) && (is_obj(ptr) || is_fwdptr(ptr))) {     \
      write_log_msg_args("gc: updating "#reg" from %p...", ptr);  \
      copy_object(&ptr);                                          \
      write_log_msg_args("gc: to %p", ptr);                       \
      asm ("movl %0,%%"#reg : "=m" (ptr));                        \
   }
#endif // __x86_64__

void
copy_reg_roots() {
   ref maybe_ptr;

#ifdef __x86_64__
   copy_reg_root(maybe_ptr, rax);
   copy_reg_root(maybe_ptr, rbx);
   copy_reg_root(maybe_ptr, rcx);
   copy_reg_root(maybe_ptr, rdx);
   copy_reg_root(maybe_ptr, rdi);
   copy_reg_root(maybe_ptr, rsi);
#else
   copy_reg_root(maybe_ptr, eax);
   copy_reg_root(maybe_ptr, ebx);
   copy_reg_root(maybe_ptr, ecx);
   copy_reg_root(maybe_ptr, edx);
   copy_reg_root(maybe_ptr, edi);
   copy_reg_root(maybe_ptr, esi);
#endif // __x86_64__
}

void
copy_ctlstack() {
   register ref pos;

   for (pos = ctlstack_ptr->start; pos < ctlstack_ptr->sp; pos += 2) {
      if (*pos != 0x0) {
         copy_object((ref *)pos);
         copy_object((ref *)pos+1);
      }
   }

}

void
copy_roots(env *e) {
   write_log_msg("gc: copying static symbols...");
   copy_object(&nil);
   copy_object(&nilsym);
   copy_object(&T);
   write_log_msg("gc: copying symbol table...");
   copy_symtab();
   write_log_msg("gc: copying global bindings...");
   copy_bindings(e->globals);
   write_log_msg("gc: copying local bindings...");
   copy_ctlstack();
}

void
copy_refs(ref *o) {
   switch(get_type(*o)) {
      case fwdptr_type: 
         *o = deref_fwdptr(*o)->ptr; 
         break;
      case number_type: // fall through
      case string_type: // fall through
      case fbuiltin_type: // fall through
      case sfbuiltin_type:
         /* these objects have no references */
         break;
      case symbol_type:
         copy_object(&deref_symbol(*o)->name);
         break;
      case cons_type:
         copy_object(&deref_cons(*o)->car);
         copy_object(&deref_cons(*o)->cdr);
         break;
      case fuser_type:
         copy_object(&deref_function(*o)->f.def);
         break;
      case binding_type:
         copy_object(&deref_binding(*o)->sym);
         copy_object(&deref_binding(*o)->value);
         break;
      default:
         error("copy_refs(): unhandled type %d @ %p", get_type(*o), *o);
   }
}

bool
validate_object(ref o) {
   if (!is_from_ptr(o)) {
      return false; 
   }

   switch(get_type(o)) {
      case fwdptr_type: 
         return false;
      case number_type: // fall through
      case string_type: // fall through
      case fbuiltin_type: // fall through
      case sfbuiltin_type:
         /* these objects have no references */
         return true;
      case symbol_type:
         if (is_fwdptr(deref_symbol(o)->name) ||
               !is_from_ptr(deref_symbol(o)->name)) {
            return false; 
         }
         return true;
      case cons_type:
         if (is_fwdptr(deref_cons(o)->car) || is_fwdptr(deref_cons(o)->cdr) ||
               !is_from_ptr(deref_cons(o)->car) || 
               !is_from_ptr(deref_cons(o)->car)) {
            return false; 
         }
         return true;
      case binding_type:
         if (is_fwdptr(deref_binding(o)->sym) || is_fwdptr(deref_binding(o)->value) ||
               !is_from_ptr(deref_binding(o)->sym) || 
               !is_from_ptr(deref_binding(o)->value)) {
            return false; 
         }
         return true;
      case fuser_type:
         if (is_fwdptr(deref_function(o)->f.def) || 
               !is_from_ptr(deref_function(o)->f.def)) { 
            return false; 
         }
         return true;
      default:
         write_log_msg_args("validate_object(): unhandled type %d @ %p", get_type(o), o);
         return false;
   }
}

bool
validate_heap() {
   char *unscanned = heapptr->from;
   bool valid = true;

   while (unscanned < heapptr->from_free_ptr) {
      ref o = (ref)unscanned; 
      size_t sz = object_size(o);
      valid = validate_object(o);
      unscanned += sz;
   }

   if (!valid) {
      write_log_msg("gc: heap invalid after collect. "
            "a dump of the corrupted heap follows");
      write_heap_dump();
   }

   return valid;
}

bool
validate_cstack(unsigned long *top, unsigned long *bottom) {
   bool valid = true;
   unsigned long *stack_pos = top;

   while (stack_pos > bottom) {
      //long maybe_ptr = *(long *)stack_pos;
      ref maybe_ptr = (ref)*stack_pos;
      if (is_from_ptr(maybe_ptr)) { 
         if (is_obj(maybe_ptr) || get_type(maybe_ptr) == fwdptr_type) {
            if (!validate_object(maybe_ptr)) {
               write_log_msg_args("validate_cstack(): invalid object %p @ %p", 
                     *(long *)maybe_ptr, maybe_ptr);
               valid = false;
            }
         }
      } else if (is_to_ptr(maybe_ptr)) {
            write_log_msg_args("validate_cstack(): dangling pointer %p @ %p?", 
                  *(long *)maybe_ptr, maybe_ptr);
      }
      stack_pos--;
   }

   return valid;
}

bool
validate_binding_values(stack *values) {
   bool valid = true;
   stack_it *it = stack_it_create(values);

   while (stack_it_has_next(it)) {
      bframe *bf = stack_it_get_next(it); 
      if (!validate_object(bf->value)) {
         write_log_msg_args("validate_binding_values(): Invalid opject 0x%lx @ %p\n", 
               *bf->value, bf->value);
         valid = false; 
      }
   }

   stack_it_destroy(it);

   return valid;
}

bool
validate_bindings(htab *ht) {
   bool valid = true;
   htab_it *it = htab_it_create(ht);

   while (htab_it_has_next(it)) {
      htab_entry *ent = htab_it_get_next(it); 
      binding *bind = (binding *)ent->value;
      if (!validate_object(ent->key)) {
         write_log_msg_args("validate_bindings(): Invalid opject 0x%lx @ %p\n", 
               *(ref)ent->key, ent->key);
         valid = false; 
      }
      if (!validate_object(bind->sym)) {
         write_log_msg_args("validate_bindings(): Invalid opject 0x%lx @ %p\n", 
               *bind->sym, bind->sym);
         valid = false; 
      }
      if (!validate_binding_values(bind->values)) {
         valid = false; 
      }
   }

   htab_it_destroy(it);

   return valid; 
}

bool
validate_ctlstack() {
   bool valid = true;
   ref pos;

   for (pos = ctlstack_ptr->start; pos < ctlstack_ptr->sp; pos += 2) {
      if (*pos != 0x0) {
         if (!validate_object(*(ref *)pos)) {
            write_log_msg_args("validate_ctlstack(): Invalid opject 0x%lx @ %p\n", 
                  *pos, pos);
            valid = false; 
         }
         if (!validate_object(*(ref *)(pos+1))) {
            write_log_msg_args("validate_ctlstack(): Invalid opject 0x%lx @ %p\n", 
                  *(pos+1), pos+1);
            valid = false; 
         }
      }
   }

   return valid;
}

bool
validate_symtab() {
   bool valid = true;
   htab_it *it = htab_it_create(symbols);

   while (htab_it_has_next(it)) {
      htab_entry *ent = htab_it_get_next(it); 
      if (!validate_object(ent->key)) {
         write_log_msg_args("validate_symtab(): Invalid opject 0x%lx @ %p\n", 
               *(ref)ent->key, ent->key);
         valid = false; 
      }
      if (!validate_object(ent->value)) {
         write_log_msg_args("validate_symtab(): Invalid opject 0x%lx @ %p\n", 
               *(ref)ent->value, ent->value);
         valid = false; 
      }
   }

   htab_it_destroy(it);

   return valid;
}

void
validate(unsigned long *top, unsigned long *bottom) {
   bool valid = true;
   if (!validate_heap()) valid = false;
   if (!validate_cstack(top, bottom)) valid = false;
   if (!validate_ctlstack()) valid = false;
   if (!validate_bindings(envp->globals)) valid = false;
   if (!validate_symtab()) valid = false;

   if (!valid) {
      error("gc: failed!"); 
   }
}

void
start_gc() {
   struct timeval begin, end;
   unsigned long *stack_bottom;
   uint32_t old_consumption = heapptr->from_free_ptr - heapptr->from;
   uint32_t freed;

   gettimeofday(&begin, NULL);

   get_stack_ptr(stack_bottom);

   write_log_msg_args("gc: started compacting heap from %p-%p -> to %p",
         heapptr->from, heapptr->from_free_ptr, heapptr->to); 
   write_heap_dump();
   write_ctlstack_dump();

   copy_reg_roots();
   copy_cstack_roots(stack_base, stack_bottom);
   copy_roots(envp);

   char *unscanned = heapptr->to;

   while (unscanned < heapptr->to_free_ptr) {
      ref o = (ref)unscanned; 
      size_t sz = object_size(o);
      copy_refs(&o);
      unscanned += sz;
   }

   char *tmp = heapptr->from;
   heapptr->from = heapptr->to;
   heapptr->to = tmp;
   heapptr->from_free_ptr = heapptr->to_free_ptr; 
   heapptr->to_free_ptr = heapptr->to;
   heapptr->gc_trigger_addr = heapptr->from_free_ptr +
      ((size_t)(((heapptr->from + (HEAP_SIZE>>1)) - 
            heapptr->from_free_ptr) * 0.9) & ~0x3);

   freed = old_consumption - (heapptr->from_free_ptr - heapptr->from);
  
   write_log_msg_args("gc: compacted heap to %p-%p",
         heapptr->from, heapptr->from_free_ptr);
   write_log_msg_args("gc: %d bytes freed, %d bytes available", freed,
         heapptr->from + (HEAP_SIZE>>1) - heapptr->from_free_ptr);

   validate(stack_base, stack_bottom);
   write_log_msg("gc: finished successfully. new heap follows:"); 
   write_heap_dump();
   write_log_msg("gc: control stack:"); 
   write_ctlstack_dump();

   gettimeofday(&end, NULL);

   stats.runs++;
   stats.bytes += freed;
   stats.elapsed.tv_sec += end.tv_sec - begin.tv_sec;
   stats.elapsed.tv_usec += end.tv_usec - begin.tv_usec; 

   if (end.tv_sec > begin.tv_sec && end.tv_usec < begin.tv_usec) {
      stats.elapsed.tv_sec--;
      stats.elapsed.tv_usec += 1000000; 
   }

   if (stats.elapsed.tv_usec >= 1000000) {
      stats.elapsed.tv_sec++;
      stats.elapsed.tv_usec -= 1000000;
   }
}

GcStats *
gc_get_stats() {
   return &stats;
}

