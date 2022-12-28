#ifndef _ENV_H_
#define _ENV_H_

#include "htab.h"
#include "binding.h"
#include "alloc.h"

typedef struct {
   int frame;
   htab *globals;
   htab *locals;
} env;

void env_init(); 
void env_push_frame();
void env_pop_frame();
void env_bind_global(ref sym, ref value);
binding *env_get_global(ref sym);
void env_unbind_global(ref sym);
void env_bind_local(ref sym, ref value);
void env_rebind_local(ref sym, ref value);
Binding *env_get_local(ref sym);
ref env_get_function(ref sym);
void print_env();

#endif //_ENV_H_
