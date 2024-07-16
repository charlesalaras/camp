// Based on Linux's llist.h

#ifndef _LIST_H_
#define _LIST_H_

#include "types.h"
#include <stddef.h>

struct list_head {
  struct list_node *first;
};

struct list_node {
  struct list_node* next;
};

bool list_empty(const struct list_head* head) {
  return head->first == NULL;
}

bool list_add(struct list_node* new, struct list_head* head) {
  struct list_node *entry, *old_entry;
  entry = head->first;
  for (;;) {
    old_entry = entry;
    new->next = entry;
    // Non-atomic don't run this threaded
    entry = head->first == old_entry ? head->first = new : head->first;
    if (entry == old_entry) break;
  }

  return old_entry == NULL;
}

bool list_del(struct list_head* head) {
  struct list_node *entry, *old_entry, *next;

  entry = head->first;
  for (;;) {
    if (entry == NULL) return NULL;
    old_entry = entry;
    next = entry->next;
    entry = head->first == old_entry ? head->first = next : head->first;
    if (entry == old_entry) break;
  }

  return entry;
}

#endif
