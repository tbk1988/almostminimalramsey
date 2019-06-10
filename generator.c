/*
 * Reads triangle-free undirected graphs from stdin in graph6 format
 * and outputs the adjacency matrices of all H_{13}-patterns of such graphs.
 *
 * Assuming that the pattern has less no more than 62 vertices.
 */

#include "naututil.h"
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>
#include <stdint.h>
#define MAXSIZE 200
//#define DEBUGPRINTING

/* Macro for swapping two ints */
#define SWAP(a,b) do { a ^= b; b ^= a; a ^= b; } while(0)

/*
 * Increment the counter.
 * ----------------------
 * dc[i] = 0 unless deg(i) = 1 or 3
 * deg(i) = 3:
 *  dc[i] = 0 means i is symmetric
 *  dc[i] = 1 means i is assym. directed to nbr with lowest label
 *  dc[i] = 2 means i is assym. directed to nbr with snd lowest label
 *  dc[i] = 3 means i is assym. directed to nbr with highest label
 * deg(i) = 1:
 *  dc[i] = 0 means undirected
 *  dc[i] = 1 means looped with edge undirected
 *  dc[i] = 2 means looped with edge out-directed
 *
 * Input:
 *   char* dc; list of current assignmed according to above (size = n)
 *   graph* g; the undirected underlying graph.
 *   int n; the order of the underlying graph.
 *   int m; the maximum size of an adjacency set (for NAUTY).
 *
 * Returns:
 *   TRUE if increment was possible
 *   FALSE otherwise 
 */
int increment_counter(char* dc, graph* g, int n, int m) {
  int i, deg_i;
  int carry = 0;

  for(i = 0; i < n; i++) {
    if(carry == 0) { 
     deg_i = setsize(GRAPHROW(g,i,m), m); 
     if(deg_i == 3) {
        if(dc[i] < 3) {
          dc[i]++;
          return(TRUE);
        } else {
          carry = 1;
          dc[i] = 0;
        }
      } else if(deg_i == 1) {
        if(dc[i] < 2) {
          dc[i]++;
          return(TRUE);
        } else {
          carry = 1;
          dc[i] = 0;
        }
      }
    } else {
      deg_i = setsize(GRAPHROW(g,i,m), m); 
      if((deg_i == 3 && dc[i] < 3) | (deg_i == 1 && dc[i] < 2)) {
        dc[i]++;
        return(TRUE);
      } else if(deg_i == 3 || deg_i == 1) {
        dc[i] = 0;
      }
    }
  }
  return(FALSE);
}

/*
 * Apply the counter to graph.
 * ---------------------------
 *
 * Input: 
 *   char* dg; where to write the adjacency matrix of the directed graph.
 *   char* dc; the direction counter to apply.
 *   graph* graph; the underlying graph.
 *   int n; the order of the graph.
 *   int m; maximum size of an adjacency set.
 *
 * Returns:
 *   TRUE if the counter corresponds to a real pattern.
 *   FALSE if the counter does not correspond to a pattern.
 */
bool apply_counter(char* dg, char* dc, graph* g, int n, int m) {
  int i,j,k;
  set* gr;

  uint32_t* adj = calloc(3*n,sizeof(uint32_t));
  uint32_t* deg = calloc(n,sizeof(uint32_t));
  for(i = 0; i < n; i++) {
    j = 0;
    gr = GRAPHROW(g,i,m);
    for(k = -1; (k = nextelement(gr,m,k)) >= 0;) {
      adj[3*i + j] = k;
      j++;
    }
    deg[i] = j;

    if(deg[i] == 3) {
      if(adj[3*i + 0] > adj[3*i + 1]) SWAP(adj[3*i + 0], adj[3*i + 1]); 
      if(adj[3*i + 1] > adj[3*i + 2]) SWAP(adj[3*i + 1], adj[3*i + 2]); 
      if(adj[3*i + 0] > adj[3*i + 1]) SWAP(adj[3*i + 0], adj[3*i + 1]);
    }
  }

  /* Write all undirected edges */
  for(i = 0; i < n; i++) {
    for(j = 0; j < n; j++) {
      if(ISELEMENT(GRAPHROW(g,i,m),j)) dg[n*i + j] = 1;
      else dg[n*i + j] = 0;
    }
  }

  /* Apply all the directed edges. */
  for(i = 0; i < n; i++) {
    if(deg[i] == 3) {
      if(dc[i] == 0) {
        dg[n*adj[3*i + 0] + i] = 0;
        dg[n*adj[3*i + 1] + i] = 0;
        dg[n*adj[3*i + 2] + i] = 0;
      } else if(dc[i] == 1) {
        dg[n*adj[3*i + 0] + i] = 0; 
      } else if(dc[i] == 2) {
        dg[n*adj[3*i + 1] + i] = 0; 
      } else { 
        dg[n*adj[3*i + 2] + i] = 0;
      }
    } else if(deg[i] == 1 && dc[i] > 0) {
      dg[n*i + i] = 1;
      if(dc[i] == 1) {
        dg[n*adj[3*i + 0] + i] = 0;
      } 
    }
  }

  bool is_pattern = TRUE;
  /* Check that there are no doubly directed edges */
  for(i = 0; i < n; i++) {
    gr = GRAPHROW(g,i,m);
    for(k = -1; (k = nextelement(gr,m,k)) >= 0;) {
      if(i < k) {
        if(!dg[n*k + i] && !dg[n*i + k]) is_pattern = FALSE;
      }
    } 
  }

  free(adj);
  free(deg);

  return(is_pattern);
}

