// Directed acyclic graph (dag) implementation
// This is used for a render graph
// Based on code by James Aspnes, Yale University
// http://www.cs.yale.edu/homes/aspnes/classes/223/notes.html#graphs
#ifndef _DAG_H_
#define _DAG_H_

#include "types.h"
#include <stdlib.h>
#include <string.h>
#ifndef DEBUG
//#include "debug.h"
#include <stdio.h>
#endif
/* basic directed graph type */
struct graph {
  int v;              /* number of vertices */
  int e;              /* number of edges */
  int* ordering;      /* Topological sorting of nodes */
  struct successors {
    int id;           /* in-degree */
    int od;            /* out-degree */
    int len;          /* number of slots in array */
    //bool is_sorted;   /* true if list is already sorted */
    void* data;       /* Data payload of each node */
    int list[];      /* actual list of successors */
  } *alist[];
};

/* create a new graph with n vertices labeled 0..n-1 and no edges */
struct graph* graph_init(int n) {
  struct graph* g = malloc(sizeof(struct graph) + sizeof(struct successors*) * n);
  g->v = n;
  g->e = 0;

  g->ordering = malloc(sizeof(int) * n);
  memset((void*)g->ordering, 0, n * sizeof(int));

  int i;
  for (i = 0; i < n; i++) {
    g->alist[i] = malloc(sizeof(struct successors));

    g->alist[i]->id = 0;
    g->alist[i]->od = 0;
    g->alist[i]->len = 0;
  }

  return g;
}

/* free all space used by graph */
void graph_exit(struct graph* self) {
  int i;
  free(self->ordering);
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
  while (self->alist[u]->od >= self->alist[u]->len) {
    self->alist[u]->len = min(self->alist[u]->len * 2 + 1, self->v - 1);
    self->alist[u] = realloc(
      self->alist[u], 
      sizeof(struct successors) + sizeof(int) * self->alist[u]->len
    );
  }
  /* now add the new sink */
  self->alist[u]->list[self->alist[u]->od++] = v;
  /* bump edge count */
  self->e++;
  /* bump in-degree of v */
  self->alist[v]->id++;
}

/* return 1 if edge (source, sink) exists), 0 otherwise */
bool graph_has_edge(struct graph* self, int source, int sink) {
  /* DISCLAIMER:
  If you use this function, you will incur pretty significant time cost
  Mainly because it does a linear search, O(n^2).
  */
  int i;
  for (i = 0; i < self->alist[source]->od; i++) {
    if (self->alist[source]->list[i] == sink) return true;
  }

  return false;
}

/* Kahn's Algorithm Topological Sort */
/* Ordering is the array with a sorted node list */
/* DISCLAIMER:
   Note that although the graph will remain traversable, the id variable is effectively dead
   The only way you can get the id variable back is by reconstructing the graph or
   re-search the in degrees. This is a semi-destructive function,
   in that you lose all your in-degrees, but gain a topologically sorted array.
*/
void tsort(struct graph* g) {
  /*
  Remember that its impossible for every vertex to have no in-degree 
  (that's an edgeless graph and should not be called onto this)
  Thus at most v - 1 can have no indegree (2-depth tree with n children)
  */
  int s[g->v - 1];
  size_t s_size = 0;
  memset((void*)s, 0, (g->v - 1) * sizeof(int));
  int i;
  // Find nodes with no incoming edges and store into S
  for (i = 0; i < g->v; i++) {
    if (g->alist[i]->id == 0) {
      s[s_size] = i;
      s_size++;
    }
  }
  i = 0; // ordering list counter
  int j;
  int curr_u;
  while (s_size > 0) {
    for (int i = 0; i < g->v - 1; i++) printf("%d ", s[i]);
    s_size--;
    curr_u = s[s_size]; 
    g->ordering[i] = curr_u; // remove node n from stack S & add node n to list L
    i++;
    for (j = 0; j < g->alist[curr_u]->od; j++) { // for each node from n to m
      int curr_v = g->alist[curr_u]->list[j]; // current m node number
      if (g->alist[curr_v]->id == 1) { // if removing this edge removes it from graph
        s[s_size] = curr_v; // add node back to S
        s_size++;
      }
      g->alist[curr_v]->id--;
    }
  }
  /*
  Theoretically, no ids should exist at this point?
  */
}

/* invoke f on all edges (u,v) with source u */
/* supplying data as final parameter to f */
/* no particular order is guaranteed */
void graph_foreach(struct graph* self, int source,
        void (*f)(struct graph* g, int source, int sink, void *data),
        void *data) {
  int i;
  for (i = 0; i < self->alist[source]->od; i++) {
    f(self, source, self->alist[source]->list[i], data);
  }
}

#endif
