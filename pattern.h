#ifndef PATTERN_H
#define PATTERN_H

#include "graph.h" 

typedef struct {
  uint32_t order;
  bool* adj;
} pattern;

uint32_t pattern_e(pattern* P);
uint32_t pattern_n(pattern* P);

pattern* init_pattern(uint32_t order);
void delete_pattern(pattern* P);

bool pattern_has_edge(pattern* P, vertex v, vertex w);
bool pattern_has_diedge(pattern* P, vertex v, vertex w);
bool pattern_has_adiedge(pattern* P, vertex v, vertex w);
void pattern_add_edge(pattern* P, vertex v, vertex w);
int pattern_delete_edge(pattern* P, vertex v, vertex w);
void pattern_to_string(pattern* P, char* str, uint32_t max_string_size);
void read_pattern(pattern* P, char* str);
uint32_t pattern_deg(pattern* P, vertex v);
uint32_t pattern_back_deg(pattern* P, vertex v, vertex before);
uint32_t pattern_outdeg(pattern* P, vertex v);
uint32_t pattern_indeg(pattern* P, vertex v);

bool is_symm(pattern* P, vertex v);

vertex points_to(pattern* P, vertex v);

void set_joins(pattern* P, vertex x, vertex* joins); 

#endif
