/*
 * Main file in the proper implementation, reads the patterns from
 * stdin and writes the graphs obtained from them to stdout.
 *
 * Passing flag -v makes the program print a lot of auxiliary stuff.
 *              -n makes the program print line numbers to stderr.
 */

#include "naututil.h"
#include <stdio.h>
#include <stdbool.h>
#include "graph.h"
#include "pattern.h"
#include "essentialinfo.h"
#include "stitching.h" 

#define MAXSIZE 200

static int VERBOSE = 0;
static int STDERRNUM = 0;

/* isqrt(n) computes the integer square root of n. */
int isqrt(int n) {
  int a = 1;
  int b = n;
  while((a < b ? b - a : a-b) > 1) {
    b = n/a;
    a = (a + b)/2;
  }

  return(a);
}

/* number_of_monovalents(P) counts the number of monovalent vertices in pattern
 * P */
uint32_t number_of_monovalents(pattern* P) {
  uint32_t count = 0;
  uint32_t v;
  for(v = 0; v < P->order; v++) {
    if(pattern_deg(P,v) == 1) {
      count++;
     }
  }
  return(count);
}

/* graph_size(P) computes the number of vertices that will end up in the
 * patterned graph of the pattern P */
uint32_t graph_size(pattern* P) {
  return(2*pattern_n(P) + pattern_e(P) + 11*number_of_monovalents(P));
}

/* h13_explode_monovalents(G,P,info) modifies the an ordinary patterned graph G
 * with pattern P (and essensial info) into a H_{13}-decorated patterned graph 
 * by turning loopy monovalent vertices into 
 * H_{13}-decoration in the graph */
void h13_explode_monovalents(mygraph* G, pattern* P, essentialinfo* info,bool* loopy) {
  vertex v,v0,v1,v2,v3,v4,v5,v6,v7,v8,v9,v10,v11,v12;
  vertex w;
//  assert(current_order(G) + 11*number_of_monovalents(P) == n(G));
  for(v = 0; v < pattern_n(P); v++) { 
    if(pattern_deg(P,v) == 1 && loopy[v]) { 
      // Found loopy monovalent vertex in the pattern
      v0 = coapex(info,v);
      assert(v0 != UNDEFINED);
      v1 = apex(info,v);
      assert(v1 != UNDEFINED); 
      v2 = current_order(G); 
      v3 = v2+1;
      v4 = v2+2; 
      v5 = v2+3; 
      v6 = v2+4; 
      v7 = v2+5; 
      v8 = v2+6; 
      v9 = v2+7; 
      v10 = v2+8; 
      v11 = v2+9; 
      v12 = v2+10; 

      // Make H13 to attatch to
      add_edge(G,v0,v1);
      add_edge(G,v0,v2);
      add_edge(G,v0,v3);
      add_edge(G,v0,v4);
      add_edge(G,v1,v5);
      add_edge(G,v1,v9);
      add_edge(G,v1,v10);
      add_edge(G,v2,v7);
      add_edge(G,v2,v10);
      add_edge(G,v2,v12);
      add_edge(G,v3,v8);
      add_edge(G,v3,v11);
      add_edge(G,v3,v12);
      add_edge(G,v4,v6);
      add_edge(G,v4,v9);
      add_edge(G,v4,v11);
      add_edge(G,v5,v6);
      add_edge(G,v5,v7);
      add_edge(G,v5,v11);
      add_edge(G,v6,v8);
      add_edge(G,v6,v12);
      add_edge(G,v7,v8);
      add_edge(G,v7,v9);
      add_edge(G,v8,v10);
      add_edge(G,v9,v12);
      add_edge(G,v10,v11); 

      if(pattern_outdeg(P,v) == 1) {
        // Make K23- connection.
        // Twice clone apex
        for(w = 0; w < current_order(G); w++) {
          if(v1 != w && has_edge(G,v1,w)) { 
            add_edge(G,v2,w);
            add_edge(G,v4,w); 
          }
          if(v0 != w && has_edge(G,v0,w)) {
            add_edge(G,v10,w);
          }
        }
      } else { 
        for(w = 0; w < current_order(G); w++) {
          if(v1 != w && has_edge(G,v1,w)) { 
            add_edge(G,v2,w);
            add_edge(G,v3,w); 
            add_edge(G,v4,w);
          } 
        } 
      }
      add_vertices(G,11);
    }
  } 
}

