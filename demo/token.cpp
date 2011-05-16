#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ITSModel.hh"
#include "petri/PNet.hh"
#include "composite/Composite.hh"
#include "scalar/CircularSet.hh"
#include "util/dotExporter.h"

#include "statistic.hpp"


using namespace its;
using namespace std;

void usage() {
  cerr << "Instantiable Transition Systems: Philo example; package " << PACKAGE_STRING <<endl;
  cerr << "Usage : philoITS NN [-ssD2/-ssDR/-ssDS see below]  where NN is the mandatory size. Default is -ssD2 1" << endl;
  cerr<<  "    -ssD2 INT : use 2 level depth for scalar sets. Integer provided defines level 2 block size." <<endl;
  cerr<<  "    -ssDR INT : use recursive encoding for scalar sets. Integer provided defines number of blocks at highest levels." <<endl;
  cerr<<  "    -ssDS INT : use alternative recursive encoding for scalar sets. Integer provided defines number of blocks at lowest level." <<endl;
}


static vLabel id (int k) {
  return "s"+to_string(k)+"_";
}

void buildPrioType (ITSModel & m, int n) {
  PNet net ("Priority") ;
  

  for (int i=0; i < n; ++i) {
    net.addPlace (id(i));
  }

  for (int i=0; i < n; ++i) {
    net.setMarking("init"+id(i),id(i),1);
  }

  for (int i=0; i < n; ++i) {
    net.setMarking("initfull",id(i),1);
    net.setMarking("initempty",id(i),0);
  }


  for (int i=0; i < n-1; ++i) {
    vLabel tname = "inc"+id(i+1);
    net.addTransition(tname, tname , PUBLIC);
    net.addArc (PNet::ArcVal(id(i),1), tname,TEST);
    net.addArc (PNet::ArcVal(id(i+1),1), tname,OUTPUT);
  }

  for (int i=1; i < n; ++i) {
    vLabel tname = "reset"+id(i);
    net.addTransition(tname, tname , PUBLIC);
    net.addArc (PNet::ArcVal(id(i),0), tname,RESET);
  }

  vLabel tname = "test";
  net.addTransition(tname, tname , PUBLIC);
  net.addArc (PNet::ArcVal(id(n-1),1), tname,TEST);
  net.addArc (PNet::ArcVal(id(0),1), tname,INPUT);

  tname = "put";
  net.addTransition(tname, tname , PUBLIC);
  net.addArc (PNet::ArcVal(id(0),1), tname,OUTPUT);
  
  m.declareType (net);
}


void buildTokenRingCircular (ITSModel & m, int n) {
  CircularSet cs ("Ring");

  cs.setSize(n);

  cs.setInstance("prio","Priority",m);

  {
    labels_t substates;
//    substates.push_back("initfull");
    for (int i=0 ; i <n ; ++i) {
//      substates.push_back("initempty");
      substates.push_back("init"+id(i));
    }
    cs.createStateDef("init",substates);
  }

  
  
  vLabel sname = "loop";
  cs.addSynchronization(sname,"");
  cs.addSyncPart(sname,CircularSet::CURRENT,labels_t(1,"test"));
  cs.addSyncPart(sname,CircularSet::NEXT,labels_t(1,"put"));

  for (int i=1; i < n; ++i) {
    sname = "inc"+id(i);
    cs.addDelegator (sname, sname, false);
  }

  for (int i=1; i < n; ++i) {
    sname = "reset"+id(i);
    cs.addDelegator (sname, sname, true);
  }

//  cs.print(std::cout);
  m.declareType(cs);
}

void buildTokenRingFinal (ITSModel & m, int n) {
  Composite comp ("FullRing");

  comp.addInstance ("ring","Ring",m);
  
  comp.updateStateDef("init","ring","init");

  
  for (int i=1; i < n; ++i) {
    // private = empty label
    vLabel sname = "internal"+id(i);
    comp.addSynchronization(sname,"");
    comp.addSyncPart(sname,"ring","reset"+id(i));
    comp.addSyncPart(sname,"ring","inc"+id(i));    
  }

  m.declareType (comp);
}



int main (int argc, char **argv) {
  
  // create a model to hold net types.
  ITSModel model ;

  int n = atoi(argv[1]);
  buildPrioType(model,n);

  buildTokenRingCircular(model,n);
  buildTokenRingFinal(model,n);


  model.setInstance("FullRing","main");
  model.setInstanceState("init");
//  std::cerr << model << std::endl;

  State s = model.computeReachable(true);
  Statistic S = Statistic(s,"token ring "+to_string(n),CSV);  

  S.print_header(std::cout);
  S.print_line(std::cout);


  exportDot(s, "token"+to_string(n));
}
