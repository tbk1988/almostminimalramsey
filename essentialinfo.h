#ifndef ESSENTIALINFO_H
#define ESSENTIALINFO_H

#include "pattern.h"

#define BALL 0
#define CYCLE 1
#define PATH 2
#define EMPTY 3

#define APEX 0
#define COAPEX 1
#define CX 2
#define PX 3

typedef struct {
  char type;
  vertex set[4];
} stss;

typedef struct {
  uint32_t size;
  uint32_t graph_size;
  vertex* apex;
  vertex* coapex;
  bool* active[2];
  stss* typical[4];
  bool* aspects;
} essentialinfo;

bool has_aspect(essentialinfo* info, vertex gv, vertex pv);
void add_aspect(essentialinfo* info, vertex gv, vertex pv);
void delete_aspect(essentialinfo* info, vertex gv, vertex pv);
void delete_all_aspects_graph_vertex(essentialinfo* info, vertex gv);
void delete_all_aspects_pattern_vertex(essentialinfo* info, vertex pv); 

essentialinfo* init_essentialinfo(pattern* P, uint32_t graph_order);
void delete_essentialinfo(essentialinfo* info);
void print_essentialinfo(essentialinfo* info); 

uint32_t num_stss(essentialinfo* info, vertex x);
stss get_stss(essentialinfo* info, vertex x, uint32_t num);
stss get_ball_stss(essentialinfo* info, vertex x, uint32_t ball_num);
stss get_cycle_stss(essentialinfo* info, vertex x);
stss get_path_stss(essentialinfo* info, vertex x);
void set_stss(essentialinfo* info, vertex x, uint32_t num, stss M);

vertex apex(essentialinfo* info,vertex x);
vertex coapex(essentialinfo* info,vertex x);

void set_apex(essentialinfo* info, vertex x, vertex apex);
void set_coapex(essentialinfo* info, vertex x, vertex coapex);

void activate(essentialinfo* info, vertex x, int p);
void deactivate(essentialinfo* info, vertex x, int p);
bool is_active(essentialinfo* info, vertex x, int p);

void special_find4cycle(stss M, mygraph* G, vertex* set);

#endif
