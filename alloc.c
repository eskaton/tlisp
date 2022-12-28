#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "alloc.h"
#include "collect.h"
#include "object.h"
#include "utils.h"
#include "debug.h"
#include "hex_dump.h"
#include "print.h"

static heap *heapptr;

void *
get_free_block(size_t bs) {
   if (heapptr->from_free_ptr + bs >= heapptr->gc_trigger_addr) {
      start_gc();
      if (heapptr->from_free_ptr + bs >= heapptr->from + (HEAP_SIZE>>1)) {
         error("Can't allocate %u bytes of heap memory. Out of memory", bs); 
      }
   } 

   void *blk = heapptr->from_free_ptr;
   heapptr->from_free_ptr += bs;
   return blk;
}

ref
talloc(size_t bytes) {
   if (!bytes) {
      return NULL; 
   }

   size_t bs = get_blk_size(bytes);
   ref r = get_free_block(bs);

   if (r) {
      DEBUG(DEBUG_ALLOC, "allocating block of size %d @ %p\n", bs, r);
      bzero(r, bs);
      return r;
   } else {
      DEBUG(DEBUG_ALLOC, "allocating block of size=%d failed\n", bs);
      error("memory exhausted");
   }

   return NULL; 
}

ref
trealloc(ref r, size_t bytes) {
   size_t bs = get_blk_size(bytes);
   ref new_r = get_free_block(bs);

   if (new_r) {
      DEBUG(DEBUG_ALLOC, "reallocating block %p, size=%d to %p, size=%d\n", 
            r, object_size(r), new_r, bs);
      memcpy(new_r, r, object_size(r));
      return new_r;
   } else {
      DEBUG(DEBUG_ALLOC, "reallocating block 0x%lx of size=%d failed\n", (long)r, bs);
      error("memory exhausted");
   }
   
   return NULL;
}

void
tfree(ref r) {
   // TODO: implement
}

void
print_heap_stats() {
   printf("total_memory_available: %lu\n", HEAP_SIZE>>1);
   printf("total_memory_used: %lu\n", 
         (unsigned long)heapptr->from_free_ptr - (unsigned long)heapptr->from);
}

void
heap_dump_raw_to(FILE *dest) {
   fprintf(dest, "from area:\n");
   hex_dump_to(dest, heapptr->from, heapptr->from_free_ptr - heapptr->from,
         (unsigned long)heapptr->from);

   /* If we're in the middle of a collect phase, 
      also dump what we've copied so far */
   if (heapptr->to != heapptr->to_free_ptr) {
      fprintf(dest, "to area:\n");
      hex_dump_to(dest, heapptr->to, heapptr->to_free_ptr - heapptr->to,
            (unsigned long)heapptr->to);
   }
}

void
heap_dump_raw() {
   heap_dump_raw_to(stderr);
}

void
heap_dump() {
   char *addr = heapptr->from;

   while (addr < heapptr->from_free_ptr) {
      ref obj = (ref)addr;
      size_t sz = object_size(obj);
      hex_dump(obj, sz, (unsigned long)obj);
      printf("type:  %s\n", get_type_str(obj));
      printf("value: ");
      print(obj);
      printf("\n");
      addr += sz;
   }
}

void
heap_create() {
   heapptr = (heap *)calloc(1, sizeof(heap));
   heapptr->from = malloc(HEAP_SIZE);
   heapptr->from_free_ptr = heapptr->from;
   heapptr->to = heapptr->from + (HEAP_SIZE>>1);
   heapptr->to_free_ptr = heapptr->to;
   heapptr->gc_trigger_addr = heapptr->from + ((size_t)((HEAP_SIZE>>1)*0.9) & ~0x3);
   init_gc(heapptr);
}
