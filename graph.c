#include "graph.h" 
#include <stdio.h>

bool adj_is_symmetric(mygraph* G) {
  vertex v,w;
  for(v = 0; v < G->order; v++) {
    for(w = 0; w < G->order; w++) {
      if(has_edge(G,v,w) != has_edge(G,w,v))
        return(false);
    }
  }
  return(true);
}

uint32_t e(mygraph* G) {
  uint32_t n = G->order;
  vertex v,w;
  uint32_t ecount = 0;
  for(v = 0; v < n; v++) {
    for(w = v+1; w < n; w++) {
      if(has_edge(G,v,w))
        ecount++;
    }
  }
  return(ecount);
}

uint32_t n(mygraph* G) {
  return(G->order);
}

mygraph* init_graph(uint32_t order) {
  mygraph* G = malloc(sizeof(mygraph));
  G->order = order;
  G->current_order = 0;
  G->adj = calloc(order*order,sizeof(bool));
  return(G);
}

void delete_graph(mygraph* G) { 
  free(G->adj);
  free(G);
}

bool has_edge(mygraph* G, vertex v, vertex w) {
  return(G->adj[v*(G->order) + w]);
}

void add_edge(mygraph* G, vertex v, vertex w) {
  G->adj[v*(G->order) + w] = true;
  G->adj[w*(G->order) + v] = true;
}

int delete_edge(mygraph* G, vertex v, vertex w) {
  if(!has_edge(G,v,w)) {
    return(-1);
  } else {
    G->adj[v*(G->order) + w] = false;
    G->adj[w*(G->order) + v] = false;
  }
  return(0);
}

void to_string(mygraph* G, char* str, uint32_t max_string_size) {
  assert((G->order)*(G->order) < max_string_size);
  
  uint32_t i,j;
  uint32_t k = 0;
//  uint32_t sz = (G->current_order)*(G->current_order);
  for(i = 0; i < G->current_order; i++) {
    for(j = 0; j < G->current_order; j++) {
      str[k] = '0' + (has_edge(G,i,j) ? 1 : 0);
      k++;
    }
  } 
  str[k] = '\0';
}

void read_graph(mygraph* G, char* str) {
  assert((G->order)*(G->order) == strlen(str));

  uint32_t i;
  uint32_t len = strlen(str);
  for(i = 0; i < len; i++) {
    assert(str[i] == '0' || str[i] == '1');
    G->adj[i] = (str[i] == '0' ? false : true);
  }

  assert(adj_is_symmetric(G));
}

uint32_t deg(mygraph* G, vertex v) {
  uint32_t deg_count = 0;
  vertex w;
  for(w = 0; w < G->order; w++) {
    if(has_edge(G,v,w)) deg_count++;
  }
  return(deg_count);
}

uint32_t back_deg(mygraph* G, vertex v, vertex before) {
  uint32_t deg_count = 0;
  vertex w;
  for(w = 0; w < before; w++) {
    if(has_edge(G,v,w)) deg_count++;
  }
  return(deg_count);
} 

uint32_t current_order(mygraph* G) {
  return(G->current_order); 
}

void add_vertices(mygraph* G, uint32_t num) {
  G->current_order += num;
}

void print_graph(mygraph* G) {
  vertex v,w;
  for(v = 0; v < G->order; v++) {
    printf("  %d: ", v);
    for(w = 0; w < G->order; w++) {
      if(has_edge(G,v,w))
        printf("%d, ", w);
    }
    printf("\n");
  }
}

void find_1ball(mygraph* G, vertex center, vertex* set, uint32_t max_set_size) {
  assert(max_set_size >= 1);
  set[0] = center;
  uint32_t set_counter = 1;
  vertex w; 
  for(w = 0; w < G->order; w++) {
    if(has_edge(G,center,w)) {
      assert(set_counter < max_set_size);
      set[set_counter] = w;
      set_counter++;
      // TODO: Fix Ugly hack
      if(set_counter == max_set_size) return;
    }
  }
  while(set_counter < max_set_size) {
    set[set_counter] = UNDEFINED;
    set_counter++;
  }
}

void find_4cycle(mygraph* G, vertex c1, vertex c2, vertex* set) { 
  set[0] = c1;
  if(c2 != UNDEFINED) {
    set[1] = c2; 
    assert(has_edge(G,c1,c2));
    vertex w, v;
    for(w = 0; w < G->order; w++) {
      for(v = 0; v < G->order; v++) {
        if(w != c1 && v != c2 &&
           has_edge(G,c2,w) &&
           has_edge(G,c1,v) &&
           has_edge(G,v,w)) {
          set[2] = w;
          set[3] = v;
#ifndef NDEBUG
  // There should be a _unique_ four cycle
  // This assertion seems to fail in many non-tree cases, could it be that it
  // is only true in the tree-case?
  //
  // What is an alternative way to define this cycle (which does not require
  // it to be a unique one)?
  vertex x,y;
  for(x = 0; x < G->order; x++) {
    for(y = 0; y < G->order; y++) {
      if(x != c1 && y != c2 &&
         has_edge(G,c2,x) &&
	 has_edge(G,c1,y) &&
	 has_edge(G,x,y) &&
	 !((x == w && y == v) || (x == v && y == w))) {
	assert(false);
      }
    }
  }
#endif
          return;
        }
      }
    }
  } else {
    for(c2 = 0; c2 < G->order; c2++) {
      if(has_edge(G,c1,c2)) {
        set[1] = c2; 
        vertex w, v;
        for(w = 0; w < G->order; w++) {
          for(v = 0; v < G->order; v++) {
            if(w != c1 && v != c2 &&
               has_edge(G,c2,w) &&
               has_edge(G,c1,v) &&
               has_edge(G,v,w)) {
              set[2] = w;
              set[3] = v;
              return;
            }
          }
        }
      }
    }
  }

#ifndef NDEBUG
  // There should be a _unique_ cycle containing 
  
#endif

  // should not reach this
  assert(false);
}

void find_4path(mygraph* G, vertex ap, vertex co, vertex avoid1, vertex avoid2,
    vertex* set) {
  assert(has_edge(G,ap,co));
  set[0] = UNDEFINED;
  set[1] = ap;
  set[2] = co;
  set[3] = UNDEFINED;
  
  vertex w;
  for(w = 0; w < G->order; w++) {
    if(has_edge(G,ap,w)) {
      if(w != avoid1 && w != avoid2 && w != co) {
        set[0] = w;
      }
    }
    if(has_edge(G,co,w)) {
      if(w != avoid1 && w != avoid2 && w != ap) {
        set[3] = w;
      }
    }
  }

  assert(set[0] != UNDEFINED);
  assert(set[3] != UNDEFINED);
}
