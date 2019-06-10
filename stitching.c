#include "stitching.h" 
#include "graph.h"

void attach2(mygraph* G, vertex v, vertex w, stss M) {
  assert(M.type == BALL || M.type == CYCLE || M.type == PATH);
  int i;
  assert(M.set[0] != UNDEFINED);

  add_edge(G,v,M.set[0]);
  if(M.type == BALL) {
    assert(M.set[1] != UNDEFINED);
    for(i = 1; i < 4; i++) {
      if(M.set[i] != UNDEFINED) add_edge(G,w,M.set[i]); 
    }
  } else if (M.type == CYCLE) {
    assert(M.set[1] != UNDEFINED);
    assert(M.set[2] != UNDEFINED);
    assert(M.set[3] != UNDEFINED); 
    add_edge(G,v,M.set[2]);
    add_edge(G,w,M.set[1]);
    add_edge(G,w,M.set[3]);
  } else {  // M.type == PATH
    assert(M.set[1] != UNDEFINED);
    assert(M.set[2] != UNDEFINED);
    assert(M.set[3] != UNDEFINED);
    add_edge(G,v,M.set[2]);
    add_edge(G,w,M.set[1]);
    add_edge(G,w,M.set[3]);
  }
}

void upd1stitch(mygraph* G, pattern* P, essentialinfo* info, vertex x) {
  vertex x1 = current_order(G);
  vertex x2 = x1+1;
  add_edge(G,x1,x2);
  set_apex(info,x,x1);
  set_coapex(info,x,x2);
  activate(info,x,APEX);
  activate(info,x,COAPEX);
}

void make2stitch(mygraph* G, vertex apex, vertex snd, vertex thd, stss M) {
  add_edge(G,apex,snd);
  add_edge(G,apex,thd);
  attach2(G,snd,thd,M);
}

stss get_fitting_stss(pattern* P, essentialinfo* info, vertex x, vertex join,
    int join_back_deg) { 
  stss M;

  if(join_back_deg == 0) {
    M.type = BALL;
    M.set[0] = coapex(info,join);
    M.set[1] = apex(info,join); 
    M.set[2] = UNDEFINED;
    M.set[3] = UNDEFINED;
    assert(M.type == BALL);
  } else if (join_back_deg == 1) {
    M = get_ball_stss(info, join, 0);
    assert(M.type == BALL);
  } else { // join_back_deg == 2
    int join_outdeg = pattern_outdeg(P, join);
    assert(join_outdeg == 1 || join_outdeg == 3);
    if(join_outdeg == 1) {
      if(pattern_has_diedge(P,join,x)) {
        M = get_cycle_stss(info,join);
      } else {
        if(has_aspect(info,apex(info,join),points_to(P,join))) {
          M = get_ball_stss(info,join,0);
        } else {
          assert(has_aspect(info,coapex(info,join),points_to(P,join)));
          M = get_ball_stss(info,join,1);
        }
        assert(M.type == BALL);
      }
    } else { // join_outdeg == 3
      M = get_path_stss(info,join);
      assert(M.type == PATH);
    }
  }

  return(M);
}

/* TODO: Perhaps merge the above function with the one below?? */

