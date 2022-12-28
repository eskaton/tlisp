#include <stdlib.h>
#include <stdbool.h>

#include "stack.h"

inline stack *
stack_create() {
   return (stack *)calloc(1, sizeof(stack));
}

inline void
stack_destroy(stack *s) {
   free(s);
}

void
stack_destroy_free(stack *s, void (*free_entry)(void *value)) {
   stack_entry *e = s->top;
   stack_entry *tmp;

   while (e) {
      tmp = e->prev; 
      free_entry(e->value);
      free(e);
      e = tmp;
   }

   free(s);
}

inline bool
stack_is_empty(stack *s) {
   return s->size == 0;
}

inline uint32_t
stack_size(stack *s) {
   return s->size;
}

void
stack_push(stack *s, void *value) {
   stack_entry *e = (stack_entry *)calloc(1, sizeof(stack_entry));
   e->value = value;
   
   if (!stack_is_empty(s)) {
      e->prev = s->top; 
   }

   s->top = e;
   s->size++;
}

void *
stack_pop(stack *s) {
   stack_entry *e; 
   void *value;

   if (stack_is_empty(s)) {
      return NULL; 
   }

   e = s->top; 
   s->top = e->prev;
   s->size--;
   value = e->value;
   free(e);
   
   return value;
}

inline void *
stack_peek(stack *s) {
   if (stack_is_empty(s)) {
      return NULL; 
   }

   return s->top->value;
}

stack_it *
stack_it_create(stack *s) {
   stack_it *it = (stack_it *)calloc(1, sizeof(stack_it));
   if (s) {
      it->s = s;
      it->next = s->top;
   }
   return it;
}

void
stack_it_destroy(stack_it *it) {
   free(it);
}

bool 
stack_it_has_next(stack_it *it) {
   return it->next != NULL;
}

void *
stack_it_get_next(stack_it *it) {
   stack_entry *e = it->next;

   if (e) {
      it->next = e->prev; 
      return e->value;
   } 

   return NULL;
}

