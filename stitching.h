#ifndef STITCHING_H
#define STITCHING_H

#include "graph.h"
#include "essentialinfo.h"

void attach2(mygraph* G, vertex v, vertex w, stss M); 
void upd1stitch(mygraph* G, pattern* P, essentialinfo* info, vertex x);
void make2stitch(mygraph* G, vertex apex, vertex snd, vertex thd, stss M);
void upd2stitch(mygraph* G, pattern* P, essentialinfo* info, vertex x,
    vertex join);
void make3stitch(mygraph* G, vertex c1, vertex c2, vertex c3, vertex c4,
    stss M1, stss M2); 
void upd3stitch(mygraph* G, pattern* P, essentialinfo* info, vertex x,
    vertex join1, vertex join2);
stss get_fitting_stss(pattern* P, essentialinfo* info, vertex x, vertex join,
    int join_back_deg); 
void make4symmstitch(mygraph* G, vertex apex, vertex coapex, vertex v1,
    vertex v2, vertex v3, stss M1, stss M2, stss M3); 
void make4asymmstitch(mygraph* G, vertex h1, vertex h2, vertex h3, vertex h4,
  vertex h5, stss M1, stss M2, stss M3);
void upd4stitch(mygraph* G, pattern* P, essentialinfo* info, vertex x,
    vertex join1, vertex join2, vertex join3); 

#endif
