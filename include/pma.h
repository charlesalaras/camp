// Pool Allocator
// Based on https://github.com/mtrebi/memmory-allocators
#ifndef _PMA_H_
#define _PMA_H_
#include "types.h"
#include "list.h"
#include <stddef.h>
#include <stdlib.h>

// Ignoring these conditions leads to **UNDEFINED BEHAVIOR**
// BLOCK_SIZE < POOL_SIZE
// POOL_SIZE % BLOCK_SIZE == 0
#define BLOCK_SIZE 64
#define POOL_SIZE 4096

// Quick Theory:
/*
The only time it makes sense to use a linked list here is when
the pool size is unknown. That's not really true though, if we
first ask the user for the pool size, then we can allocate an
array with the right bytesize. But the issue lies in the fact
that the array CANNOT represent actual data, otherwise we risk
it being overwritten by the stack pointer updating. If the array
instead represented addresses to memory, then we have allocated
an extra BLOCK_SIZE pointers to track the memory. Thus, linked
list should be most efficient here
*/


struct pma {
  struct list_head head; // Tracks free blocks
  size_t bs; // Block size
  size_t ps; // Pool size
  size_t used; // Currently used memory
  size_t peak; // Peak memory usage
  void* ptr;
};

void pma_init(struct pma* self, size_t block_size, size_t pool_size) {
  self->head.first = NULL;
  self->ptr = malloc(pool_size);

  self->bs = block_size;
  self->ps = pool_size;
  
  self->used = 0;
  self->peak = 0;
  const int nBlocks = pool_size / block_size;
  for (int i = 0; i < nBlocks; ++i) {
     ptr_t address = (ptr_t)(self->ptr + i * block_size);
    list_add((struct list_node*)address, &self->head);
  }
}

void* pmalloc(struct pma* self) {
  if (list_empty(&self->head)) return NULL; // Out of memory
  // Only serve memory of BLOCK_SIZE,
  struct list_node* free_ptr = (struct list_node*)self->head.first;
  list_del(&self->head);
  
  self->used += self->bs;
  self->peak = max(self->peak, self->used);

  return (void*)free_ptr;
}

void pfree(struct pma* self, void* ptr) {
  self->used -= self->bs;
  list_add((struct list_node*)ptr, &self->head);
}

void pma_exit(struct pma* self) {
  free(self->ptr);
}

#endif // _PMA_H_
