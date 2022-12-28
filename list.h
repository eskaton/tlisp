#ifndef _LIST_H_
#define _LIST_H_

#include <sys/types.h>
#include <stdbool.h>

typedef struct list_entry {
   void *value;
   struct list_entry *prev;
   struct list_entry *next;
} list_entry;

typedef struct {
   uint32_t size;
   list_entry *first;
   list_entry *last;
} list;

typedef struct {
   list *l;
   list_entry *next;
} list_it;

list *list_create();
void list_destroy(list *l);
bool list_is_empty(list *l);
uint32_t list_size(list *l);
void list_insert(list *l, void *value);
void list_append(list *l, void *value);
void *list_remove_first(list *l);
void *list_remove_last(list *l);
void list_apply(list *l, void(*f)(void *));

list_it *list_it_create(list *l);
void list_it_destroy(list_it *it);
bool list_it_has_next(list_it *it);
void *list_it_get_next(list_it *it);

#endif //_LIST_H_