void upd2stitch(mygraph* G, pattern* P, essentialinfo* info, vertex x,
    vertex join) {
  vertex ap = current_order(G);
  vertex snd = ap+1;
  vertex thd = ap+2; 
  stss tmpstss;

  stss Mdummy;
  Mdummy.type = EMPTY;
  Mdummy.set[0] = UNDEFINED; Mdummy.set[1] = UNDEFINED;
  Mdummy.set[2] = UNDEFINED; Mdummy.set[3] = UNDEFINED; 

  int join_back_deg = pattern_back_deg(P,join,x);
  assert(join_back_deg == 0 || join_back_deg == 1 || join_back_deg == 2);

  assert(join != UNDEFINED);
  assert(x != UNDEFINED);

  stss M = get_fitting_stss(P,info,x,join,join_back_deg);
  make2stitch(G,ap,snd,thd,M);

  if(join_back_deg == 0) { 
    set_apex(info,x,ap);
    set_coapex(info,x,snd);
    activate(info,x,APEX);
    activate(info,x,COAPEX); 

    find_1ball(G,apex(info,join),tmpstss.set,4); 
    tmpstss.type = BALL;
    set_stss(info,join,APEX,tmpstss);

    add_aspect(info,coapex(info,x),join);
    add_aspect(info,coapex(info,join),x);
  } else if (join_back_deg == 1) { 
    set_apex(info,x,ap);
    set_coapex(info,x,snd);
    activate(info,x,APEX);
    activate(info,x,COAPEX);

    find_1ball(G,apex(info,join),tmpstss.set,4);
    tmpstss.type = BALL;
    set_stss(info,join,APEX,tmpstss); 
    //find_4cycle(G,apex(info,join),coapex(info,join),tmpstss.set);
    special_find4cycle(get_ball_stss(info,join,APEX),G,tmpstss.set);
    tmpstss.type = CYCLE;
    set_stss(info,join,CX,tmpstss);
    if(coapex(info,join) != UNDEFINED &&
        is_active(info,join,COAPEX)) {
      find_1ball(G,coapex(info,join),tmpstss.set,4);
      tmpstss.type = BALL;
      set_stss(info,join,COAPEX,tmpstss); 
      find_4path(G,apex(info,join),coapex(info,join),
          get_cycle_stss(info,join).set[2],
          get_cycle_stss(info,join).set[3],tmpstss.set);
      tmpstss.type = PATH;
      set_stss(info,join,PX,tmpstss);
    } 

    add_aspect(info,apex(info,join),x); 
  } else { // join_back_deg == 2
    int join_outdeg = pattern_outdeg(P, join);
    assert(join_outdeg == 1 || join_outdeg == 3);
    set_apex(info,x,ap); 
    activate(info,x,APEX);
// TMP
    set_coapex(info,x,snd);
// END TMP
    if(join_outdeg == 1 && !pattern_has_diedge(P,join,x)) {
      set_coapex(info,x,snd);
      activate(info,x,COAPEX);
    } 

    deactivate(info,join,APEX);
    deactivate(info,join,COAPEX);
    delete_all_aspects_graph_vertex(info,apex(info,join));
    if(coapex(info,join) != UNDEFINED)
      delete_all_aspects_graph_vertex(info,coapex(info,join));

    set_stss(info,join,APEX,Mdummy);
    set_stss(info,join,COAPEX,Mdummy);
    set_stss(info,join,CX,Mdummy);
    set_stss(info,join,PX,Mdummy);
   
    vertex xi; 
    for(xi = 0; xi < P->order; xi++) {
      if(pattern_has_diedge(P,join,xi)) {
        if(is_active(info,xi,APEX) && 
           has_aspect(info,apex(info,xi),join)) {
          delete_all_aspects_graph_vertex(info,apex(info,xi));
          deactivate(info,xi,APEX);
          set_stss(info,xi,APEX,Mdummy);
          set_stss(info,xi,PX,Mdummy);
        }

        if(is_active(info,xi,COAPEX) &&
           has_aspect(info,coapex(info,xi),join)) {
          delete_all_aspects_graph_vertex(info,coapex(info,xi));
          deactivate(info,xi,COAPEX);
          set_stss(info,xi,COAPEX,Mdummy); 
          set_stss(info,xi,PX,Mdummy);
        }
      }
    }

  } 

  find_1ball(G,apex(info,x),tmpstss.set,4);
  tmpstss.type = BALL;
  set_stss(info,x,APEX,tmpstss); 

  if(coapex(info,x) != UNDEFINED) 
    add_aspect(info,coapex(info,x),join);
}

void make3stitch(mygraph* G, vertex c1, vertex c2, vertex c3, vertex c4,
    stss M1, stss M2) {
  add_edge(G,c1,c2);
  add_edge(G,c2,c3);
  add_edge(G,c3,c4);
  add_edge(G,c4,c1);
  attach2(G,c1,c3,M1);
  attach2(G,c2,c4,M2);
}