/*
 * Get the number of loops from a pattern. 
 * ---------------------------------------
 * Input:
 *   char* dg; adjacency matrix of the pattern
 *   int n; order of the pattern
 * Returns:
 *   number of loops in dg.
 */
int number_of_loops(char* dg, int n) {
  int c=0,i;
  for(i = 0; i < n; i++) {
    if(dg[i*n + i] == 1) c++;
  }
  return(c);
}

/*
 * Print the adjacency matrix of the current graph.
 * ------------------------------------------------
 * First, canonically labels the graph.
 * 
 * char* dg; adjacency matrix of the partially directed graph.
 * int n; order of the graph.
 */
void print_pattern(char* dg, int n) {
  DYNALLSTAT(int,lab,lab_sz); 
  DYNALLSTAT(int,ptn,ptn_sz);
  DYNALLSTAT(int,orbits,orbits_sz);
  DYNALLSTAT(int,map,map_sz);
  DYNALLSTAT(graph,g,g_sz); 
  DYNALLSTAT(graph,cg,cg_sz); 
  static DEFAULTOPTIONS_GRAPH(options);

  options.digraph = TRUE;

  statsblk stats;

  int m,i; 

  /* Select option for canonical labelling */

  options.getcanon = TRUE; 

  m = SETWORDSNEEDED(n);
  nauty_check(WORDSIZE,m,n,NAUTYVERSIONID);

  DYNALLOC1(int,lab,lab_sz,n,"malloc"); 
  DYNALLOC1(int,ptn,ptn_sz,n,"malloc");
  DYNALLOC1(int,orbits,orbits_sz,n,"malloc");
  DYNALLOC1(int,map,map_sz,n,"malloc");
  DYNALLOC2(graph,g,g_sz,n,m,"malloc"); 
  DYNALLOC2(graph,cg,cg_sz,n,m,"malloc"); 

  char* line = calloc(n+1,sizeof(char));
  int k,v; 
  EMPTYGRAPH(g,m,n); 

  for(k = 0; k < n*n; k++) { 
    if(dg[k] == 1) ADDELEMENT(GRAPHROW(g,k/n,m),k%n); 
  }

  densenauty(g,lab,ptn,orbits,&options,&stats,m,n,cg);

  for(v = 0; v < n; v++) {
    for(i = 0; i < n; i++) {
      if(ISELEMENT(GRAPHROW(cg,v,m),i)) printf("1");
      else printf("0");
    }
  }
  printf("\n"); 
}

int main(int argc, char *argv[]) { 
  /* Nauty setup dynamic allocation */
  DYNALLSTAT(graph,g,g_sz);
  DYNALLSTAT(int,lab,lab_sz);
  DYNALLSTAT(int,ptn,ptn_sz);
  DYNALLSTAT(int,orbits,orbits_sz);
  static DEFAULTOPTIONS_GRAPH(options);
  options.getcanon = TRUE;
  statsblk stats;

  int n,m,v,i;
  set *gv; 

  char* dg; /* adj.mx. of current pattern */ 
  char* dc; /* counter for looping through directions */

  char* line = calloc(MAXSIZE*MAXSIZE + 1, sizeof(char));
  while(scanf("%s\n",line) != EOF) { /* whocares bufferoverflows */
    n = graphsize(line);
    m = SETWORDSNEEDED(n);
    nauty_check(WORDSIZE,m,n,NAUTYVERSIONID);

    DYNALLOC2(graph,g,g_sz,m,n,"malloc");
    DYNALLOC1(int,lab,lab_sz,n,"malloc"); 
    DYNALLOC1(int,ptn,ptn_sz,n,"malloc");
    DYNALLOC1(int,orbits,orbits_sz,n,"malloc"); 

    stringtograph(line,g,m); 
#ifdef DEBUGPRINTING
    printf(" > degseq ");
    for(i = 0; i < n; i++) { 
      printf("%d ", setsize(GRAPHROW(g,i,m), m));
    }
    printf("\n");
#endif

    dg = calloc(n*n,sizeof(char)); 
    dc = calloc(n,sizeof(char));
    for(i = 0; i < n; i++) dc[i] = 0;  /* initialize */
    do { 
     if(apply_counter(dg,dc,g,n,m)) {
       printf("%d ",n+3*number_of_loops(dg,n));
       print_pattern(dg,n);
     }
#ifdef DEBUGPRINTING
     printf(" > ");
     for(v = 0; v < n; v++) {
       printf("%d ", dc[v]);
     } 
     printf("\n");
#endif 
    } while(increment_counter(dc,g,n,m));
    free(dg);
    free(dc); 
  }

  free(line);
  return(0); 
}
