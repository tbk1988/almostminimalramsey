#ifndef GRAPH_H
#define GRAPH_H

#include <stdlib.h> 
#include <string.h>
#include <stdint.h>
#include <assert.h> 
#include <stdbool.h>

#define UNDEFINED 0xffffffff

typedef uint32_t vertex;

typedef struct {
  uint32_t order;
  uint32_t current_order;
  bool* adj;
} mygraph;

uint32_t e(mygraph* G);
uint32_t n(mygraph* G);

mygraph* init_graph(uint32_t order);
void delete_graph(mygraph* G);

bool has_edge(mygraph* G, vertex v, vertex w);
void add_edge(mygraph* G, vertex v, vertex w);
int delete_edge(mygraph* G, vertex v, vertex w);
void to_string(mygraph* G, char* str, uint32_t max_string_size);
void read_graph(mygraph* G, char* str);
uint32_t deg(mygraph* G, vertex v);
uint32_t back_deg(mygraph* G, vertex c, vertex before);

uint32_t current_order(mygraph* G);
void add_vertices(mygraph* G, uint32_t num);

void print_graph(mygraph* G);

void find_1ball(mygraph* G, vertex center, vertex* set, uint32_t max_set_size);
void find_4cycle(mygraph* G, vertex c1, vertex c2, vertex* set);
void find_4path(mygraph* G, vertex ap, vertex co, vertex avoid1, vertex avoid2,
    vertex* set); 

#endif
