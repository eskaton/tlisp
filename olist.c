#include <stdlib.h>
#include <stdbool.h>
#include "olist.h"

olist *
olist_create(int (*cmp_func)(void *, void *)) {
   olist *l = (olist *)calloc(1, sizeof(olist));
   l->cmp_func = cmp_func;
   return l;
}

void
olist_destroy(olist *l) {
   olist_entry *e = l->first, *tmp;

   while (e) {
      tmp = e->next;
      free(e);
      e = tmp;
   }

   free(l);
}

inline bool
olist_is_empty(olist *l) {
   return l->size == 0;
}

inline uint32_t
olist_size(olist *l) {
   return l->size;
}

void
olist_insert(olist *l, void *value) {
   olist_entry *e = (olist_entry *)calloc(1, sizeof(olist_entry));
   e->value = value;
   
   if (!olist_is_empty(l)) {
      olist_entry *cur = l->first;

      while (cur) {
         if (l->cmp_func(value, cur->value) <= 0) {
            e->next = cur; 
            e->prev = cur->prev;
            cur->prev = e;
            if (e->prev) {
               e->prev->next = e;
            } else {
               l->first = e;
            }
            break;
         } 
         cur = cur->next; 
      }

      if (!cur) {
         e->prev = l->last; 
         l->last->next = e;
         l->last = e;
      }

   } else {
      l->last = e;
      l->first = e;
   }

   l->size++;
}

void *
olist_remove_first(olist *l) {
   olist_entry *e; 
   void *value;

   if (olist_is_empty(l)) {
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
olist_remove_last(olist *l) {
   olist_entry *e; 
   void *value;

   if (olist_is_empty(l)) {
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

void *
olist_remove(olist *l, void *value) {
   olist_entry *cur = l->first;

   while (cur) {
      if (!l->cmp_func(value, cur->value)) {
         if (cur->prev) {
            cur->prev->next = cur->next;
         } else {
            l->first = cur->next;
         }

         if (cur->next) {
            cur->next->prev = cur->prev;
         } else {
            l->last = cur->prev;
         }

         void *ret = cur->value;
         free(cur);
         l->size--;
         return ret;
      }
      cur = cur->next;
   }

   return NULL;
}

void
olist_apply(olist *l, void(*f)(void *)) {
   olist_it *it = olist_it_create(l); 
   while (olist_it_has_next(it)) {
      void *value = olist_it_get_next(it);
      if (value) {
         f(value); 
      }
   }
   olist_it_destroy(it);
}

olist_it *
olist_it_create(olist *l) {
   olist_it *it = (olist_it *)calloc(1, sizeof(olist_it));
   if (l) {
      it->l = l;
      it->next = l->first;
   }
   return it;
}

void
olist_it_destroy(olist_it *it) {
   free(it);
}

bool 
olist_it_has_next(olist_it *it) {
   return it->next != NULL;
}

void *
olist_it_get_next(olist_it *it) {
   olist_entry *e = it->next;

   if (e) {
      it->next = e->next; 
      return e->value;
   } 

   return NULL;
}

