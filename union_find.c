//
// Tomás Oliveira e Silva, AED, December 2015
//
// union-find
//

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
// custom graph implementation (with union-find infrastructure)
//

typedef struct edge
{
  struct edge *next;
  int vertex_number;
  int weight;
}
edge;

edge *free_edges = NULL;

typedef struct vertex
{
  struct edge *out_edges; // adjacency list head
  int mark;               // for graph traversals
  int representative;     // representative of each connected component
}
vertex;

typedef struct graph
{
  struct vertex *vertices;    // array of vertices (pointer)
  int n_vertices;             // number of vertices
  int n_connected_components; // number of connected components
}
graph;

edge *create_edge(void)
{
  edge *e;
  int i;

  if(free_edges == NULL)
  {
    free_edges = (edge *)malloc((size_t)1000 * sizeof(edge));
    assert(free_edges != NULL);
    for(i = 0;i < 999;i++)
      free_edges[i].next = &free_edges[i + 1];
    free_edges[i].next = NULL;
  }
  e = free_edges;
  free_edges = free_edges->next;
  return e;
}

graph *create_graph(int n_vertices)
{
  graph *g;
  int i;

  assert(n_vertices >= 1 && n_vertices <= 1000000);
  g = (graph *)malloc(sizeof(graph));
  assert(g != NULL);
  g->vertices = (vertex *)malloc((size_t)n_vertices * sizeof(vertex));
  assert(g->vertices != NULL);
  g->n_vertices = n_vertices;
  g->n_connected_components = n_vertices;
  for(i = 0;i < n_vertices;i++)
  {
    g->vertices[i].out_edges = NULL;
    g->vertices[i].representative = i;
  }
  return g;
}

void destroy_graph(graph *g)
{
  edge *e;
  int i;

  assert(g != NULL && g->vertices != NULL);
  for(i = 0;i < g->n_vertices;i++)
    if(g->vertices[i].out_edges != NULL)
    {
      for(e = g->vertices[i].out_edges;e->next != NULL;e = e->next)
        ;
       e->next = free_edges;
       free_edges = e;
    }
  free(g->vertices);
  free(g);
}

int find_representative(graph *g,int vertex_number)
{
  int i,j,k;

  // find
  for(i = vertex_number;i != g->vertices[i].representative;i = g->vertices[i].representative)
    ;
  // path compression
  for(j = vertex_number;j != i;j = k)
  {
    k = g->vertices[j].representative;
    g->vertices[j].representative = i;
  }
  return i;
}

int add_edge(graph *g,int from,int to,int weight)
{
  int fi,ti;
  edge *e;

  assert(from >= 0 && from < g->n_vertices && to >= 0 && to < g->n_vertices && from != to);
  for(e = g->vertices[from].out_edges;e != NULL && e->vertex_number != to;e = e->next)
    ;
  if(e != NULL)
    return 0;
  e = create_edge();
  e->next = g->vertices[from].out_edges;
  g->vertices[from].out_edges = e;
  e->vertex_number = to;
  e->weight = weight;
  fi = find_representative(g,from);
  ti = find_representative(g,to);
  if(fi != ti)
  { // union
    g->vertices[ti].representative = fi;
    g->n_connected_components--;
  }
  return 1;
}

graph *init_graph(char *s,int undirected)
{
  int from,to,weight;
  graph * g;
  char *ss;
  long l;
  int i;

  l = strtol(s,&ss,10);
  assert(ss != s && l >= 1l && l <= 1000000l);
  s = ss;
  g = create_graph((int)l);
  for(;;)
  {
    while(*s == ' ' || *s == '\n' || *s == '\t' || *s == '\r')
      s++;
    if(*s == '\0')
      return g;
    l = strtol(s,&ss,10); assert(ss != s); s = ss; from = (int)l - 1;
    l = strtol(s,&ss,10); assert(ss != s); s = ss; to = (int)l - 1;
    l = strtol(s,&ss,10); assert(ss != s); s = ss; weight = (int)l;
    i = add_edge(g,from,to,weight);
    assert(i == 1);
    if(undirected != 0 && from != to)
      add_edge(g,to,from,weight);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// main program
//

int main(void)
{
  graph *g;

  g = init_graph("9 "
                 "1 2 2 1 3 4 1 4 6 "
                 "2 3 1 2 5 1 "
                 "3 5 2 3 6 2 "
                 "4 7 2 4 8 2 "
                 "5 6 1 5 9 4 "
                 "6 7 2 6 9 2 "
                 "7 9 3 "
				 "8 9 1",
                 1);
                 
 /*
 "9 "
 "1 2 2 1 3 4 1 4 6 "
 "2 3 1 2 5 1 "
 "3 5 2 3 6 2 "
 "4 7 2 4 8 2 "
 "5 6 1 5 9 4 "
 "6 7 2 6 9 2 "
 "7 9 3 "
 "8 9 1"
  */
  
  printf("The graph has %d connected component%s\n",g->n_connected_components,(g->n_connected_components == 1) ? "" : "s");
  destroy_graph(g);
  return 0;
}
