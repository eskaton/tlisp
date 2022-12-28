#ifndef _ARCH_H_
#define _ARCH_H_

#ifdef __CYGWIN__
#include <setjmp.h>
#define get_stack_base(sb) \
   __asm ("movl %%ebp,%0" : "=r" (sb));

#define get_stack_ptr(sb) \
   __asm ("movl %%esp,%0" : "=r" (sb));

#else
#include <ucontext.h>
#ifdef __x86_64__
#define get_stack_base(sb) \
   ucontext_t ctx; \
   getcontext(&ctx); \
   sb = (unsigned long *)ctx.uc_mcontext.mc_rbp;

#define get_stack_ptr(sb) \
   ucontext_t ctx; \
   getcontext(&ctx); \
   sb = (unsigned long *)ctx.uc_mcontext.mc_rsp;
#else
#define get_stack_base(sb) \
   ucontext_t ctx; \
   getcontext(&ctx); \
   sb = (unsigned long *)ctx.uc_mcontext.mc_ebp;

#define get_stack_ptr(sb) \
   ucontext_t ctx; \
   getcontext(&ctx); \
   sb = (unsigned long *)ctx.uc_mcontext.mc_esp;
#endif // __x86_64__ 
#endif // __CYGWIN__

#endif // _ARCH_H_