void upd3stitch(mygraph* G, pattern* P, essentialinfo* info, vertex x,
    vertex join1, vertex join2) {
  assert(x != UNDEFINED);
  vertex c2 = current_order(G);
  vertex c1 = c2+1;
  vertex c3 = c2+2; 
  vertex c4 = c2+3;
  stss tmpstss;

  stss Mdummy;
  Mdummy.type = EMPTY;
  Mdummy.set[0] = UNDEFINED; Mdummy.set[1] = UNDEFINED;
  Mdummy.set[2] = UNDEFINED; Mdummy.set[3] = UNDEFINED; 

  int join1_back_deg = pattern_back_deg(P,join1,x);
  int join2_back_deg = pattern_back_deg(P,join2,x);
  /* TODO: Change this to a swap-macro */
  if(join1_back_deg > join2_back_deg) {
    vertex tmp = join1;
    join1 = join2;
    join2 = tmp;

    int tmp_bd = join1_back_deg;
    join1_back_deg = join2_back_deg;
    join2_back_deg = tmp_bd;
  } else if(join1_back_deg == 2 && join2_back_deg == 2 &&
      pattern_has_diedge(P,join2,x) &&
      !pattern_has_diedge(P,join1,x)) {
    vertex tmp = join1;
    join1 = join2;
    join2 = tmp;

    int tmp_bd = join1_back_deg;
    join1_back_deg = join2_back_deg;
    join2_back_deg = tmp_bd; 
  }
  assert(join1_back_deg == pattern_back_deg(P,join1,x));
  assert(join2_back_deg == pattern_back_deg(P,join2,x));
  assert(join1_back_deg <= join2_back_deg);

  stss M1 = get_fitting_stss(P,info,x,join1,join1_back_deg);
  stss M2 = get_fitting_stss(P,info,x,join2,join2_back_deg);
  make3stitch(G,c1,c2,c3,c4,M1,M2);

  //TODO: The below is same thing as above, probably should be moved into
  // makeXstitch-function
  vertex join;
  vertex joins[2] = {join1,join2};
  int jbds[2] = {join1_back_deg, join2_back_deg};
  int join_back_deg;
  int i;
  for(i = 0; i < 2; i++) {
    join = joins[i];
    join_back_deg = jbds[i];
    assert(join != UNDEFINED); 
    if(join_back_deg == 0) {
      find_1ball(G,apex(info,join),tmpstss.set,4); 
      tmpstss.type = BALL;
      set_stss(info,join,APEX,tmpstss); 
      add_aspect(info,coapex(info,join),x);
    } else if (join_back_deg == 1) { 
      find_1ball(G,apex(info,join),tmpstss.set,4);
      tmpstss.type = BALL;
      set_stss(info,join,APEX,tmpstss); 
      //find_4cycle(G,apex(info,join),coapex(info,join),tmpstss.set);
      special_find4cycle(get_ball_stss(info,join,APEX),G,tmpstss.set);
      tmpstss.type = CYCLE;
      set_stss(info,join,CX,tmpstss); 
      if(coapex(info,join) != UNDEFINED &&
          is_active(info,join,COAPEX)) {
        find_1ball(G,coapex(info,join),tmpstss.set,4);
        tmpstss.type = BALL;
        set_stss(info,join,COAPEX,tmpstss); 
        find_4path(G,apex(info,join),coapex(info,join),
            get_cycle_stss(info,join).set[2],
            get_cycle_stss(info,join).set[3],tmpstss.set);
        tmpstss.type = PATH;
        set_stss(info,join,PX,tmpstss);
      } 
      add_aspect(info,apex(info,join),x); 
    } else { // join_back_deg == 2
      int join_outdeg = pattern_outdeg(P, join);
      assert(join_outdeg == 1 || join_outdeg == 3);
      deactivate(info,join,APEX);
      deactivate(info,join,COAPEX);
      delete_all_aspects_graph_vertex(info,apex(info,join));
      if(coapex(info,join) != UNDEFINED)
        delete_all_aspects_graph_vertex(info,coapex(info,join)); 
      set_stss(info,join,APEX,Mdummy);
      set_stss(info,join,COAPEX,Mdummy);
      set_stss(info,join,CX,Mdummy);
      set_stss(info,join,PX,Mdummy); 
      vertex xi; 
      for(xi = 0; xi < P->order; xi++) {
        if(pattern_has_diedge(P,join,xi)) {
          if(is_active(info,xi,APEX) && 
             has_aspect(info,apex(info,xi),join)) {
            delete_all_aspects_graph_vertex(info,apex(info,xi));
            deactivate(info,xi,APEX);
            set_stss(info,xi,APEX,Mdummy);
            set_stss(info,xi,PX,Mdummy);
          } 
          if(is_active(info,xi,COAPEX) &&
             has_aspect(info,coapex(info,xi),join)) {
            delete_all_aspects_graph_vertex(info,coapex(info,xi));
            deactivate(info,xi,COAPEX);
            set_stss(info,xi,COAPEX,Mdummy); 
            set_stss(info,xi,PX,Mdummy);
          }
        }
      } 
    } 
  }

  set_apex(info,x,c2); 
  tmpstss.set[0] = c1; tmpstss.set[1] = c2; tmpstss.set[2] = c3;
  tmpstss.set[3] = c4;
  tmpstss.type = CYCLE;
  set_stss(info,x,CX,tmpstss);
  if((join1_back_deg == 0 && join2_back_deg < 2) ||
     (join1_back_deg == 1 && join2_back_deg == 1) ||
     (join1_back_deg == 1 && join2_back_deg == 2 &&
       !pattern_has_diedge(P,join2,x)) ||
     (join1_back_deg == 2 && join2_back_deg == 2 &&
       !pattern_has_diedge(P,join1,x) &&
       !pattern_has_diedge(P,join2,x))) {
    set_coapex(info,x,c1);
    activate(info,x,APEX);
    activate(info,x,COAPEX); 
    
    find_1ball(G,apex(info,x),tmpstss.set,4);
    tmpstss.type = BALL;
    set_stss(info,x,APEX,tmpstss); 
    find_1ball(G,coapex(info,x),tmpstss.set,4);
    tmpstss.type = BALL;
    set_stss(info,x,COAPEX,tmpstss);
    find_4path(G,apex(info,x),coapex(info,x),c3,c4,tmpstss.set);
    tmpstss.type = PATH;
    set_stss(info,x,PX,tmpstss);

    add_aspect(info,apex(info,x),join2);
    add_aspect(info,coapex(info,x),join1);
  } else if (join1_back_deg == 0) {
    set_coapex(info,x,c1);
    activate(info,x,COAPEX);

    find_1ball(G,coapex(info,x),tmpstss.set,4);
    tmpstss.type = BALL;
    set_stss(info,x,COAPEX,tmpstss); 

    if(!pattern_has_diedge(P,join2,x)) {
      activate(info,x,APEX);
      find_1ball(G,apex(info,x),tmpstss.set,4);
      tmpstss.type = BALL; 
      set_stss(info,x,APEX,tmpstss);
      find_4path(G,apex(info,x),coapex(info,x),c3,c4,tmpstss.set);
      tmpstss.type = PATH;
      set_stss(info,x,PX,tmpstss); 
      add_aspect(info,apex(info,x),join2);
    }

    add_aspect(info,coapex(info,x),join1);
  } else if (join1_back_deg == 1) {
    assert(join2_back_deg == 2);
    assert(pattern_has_diedge(P,join2,x));

    set_coapex(info,x,c1);
    activate(info,x,COAPEX);
    find_1ball(G,coapex(info,x),tmpstss.set,4);
    tmpstss.type = BALL;
    set_stss(info,x,COAPEX,tmpstss);

    add_aspect(info,coapex(info,x),join1);
  } else if(pattern_has_diedge(P,join1,x) && pattern_has_diedge(P,join2,x)) {
    /* none active, no aspects */
    assert(join1_back_deg == 2);
    assert(join2_back_deg == 2);
    assert(!is_active(info,x,APEX));
    assert(!is_active(info,x,COAPEX));
    assert(coapex(info,x) == UNDEFINED);
#ifndef NDEBUG
    vertex t;
    for(t = 0; t < P->order; t++) {
      assert(!has_aspect(info,apex(info,x),t)); 
    }
#endif
  } else {
    assert(join1_back_deg == 2);
    assert(join2_back_deg == 2);
    assert(pattern_has_diedge(P,join1,x));
    assert(!pattern_has_diedge(P,join2,x)); 
    activate(info,x,APEX);
    find_1ball(G,apex(info,x),tmpstss.set,4);
    tmpstss.type = BALL;
    set_stss(info,x,APEX,tmpstss);

    add_aspect(info,apex(info,x),join2);

    assert(coapex(info,x) == UNDEFINED);
    assert(!is_active(info,x,COAPEX));
  } 
}

