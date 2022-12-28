#ifndef _STACK_H_
#define _STACK_H_

#include <stdbool.h>
#include <sys/types.h>

typedef struct stack_entry {
   void *value;
   struct stack_entry *prev;  
} stack_entry;

typedef struct {
   uint32_t size;
   stack_entry *top;
} stack;

typedef struct {
   stack *s;
   stack_entry *next;
} stack_it;

stack *stack_create();
void stack_destroy(stack *s);
void stack_destroy_free(stack *s, void (*free_entry)(void *value));
bool stack_is_empty(stack *s);
u_int32_t stack_size(stack *s);
void stack_push(stack *s, void *value);
void *stack_pop(stack *s);
void *stack_peek(stack *s);

stack_it *stack_it_create(stack *s);
void stack_it_destroy(stack_it *it);
bool stack_it_has_next(stack_it *it);
void *stack_it_get_next(stack_it *it);

#endif //_STACK_H_
