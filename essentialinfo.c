#include <stdio.h>
#include "essentialinfo.h"

essentialinfo* init_essentialinfo(pattern* P, uint32_t graph_order) {
  essentialinfo* info = malloc(sizeof(essentialinfo));
  int sz = P->order;
  info->size = sz;
  info->graph_size = graph_order;

  info->apex = malloc(sz*sizeof(vertex));
  memset(info->apex, 0xffffffff, sz*sizeof(vertex));

  info->coapex = malloc(sz*sizeof(vertex));
  memset(info->coapex, 0xffffffff, sz*sizeof(vertex));

  info->active[0] = calloc(sz, sizeof(bool));
  info->active[1] = calloc(sz, sizeof(bool));

  int i,j;
  for(i = 0; i < 4; i++) {
    info->typical[i] = malloc(sz*sizeof(stss));
    for(j = 0; j < sz; j++)
      info->typical[i][j].type = EMPTY;
  }

  info->aspects = calloc(graph_order*sz, sizeof(bool));
  return(info);
}

bool has_aspect(essentialinfo* info, vertex gv, vertex pv) {
  return(info->aspects[gv*(info->size) + pv]);
}

void add_aspect(essentialinfo* info, vertex gv, vertex pv) {
  info->aspects[gv*(info->size) + pv] = true;
}

void delete_aspect(essentialinfo* info, vertex gv, vertex pv) {
  info->aspects[gv*(info->size) + pv] = false;
}

void delete_all_aspects_graph_vertex(essentialinfo* info,
    vertex gv) {
  assert(gv != UNDEFINED);
  vertex pv;
  for(pv = 0; pv < info->size; pv++) {
    info->aspects[gv*(info->size) + pv] = false;
  }
}

void delete_essentialinfo(essentialinfo* info) {
  free(info->apex);
  free(info->coapex);

  free(info->active[0]);
  free(info->active[1]);

  free(info->typical[0]);
  free(info->typical[1]);
  free(info->typical[2]);
  free(info->typical[3]);

  free(info->aspects);

  free(info);
}

void print_essentialinfo(essentialinfo* info) {
  int i,j;

  for(i = 0; i < info->size; i++) {
    printf("  %d: apex = %d ", i, info->apex[i]);
    if(info->active[APEX][i]) printf("(active)");
    printf(", coapex = %d ", info->coapex[i]);
    if(info->active[COAPEX][i]) printf("(active)");
    printf("\n");

    for(j = 0; j < 4; j++) {
      if(info->typical[j][i].type != EMPTY) {
        printf("  %d:   ",i);
        switch(info->typical[j][i].type) {
          case BALL: printf("ball"); break;
          case CYCLE: printf("cycle"); break;
          case PATH: printf("path");
        } 
        printf(" = {%d,%d,%d,%d}\n",info->typical[j][i].set[0],
          info->typical[j][i].set[1],
          info->typical[j][i].set[2],
          info->typical[j][i].set[3]); 
      }
    }
  }

  printf("  aspects: "); 
  for(i = 0; i < info->graph_size; i++) {
    for(j = 0; j < info->size; j++) {
      if(has_aspect(info,i,j)) {
        printf("(%d,%d), ", i,j);
      }
    }
  }
  printf("\n");
} 

uint32_t num_stss(essentialinfo* info, vertex x) {
  int i, count = 0;
  for(i = 0; i < 4; i++) {
    if(get_stss(info,x,i).type != EMPTY) count++;
  }
  return(count);
}

stss get_stss(essentialinfo* info, vertex x, uint32_t num) {
  assert(num < 4);
  return(info->typical[num][x]);
}

stss get_ball_stss(essentialinfo* info, vertex x, uint32_t ball_num) {
  assert(ball_num < 2);
  return(get_stss(info,x,ball_num));
}

stss get_cycle_stss(essentialinfo* info, vertex x) {
  return(get_stss(info,x,2));
}

stss get_path_stss(essentialinfo* info, vertex x) {
  return(get_stss(info,x,3));
}

void set_stss(essentialinfo* info, vertex x, uint32_t num, stss M) {
  assert(M.type == EMPTY || M.type == BALL || (num != 0 && num != 1));
  assert(M.type == EMPTY || M.type == CYCLE || (num != 2));
  assert(M.type == EMPTY || M.type == PATH || (num != 3));

  stss* newM = &(info->typical[num][x]);
  newM->type = M.type;

  int i;
  for(i = 0; i < 4; i++) {
    newM->set[i] = M.set[i];
  }
}

vertex apex(essentialinfo* info, vertex x) {
  return(info->apex[x]);
}

vertex coapex(essentialinfo* info, vertex x) {
  return(info->coapex[x]);
}

void set_apex(essentialinfo* info, vertex x, vertex apex) {
  info->apex[x] = apex;
}

void set_coapex(essentialinfo* info, vertex x, vertex coapex) {
  info->coapex[x] = coapex;
}

void activate(essentialinfo* info, vertex x, int p) {
  info->active[p][x] = true;
}

void deactivate(essentialinfo* info, vertex x, int p) {
  assert(x != UNDEFINED);
  info->active[p][x] = false;
}

bool is_active(essentialinfo* info, vertex x, int p) {
  return(info->active[p][x]);
}

void special_find4cycle(stss M, mygraph* G, vertex* set) {
  vertex i,j,w;
  set[0] = M.set[0];
  for(i = 1; i < 4; i++) {
    for(j = i+1; j < 4; j++) {
      if(M.set[i] != UNDEFINED && M.set[j] != UNDEFINED) {
        for(w = 0; w < G->order; w++) {
	  if(w != M.set[0] && has_edge(G,M.set[i],w) && has_edge(G,M.set[j],w)) {
	    set[1] = M.set[i];
	    set[2] = w;
	    set[3] = M.set[j];
	    return;
	  }
	}
      }
    }
  }

  assert(false);
}