void make4symmstitch(mygraph* G, vertex apex, vertex coapex, vertex v1,
    vertex v2, vertex v3, stss M1, stss M2, stss M3) {
  int i;
  assert(M1.type == BALL);
  assert(M2.type == BALL);
  assert(M3.type == BALL);

  add_edge(G,apex,coapex);
  add_edge(G,apex,v1);
  add_edge(G,apex,v2);
  add_edge(G,apex,v3); 

  add_edge(G,coapex,M1.set[0]);
  add_edge(G,coapex,M2.set[0]);
  add_edge(G,coapex,M3.set[0]);
  add_edge(G,v1,M2.set[0]);
  add_edge(G,v2,M3.set[0]);
  add_edge(G,v3,M1.set[0]);

  for(i = 1; i < 4; i++) {
    if(M1.set[i] != UNDEFINED)
      add_edge(G,v1,M1.set[i]);
    if(M2.set[i] != UNDEFINED)
      add_edge(G,v2,M2.set[i]);
    if(M3.set[i] != UNDEFINED)
      add_edge(G,v3,M3.set[i]);
  }
}

void make4asymmstitch(mygraph* G, vertex h1, vertex h2, vertex h3, vertex h4,
    vertex h5, stss M1, stss M2, stss M3) { 
  assert(M3.type == BALL);

  attach2(G,h1,h2,M1);
  attach2(G,h3,h4,M2);
  attach2(G,h4,h5,M3);
  add_edge(G,h3,M3.set[0]);

  add_edge(G,h1,h3);
  add_edge(G,h1,h4);
  add_edge(G,h1,h5);
  add_edge(G,h2,h3);
  add_edge(G,h2,h4);
  add_edge(G,h2,h5);
}

