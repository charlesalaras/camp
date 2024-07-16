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
  struct list_head* head; // Tracks free blocks
  size_t used; // Currently used memory
  size_t peak; // Peak memory usage
  void* ptr;
};

void pma_init(struct pma* self) {
  self->ptr = malloc(POOL_SIZE);

  self->used = 0;
  self->peak = 0;
  const int nBlocks = POOL_SIZE / BLOCK_SIZE;
  for (int i = 0; i < nBlocks; ++i) {
     ptr_t address = (ptr_t)(self->ptr + i * BLOCK_SIZE);
    list_add((struct list_node*)address, self->head);
  }
}

void* pmalloc(struct pma* self) {
  if (self->used + BLOCK_SIZE > POOL_SIZE) return NULL; // Out of memory
  // Only serve memory of BLOCK_SIZE,
  struct list_node* free_ptr = (struct list_node*)self->head->first;
  list_del(self->head);
  
  self->used += BLOCK_SIZE;
  self->peak = max(self->peak, self->used);

  return (void*)(free_ptr + sizeof(struct list_node*));
}

void pfree(struct pma* self, void* ptr) {
  self->used -= BLOCK_SIZE;
  list_add((struct list_node*)(ptr - sizeof(struct list_node*)), self->head);
}

void pma_exit(struct pma* self) {
  free(self->ptr);
}

#endif // _PMA_H_
