#include "pattern.h" 

uint32_t pattern_e(pattern* P) {
  uint32_t n = P->order;
  vertex v,w;
  uint32_t ecount = 0;
  for(v = 0; v < n; v++) {
    for(w = v+1; w < n; w++) {
      if(pattern_has_adiedge(P,v,w))
        ecount++;
    }
  }
  return(ecount);
}

uint32_t pattern_n(pattern* P) {
  return(P->order);
}

pattern* init_pattern(uint32_t order) {
  pattern* P = malloc(sizeof(pattern));
  P->order = order;
  P->adj = calloc(order*order, sizeof(char));
  return(P);
}

void delete_pattern(pattern* P) {
  free(P->adj);
}

bool pattern_has_edge(pattern* P, vertex v, vertex w) {
  return(P->adj[v*(P->order) + w]);
}

bool pattern_has_diedge(pattern* P, vertex v, vertex w) {
  return(pattern_has_edge(P,v,w) && !pattern_has_edge(P,w,v));
}

bool pattern_has_adiedge(pattern* P, vertex v, vertex w) {
  return(pattern_has_edge(P,v,w) || pattern_has_edge(P,w,v));
}

void pattern_add_edge(pattern* P, vertex v, vertex w) {
  P->adj[v*(P->order) + w] = true;
}

int pattern_delete_edge(pattern* P, vertex v, vertex w) {
  if(!pattern_has_edge(P,v,w)) {
    return(-1);
  } else {
    P->adj[v*(P->order) + w] = false;
  }
  return(0);
}

void pattern_to_string(pattern* P, char* str, uint32_t max_string_size) {
  assert((P->order)*(P->order) < max_string_size);

  uint32_t i;
  uint32_t sz = (P->order)*(P->order);
  for(i = 0; i < sz; i++) {
    str[i] = '0' + (P->adj[i] ? 1 : 0);
  }
  str[sz] = '\0';
}

void read_pattern(pattern* P, char* str) {
  assert((P->order)*(P->order) == strlen(str));

  uint32_t i;
  uint32_t len = strlen(str);
  for(i = 0; i < len; i++) {
    assert(str[i] == '0' || str[i] == '1');
    P->adj[i] = (str[i] == '0' ? false : true);
  }
}

uint32_t pattern_deg(pattern* P, vertex v) {
  uint32_t deg_count = 0;
  vertex w;
  for(w = 0; w < P->order; w++) {
    if(pattern_has_adiedge(P,v,w)) deg_count++;
  }
  return(deg_count);
}

uint32_t pattern_back_deg(pattern* P, vertex v, vertex before) {
  uint32_t deg_count = 0;
  vertex w;
  for(w = 0; w < before; w++) {
    if(pattern_has_adiedge(P,v,w)) deg_count++;
  }
  return(deg_count);
}

uint32_t pattern_outdeg(pattern* P, vertex v) {
  uint32_t outdeg_count = 0;
  vertex w;
  for(w = 0; w < P->order; w++) {
    if(pattern_has_edge(P,v,w) && !pattern_has_edge(P,w,v)) outdeg_count++;
  }
  return(outdeg_count);
}

uint32_t pattern_indeg(pattern* P, vertex v) {
  uint32_t indeg_count = 0;
  vertex w;
  for(w = 0; w < P->order; w++) {
    if(!pattern_has_edge(P,v,w) && pattern_has_edge(P,w,v)) indeg_count++;
  }
  return(indeg_count);
}

vertex points_to(pattern* P, vertex v) {
  assert(pattern_outdeg(P,v) == 1);
  vertex w;
  for(w = 0; w < P->order; w++) {
    if(pattern_has_edge(P,v,w) && !pattern_has_edge(P,w,v)) return(w);
  }
  return(UNDEFINED);
}

void set_joins(pattern* P, vertex x, vertex* joins) {
  vertex y;
  joins[0] = UNDEFINED; joins[1] = UNDEFINED; joins[2] = UNDEFINED;
  int join_count = 0;
  for(y = 0; y < x; y++) {
    if(pattern_has_adiedge(P,x,y)) {
      joins[join_count] = y;
      join_count++;
    }
  }
}

bool is_symm(pattern* P, vertex v) {
  assert(v != UNDEFINED);
  assert(v < P->order);

  return(pattern_outdeg(P,v) == 3);
}