int main(int argc, char* argv[]) {
  vertex x;

  essentialinfo* info;
  mygraph* G;
  pattern* P;
  int i;

  bool* loopy;

  if(argc > 1) {
    if(!strcmp(argv[1],"-v")) {
      VERBOSE = 1;
    } else if(!strcmp(argv[1],"-n")) {
      STDERRNUM = 1;
    } else {
      printf("unrecognized argument: %s\n", argv[1]);
      return(1);
    }
  }

  uint32_t num_row = 0;
  int independence;  /* saving independence number of current graph */
  int j,m;

  /* NAUTY-setting up graph */
  DYNALLSTAT(int,lab,lab_sz); 
  DYNALLSTAT(int,ptn,ptn_sz);
  DYNALLSTAT(int,orbits,orbits_sz);
  DYNALLSTAT(int,map,map_sz);
  DYNALLSTAT(graph,g,g_sz); 
  DYNALLSTAT(graph,cg,cg_sz); 
  static DEFAULTOPTIONS_GRAPH(options); 
  statsblk stats; 
  options.getcanon = TRUE;

  char* line = calloc(MAXSIZE*MAXSIZE + 1, sizeof(char));
  while(scanf("%d %s\n",&independence,line) != EOF) {
    num_row++;
    if(STDERRNUM)
      fprintf(stderr, "%d ;\n",num_row);
    if(VERBOSE)
      printf("- - - NEW PATTERN %s - - - \n\n", line);

    int line_length = strlen(line);
    int pattern_size = isqrt(line_length); 

    assert(pattern_size*pattern_size == line_length);
    assert(line_length > 0);

    P = init_pattern(pattern_size);
    loopy = calloc(pattern_size, sizeof(bool));

    /* Prehandle loopy edges */
    for(i = 0; i < pattern_size; i++) {
      loopy[i] = false;
      if(line[i*pattern_size + i] == '1') {
        line[i*pattern_size + i] = '0';
        loopy[i] = true;
      }
    }
    read_pattern(P,line);

    int n = graph_size(P);
    assert(n <= MAXSIZE);
    G = init_graph(n);
    info = init_essentialinfo(P,n);

    vertex joins[3];
    for(x = 0; x < pattern_size; x++) {
      set_joins(P, x, joins);
      if(joins[0] == UNDEFINED) { /* back_deg == 0 ! */
        upd1stitch(G,P,info, x);
        add_vertices(G,2);
      } else if(joins[1] == UNDEFINED) { /* back_deg == 1 */
        upd2stitch(G,P,info, x, joins[0]);
        add_vertices(G,3);
      } else if(joins[2] == UNDEFINED) { /* back_deg == 2 */
        upd3stitch(G,P,info, x, joins[0], joins[1]);
        add_vertices(G,4);
      } else { /* back_deg == 3 */
        upd4stitch(G,P,info, x, joins[0], joins[1], joins[2]);
        add_vertices(G,5);
      }

      if(VERBOSE) {
        printf("> Essential info:\n");
        print_essentialinfo(info);
        printf("> Graph:\n");
        print_graph(G);
      } 
    }

    h13_explode_monovalents(G,P,info,loopy);

    /* Set up a nauty version of finished graph */
    n = G->current_order;
    m = SETWORDSNEEDED(n);
    nauty_check(WORDSIZE,m,n,NAUTYVERSIONID);
    DYNALLOC2(graph,g,g_sz,m,n,"malloc");
    DYNALLOC2(graph,cg,cg_sz,m,n,"malloc");
    DYNALLOC1(int,lab,lab_sz,n,"malloc");
    DYNALLOC1(int,ptn,ptn_sz,n,"malloc");
    DYNALLOC1(int,orbits,orbits_sz,n,"malloc"); 
    DYNALLOC1(int,map,map_sz,n,"malloc");
    EMPTYGRAPH(g,m,n);
    EMPTYGRAPH(cg,m,n);
    for(i = 0; i < n; i++) {
      for(j = 0; j < n; j++) {
        if(has_edge(G,i,j)) ADDELEMENT(GRAPHROW(g,i,m),j);
      }
    }
    densenauty(g,lab,ptn,orbits,&options,&stats,m,n,cg);

    printf("%d ",independence);
    writeg6(stdout,cg,m,n); 

    free(loopy);
    delete_essentialinfo(info);
    delete_graph(G);
    delete_pattern(P);
  } 
  free(line);

  return(0);
}
