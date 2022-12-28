#include <stdlib.h>
#include <stdbool.h>
#include "list.h"

list *
list_create() {
   return (list *)calloc(1, sizeof(list));
}

void
list_destroy(list *l) {
   list_entry *e = l->first, *tmp;

   while (e) {
      tmp = e->next;
      free(e);
      e = tmp;
   }

   free(l);
}

inline bool
list_is_empty(list *l) {
   return l->size == 0;
}

inline uint32_t
list_size(list *l) {
   return l->size;
}

void
list_insert(list *l, void *value) {
   list_entry *e = (list_entry *)calloc(1, sizeof(list_entry));
   e->value = value;
   
   if (!list_is_empty(l)) {
      e->next = l->first;
      l->first->prev = e;
   } else {
      l->last = e;
   }

   l->first = e;
   l->size++;
}

void
list_append(list *l, void *value) {
   list_entry *e = (list_entry *)calloc(1, sizeof(list_entry));
   e->value = value;
   
   if (!list_is_empty(l)) {
      l->last->next = e;
      e->prev = l->last;
   } else {
      l->first = e; 
   } 

   l->last = e;
   l->size++;
}

void *
list_remove_first(list *l) {
   list_entry *e; 
   void *value;

   if (list_is_empty(l)) {
      return NULL; 
   }

   e = l->first;

   if (--l->size == 0)  {
      l->last = NULL; 
      l->first = NULL; 
   } else if (e->next) {
      e->next->prev = NULL;
      l->first = e->next;
   }

   value = e->value;
   free(e);
   
   return value;
}

void *
list_remove_last(list *l) {
   list_entry *e; 
   void *value;

   if (list_is_empty(l)) {
      return NULL; 
   }

   e = l->last;

   if (--l->size == 0)  {
      l->last = NULL; 
      l->first = NULL; 
   } else if (e->prev) {
      e->prev->next = NULL;
      l->last = e->prev;
   }

   value = e->value;
   free(e);
   
   return value;
}

void
list_apply(list *l, void(*f)(void *)) {
   list_it *it = list_it_create(l); 
   while (list_it_has_next(it)) {
      void *value = list_it_get_next(it);
      if (value) {
         f(value); 
      }
   }
   list_it_destroy(it);
}

list_it *
list_it_create(list *l) {
   list_it *it = (list_it *)calloc(1, sizeof(list_it));
   if (l) {
      it->l = l;
      it->next = l->first;
   }
   return it;
}

void
list_it_destroy(list_it *it) {
   free(it);
}

bool 
list_it_has_next(list_it *it) {
   return it->next != NULL;
}

void *
list_it_get_next(list_it *it) {
   list_entry *e = it->next;

   if (e) {
      it->next = e->next; 
      return e->value;
   } 

   return NULL;
}