void upd4stitch(mygraph* G, pattern* P, essentialinfo* info, vertex x,
    vertex join1, vertex join2, vertex join3) { 
  assert(x != UNDEFINED);
  vertex xi;
  vertex a1 = current_order(G);
  vertex a2 = a1+1; 
  vertex a3 = a1+2;
  vertex a4 = a1+3;
  vertex a5 = a1+4;
  stss tmpstss; 

  if(!is_symm(P,x)) {
    vertex tmp = join3;
    if(pattern_has_diedge(P,x,join1)) { 
      join3 = join1;
      join1 = tmp;
    } else if(pattern_has_diedge(P,x,join2)) { 
      join3 = join2;
      join2 = tmp;
    }
  }

  int join1_back_deg = pattern_back_deg(P,join1,x);
  int join2_back_deg = pattern_back_deg(P,join2,x);
  int join3_back_deg = pattern_back_deg(P,join3,x);
  stss M1 = get_fitting_stss(P,info,x,join1,join1_back_deg);
  stss M2 = get_fitting_stss(P,info,x,join2,join2_back_deg);
  stss M3 = get_fitting_stss(P,info,x,join3,join3_back_deg);

  stss Mdummy;
  Mdummy.type = EMPTY;
  Mdummy.set[0] = UNDEFINED; Mdummy.set[1] = UNDEFINED;
  Mdummy.set[2] = UNDEFINED; Mdummy.set[3] = UNDEFINED; 

  /* TODO: Set apex and (perhaps) coapex for x, even though none
   * should be active */
  if(is_symm(P,x)) {
    assert(M1.type == BALL);
    assert(M2.type == BALL);
    assert(M3.type == BALL);

    make4symmstitch(G,a1,a2,a3,a4,a5,M1,M2,M3); 
    set_apex(info,x,a1);
    set_coapex(info,x,a2);
  } else { 
    assert(M3.type == BALL);

    make4asymmstitch(G,a1,a2,a3,a4,a5,M1,M2,M3); 
    set_apex(info,x,a1);
  }

  /* TODO: Again, this is copied from the above. One should be able to make a
   * write this included in the above */
  vertex join;
  vertex joins[3] = {join1,join2,join3};
  int jbds[3] = {join1_back_deg, join2_back_deg, join3_back_deg};
  int join_back_deg;
  int i;
  for(i = 0; i < 3; i++) {
    join = joins[i];
    join_back_deg = jbds[i];
    assert(join != UNDEFINED); 
    if(join_back_deg == 0) {
      find_1ball(G,apex(info,join),tmpstss.set,4); 
      tmpstss.type = BALL;
      set_stss(info,join,APEX,tmpstss); 
      add_aspect(info,coapex(info,join),x);
    } else if (join_back_deg == 1) { 
      find_1ball(G,apex(info,join),tmpstss.set,4);
      tmpstss.type = BALL;
      set_stss(info,join,APEX,tmpstss); 
      // find_4cycle(G,apex(info,join),coapex(info,join),tmpstss.set);
      special_find4cycle(get_ball_stss(info,join,APEX),G,tmpstss.set);
      tmpstss.type = CYCLE;
      set_stss(info,join,CX,tmpstss); 
      if(coapex(info,join) != UNDEFINED &&
          is_active(info,join,COAPEX)) {
        find_1ball(G,coapex(info,join),tmpstss.set,4);
        tmpstss.type = BALL;
        set_stss(info,join,COAPEX,tmpstss); 
        find_4path(G,apex(info,join),coapex(info,join),
            get_cycle_stss(info,join).set[2],
            get_cycle_stss(info,join).set[3],tmpstss.set);
        tmpstss.type = PATH;
        set_stss(info,join,PX,tmpstss);
      } 
      add_aspect(info,apex(info,join),x); 
    } else { // join_back_deg == 2
      int join_outdeg = pattern_outdeg(P, join);
      assert(join_outdeg == 1 || join_outdeg == 3);
      deactivate(info,join,APEX);
      deactivate(info,join,COAPEX);
      delete_all_aspects_graph_vertex(info,apex(info,join));
      if(coapex(info,join) != UNDEFINED)
        delete_all_aspects_graph_vertex(info,coapex(info,join)); 
      set_stss(info,join,APEX,Mdummy);
      set_stss(info,join,COAPEX,Mdummy);
      set_stss(info,join,CX,Mdummy);
      set_stss(info,join,PX,Mdummy); 

      for(xi = 0; xi < P->order; xi++) {
        if(pattern_has_diedge(P,join,xi)) {
          if(is_active(info,xi,APEX) && 
             has_aspect(info,apex(info,xi),join)) {
            delete_all_aspects_graph_vertex(info,apex(info,xi));
            deactivate(info,xi,APEX);
            set_stss(info,xi,APEX,Mdummy);
            set_stss(info,xi,PX,Mdummy);
          } 
          if(is_active(info,xi,COAPEX) &&
             has_aspect(info,coapex(info,xi),join)) {
            delete_all_aspects_graph_vertex(info,coapex(info,xi));
            deactivate(info,xi,COAPEX);
            set_stss(info,xi,COAPEX,Mdummy); 
            set_stss(info,xi,PX,Mdummy);
          }
        }
      } 
    } 
  } 

  for(xi = 0; xi < P->order; xi++) {
    if(pattern_has_diedge(P,x,xi)) {
      if(is_active(info,xi,APEX) && 
         has_aspect(info,apex(info,xi),x)) {
        delete_all_aspects_graph_vertex(info,apex(info,xi));
        deactivate(info,xi,APEX);
        set_stss(info,xi,APEX,Mdummy);
        set_stss(info,xi,PX,Mdummy);
      } 
      if(is_active(info,xi,COAPEX) &&
         has_aspect(info,coapex(info,xi),x)) {
        delete_all_aspects_graph_vertex(info,coapex(info,xi));
        deactivate(info,xi,COAPEX);
        set_stss(info,xi,COAPEX,Mdummy); 
        set_stss(info,xi,PX,Mdummy);
      }
    }
  } 
}
