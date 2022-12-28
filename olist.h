#ifndef _OLIST_H_
#define _OLIST_H_

#include <sys/types.h>
#include <stdbool.h>

typedef struct olist_entry {
   void *value;
   struct olist_entry *prev;
   struct olist_entry *next;
} olist_entry;

typedef struct {
   uint32_t size;
   olist_entry *first;
   olist_entry *last;
   int (*cmp_func)(void *, void *);
} olist;

typedef struct {
   olist *l;
   olist_entry *next;
} olist_it;

olist *olist_create(int (*cmp_func)(void *, void *));
void olist_destroy(olist *l);
bool olist_is_empty(olist *l);
u_int32_t olist_size(olist *l);
void olist_insert(olist *l, void *value);
void *olist_remove_first(olist *l);
void *olist_remove_last(olist *l);
void *olist_remove(olist *l, void *value);
void olist_apply(olist *l, void(*f)(void *));

olist_it *olist_it_create(olist *l);
void olist_it_destroy(olist_it *it);
bool olist_it_has_next(olist_it *it);
void *olist_it_get_next(olist_it *it);

#endif //_OLIST_H_
