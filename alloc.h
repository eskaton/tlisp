#ifndef _ALLOC_H_
#define _ALLOC_H_

#include <stdio.h>

#include "olist.h"
#include "htab.h"
#include "assert.h"

#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

#define ALIGN 0x4
// TODO: configurable heap size
#define HEAP_SIZE (4UL<<24)
#define get_blk_size(bytes) (bytes + ((ALIGN - (bytes & 0x3)) & 0x3))

#define assert_valid_ptr(ptr) (assert(is_from_ptr(ptr)))
#define is_from_ptr(ptr) (ptr >= (ref)heapptr->from && ptr < (ref)heapptr->from_free_ptr)
#define is_to_ptr(ptr) (ptr >= (ref)heapptr->to && ptr < (ref)heapptr->to_free_ptr)

#ifdef __x86_64__
typedef uint64_t obj;
#else
typedef uint32_t obj;
#endif // __x86_64__
typedef obj* ref;

typedef struct {
   char *from;
   char *from_free_ptr;
   char *to;
   char *to_free_ptr;
   char *gc_trigger_addr;
} heap;

ref talloc(size_t bytes);
ref trealloc(ref r, size_t bytes);
void tfree(ref r);

u_long address_hash(void *addr);
bool address_equals(void *addr1, void *addr2);
short get_exp(size_t blk);
bool expand_heap(heap *h, size_t min_bytes);
void *get_free_block(size_t bs);
void heap_create();
void heap_dump_raw();
void heap_dump_raw_to(FILE *dest);
void heap_dump();
void print_heap_stats();

#endif // _ALLOC_H_
