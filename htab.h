#ifndef _HTAB_H_
#define _HTAB_H_

#include <stdbool.h>

#define HSIZE     16 /* Initial size for the hashtable */
#define REBUILD    3 /* Rebuild factor */

typedef struct htab_entry {
  void *value;                 /* Pointer to the data */
  void *key;                   /* Hashkey */
  struct htab_entry** fstbuck; /* Pointer to the first slot */
  struct htab_entry* nexth;    /* Pointer to next htab_entry */
} htab_entry;

typedef struct htab {
  htab_entry** buckets;  /* Array of pointers to htab_entrys */
  int num;               /* Number of entries */
  int maxent;            /* Maximum number of htab_entrys before rebuild */
  int htsize;            /* Size of the table */
  int mask;              /* Ensure a key is smaller than the number of buckets */
  bool (*eqfunc)();      /* Comperator function to find the matching entry */
  long (*hashfunc)();    /* Hash function to calculate the key */
} htab;

typedef struct {
   htab *ht;
   htab_entry *next;
   htab_entry **bucket;
} htab_it;


htab *htab_create(void *fhash, void *fequals);
void htab_destroy(htab *ht);
void htab_destroy_free(htab *ht, void (*free)(htab_entry *entry));
void *htab_get(htab* ht, void *key);
htab_entry *htab_get_entry(htab *ht, void *key);
bool htab_contains(htab *ht, void *key);
htab_entry *htab_put(htab* ht, void *key, void *value);
htab_entry *htab_delete(htab *ht, void *key);
void htab_rehash(htab *ht);
void htab_entry_destroy(htab_entry *entry);

htab_it *htab_it_create(htab *ht);
void htab_it_destroy(htab_it *it);
bool htab_it_has_next(htab_it *it);
htab_entry *htab_it_get_next(htab_it *it);
void htab_print_state(htab *ht);

#endif //_HTAB_H_

