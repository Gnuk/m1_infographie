#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "tiny_gc.h"
#define VERTEX void
#include "triangulation.h"

edge_list cons_edge_list(half_edge e, edge_list l) {
  edge_list n = (edge_list) GC_malloc(sizeof(struct edge_list_cell));
  n->head = e;
  n->tail = l;
  return n;
}

/* Allocation d'une demi-arête                                       *
 */

half_edge alloc_half_edge(void* v) {
  half_edge e = (half_edge) GC_malloc(sizeof(struct half_edge_cell));
  e->vertex = v;
  e->opp = NULL;
  e->next = NULL;
  e->prev = NULL;
  e->visited = 0;
  return e;
}

/* Fonction qui crée une arête (donc deux demi-arêtes).             *
 */

half_edge create_edge(void *v1, void *v2) {
  half_edge e1 = alloc_half_edge(v1);
  half_edge e2 = alloc_half_edge(v2);

  e1 -> opp = e2;
  e2 -> opp = e1;

  return e1;
}

/* Creation d'un nouveau triangle.                                    *
 * Attention, l'ordre des sommets doit respecté l'orientation du      *
 * triangle.                                                          *
 */

half_edge create_triangle(void* v1, void* v2, void* v3) {
  
  half_edge e1 = create_edge(v1,v2);
  half_edge e2 = create_edge(v2,v3);
  half_edge e3 = create_edge(v3,v1);

  half_edge ne1 = e3->opp;
  half_edge ne2 = e1->opp;
  half_edge ne3 = e2->opp;

  e1->next = ne1; ne1->prev = e1;
  e2->next = ne2; ne2->prev = e2;
  e3->next = ne3; ne3->prev = e3;
  
  return e1;
}

void add_vertex_to_edge(half_edge e1, void* v3) {
  
  assert(e1->next == NULL);
  assert(e1->opp->prev == NULL);

  half_edge e2 = create_edge(e1->opp->vertex, v3);
  half_edge e3 = create_edge(v3, e1->vertex);

  half_edge ne1 = e3->opp;
  half_edge ne2 = e1->opp;
  half_edge ne3 = e2->opp;

  e1->next = ne1; ne1->prev = e1;
  e2->next = ne2; ne2->prev = e2;
  e3->next = ne3; ne3->prev = e3;
}

void close_triangle(half_edge e1, half_edge e2) {
  
  assert(e1->next == NULL);
  assert(e1->opp->prev == NULL);
  assert(e2->next == NULL);
  assert(e2->opp->prev == NULL);
  assert(e2->vertex == e1 -> opp -> vertex);

  half_edge e3 = create_edge(e2->opp->vertex,e1->vertex);

  half_edge ne1 = e3->opp;
  half_edge ne2 = e1->opp;
  half_edge ne3 = e2->opp;

  e1->next = ne1; ne1->prev = e1;
  e2->next = ne2; ne2->prev = e2;
  e3->next = ne3; ne3->prev = e3;
}

void fill_triangle(half_edge e1, half_edge e2, half_edge e3) {
  
  assert(e1->next == NULL);
  assert(e1->opp->prev == NULL);
  assert(e2->next == NULL);
  assert(e2->opp->prev == NULL);
  assert(e3->next == NULL);
  assert(e3->opp->prev == NULL);
  assert(e2->vertex == e1 -> opp -> vertex);
  assert(e3->vertex == e2 -> opp -> vertex);
  assert(e1->vertex == e3 -> opp -> vertex);

  half_edge ne1 = e3->opp;
  half_edge ne2 = e1->opp;
  half_edge ne3 = e2->opp;

  e1->next = ne1; ne1->prev = e1;
  e2->next = ne2; ne2->prev = e2;
  e3->next = ne3; ne3->prev = e3;
}

void remove_triangle(half_edge e1) {
  half_edge ne1 = e1 -> next;
  half_edge e3 = ne1 -> opp;
  half_edge ne3 = e3 -> next;
  half_edge e2 = ne3 -> opp;
  half_edge ne2 = e2 -> next;

  e1->next = NULL; ne1->prev = NULL;
  e2->next = NULL; ne2->prev = NULL;
  e3->next = NULL; ne3->prev = NULL;
}
unsigned int current_visit = 0;

void iter_triangles_loop(half_edge e1, void fn(half_edge, void* , void* , void* )) {
  if (e1 && e1->visited != current_visit && e1 -> next) {
    void* v1 = e1 -> vertex;
    half_edge e3 = e1 -> next -> opp;
    void* v3 = e3 -> vertex;
    half_edge e2 = e3 -> next -> opp;
    void* v2 = e2 -> vertex;
    e1 -> visited = current_visit;
    e2 -> visited = current_visit;
    e3 -> visited = current_visit;
    fn(e1,v1,v2,v3);
    iter_triangles_loop(e1->opp,fn);
    iter_triangles_loop(e2->opp,fn);
    iter_triangles_loop(e3->opp,fn);
  }
}

/* Cette fonction applique sont second argument fn 1 fois sur tous    *
 * les triangles accessibles depuis la demi arête e.                  *
 */

void iter_triangles(half_edge e, void fn(half_edge, void* , void* , void* )) {
  current_visit++;
  iter_triangles_loop(e,fn);
}

void iter_comp_triangles(edge_list l, void fn(half_edge, void* , void* , void* )) {
  current_visit++;
  while(l) {
    iter_triangles_loop(l->head,fn);
    l=l->tail;
  }
}

void iter_edges_loop(half_edge e1, void fn(half_edge, void* , void* )) {
  if (e1 && e1->visited != current_visit && e1 -> next) {
    void* v1 = e1 -> vertex;
    half_edge e2 = e1 -> opp;
    void* v2 = e2 -> vertex;
    e1 -> visited = current_visit;
    e2 -> visited = current_visit;
    fn(e1,v1,v2);
    iter_edges_loop(e1->next,fn);
    iter_edges_loop(e2->next,fn);
  }
}

/* Cette fonction applique sont second argument fn 1 fois sur tous    *
 * les triangles accessibles depuis la demi arête e.                  *
 */

void iter_edges(half_edge e, void fn(half_edge, void* , void* )) {
  current_visit++;
  iter_edges_loop(e,fn);
}

void iter_comp_edges(edge_list l, void fn(half_edge, void* , void* )) {
  current_visit++;
  while(l) {
    iter_edges_loop(l->head,fn);
    l=l->tail;
  }
}

void iter_vertices_loop(half_edge e1, void fn(half_edge, void* )) {
  if (e1 && e1->visited != current_visit && e1 -> next) {
    void* v1 = e1 -> vertex;
    half_edge e2 = e1->next;
    while(e2 && e2 -> visited != current_visit) {
      e2 -> visited = current_visit;
      iter_vertices_loop(e2->opp,fn);
      e2 = e2 -> next;
    }
    e2 = e1->prev;
    while(e2 && e2 -> visited != current_visit) {
      e2 -> visited = current_visit;
      iter_vertices_loop(e2->opp,fn);
      e2 = e2 -> prev;
    }
    fn(e1,v1);
    iter_vertices_loop(e1->opp,fn);
  }
}

/* Cette fonction applique sont second argument fn 1 fois sur tous    *
 * les triangles accessibles depuis la demi arête e.                  *
 */

void iter_vertices(half_edge e, void fn(half_edge, void* )) {
  current_visit++;
  iter_vertices_loop(e,fn);
}

void iter_comp_vertices(edge_list l, void fn(half_edge, void* )) {
  current_visit++;
  while(l) {
    iter_vertices_loop(l->head,fn);
    l=l->tail;
  }
}


