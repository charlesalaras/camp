// Directed acyclic graph (dag) implementation
// This is used for a render graph
// Based on code by James Aspnes, Yale University
// http://www.cs.yale.edu/homes/aspnes/classes/223/notes.html#graphs
#ifndef _DAG_H_
#define _DAG_H_

#include "types.h"
#include <stdlib.h>
/* basic directed graph type */
struct graph {
  int v;              /* number of vertices */
  int e;              /* number of edges */
  struct successors {
    int d;            /* number of successors */
    int len;          /* number of slots in array */
    //bool is_sorted;   /* true if list is already sorted */
    int list[];      /* actual list of successors */
  } *alist[];
};

/* create a new graph with n vertices labeled 0..n-1 and no edges */
struct graph* graph_init(int n) {
  struct graph* g = malloc(sizeof(struct graph) + sizeof(struct successors*) * n);
  g->v = n;
  g->e = 0;

  int i;
  for (i = 0; i < n; i++) {
    g->alist[i] = malloc(sizeof(struct successors));

    g->alist[i]->d = 0;
    g->alist[i]->len = 0;
  }
}

/* free all space used by graph */
void graph_exit(struct graph* self) {
  int i;
  for (i = 0; i < self->v; i++) free(self->alist[i]);
  free(self);
}

/* add an edge to an existing graph */
void graph_add_edge(struct graph* self, int u, int v) {
  /* DISCLAIMER:
  There's not really a good way to avoid a realloc here:
  We could use our list implementation for each successor list?
  But then we need to malloc nodes anyways, so the cost hasn't been mitigated.
  We could use a simple allocator like a linear / freelist?
  But that's a LOT of overhead, as we have to expect n*(n-1) total edges
  At the very least we try to amortize cost with this realloc by doubling.
  The issue is overallocation may occur.
  We're gonna mitigate that by implementing a max of (n-1).
  Remember, a vertex in a DAG can at max have n - 1 edges.
  The ultimatum is this:
  Build your graph BEFORE intensive computation!
  Before any rendering happens at all set up the render graph
  and render passes if possible.
  Collect all the graphs if you have to and allocate up front
  */
  while (self->alist[u]->d >= self->alist[u]->len) {
    self->alist[u]->len = min(self->alist[u]->len * 2 + 1, self->v - 1);
    self->alist[u] = realloc(
      self->alist[u], 
      sizeof(struct successors) + sizeof(int) * self->alist[u]->len
    );
  }
  /* now add the new sink */
  self->alist[u]->list[self->alist[u]->d++] = v;
  /* bump edge count */
  self->e++;
}

/* return 1 if edge (source, sink) exists), 0 otherwise */
bool graph_has_edge(struct graph* self, int source, int sink) {
  /* DISCLAIMER:
  If you use this function, you will incur pretty significant time cost
  Mainly because it does a linear search, O(n^2).
  */
  int i;
  for (i = 0; i < self->alist[source]->d; i++) {
    if (self->alist[source]->list[i] == sink) return true;
  }

  return false;
}

/* invoke f on all edges (u,v) with source u */
/* supplying data as final parameter to f */
/* no particular order is guaranteed */
void graph_foreach(struct graph* self, int source,
        void (*f)(struct graph* g, int source, int sink, void *data),
        void *data) {
  int i;
  for (i = 0; i < self->alist[source]->d; i++) {
    f(self, source, self->alist[source]->list[i], data);
  }
}

#endif
