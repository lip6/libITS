#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "its/ITSModel.hh"
#include "petri/PNet.hh"
#include "its/composite/Composite.hh"
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

void buildProcType (ITSModel & m, int n) {
  PNet net ("Process") ;
  
  net.addPlace ("prio");

  for (int i=0; i < n; ++i) {
    net.setMarking("init"+id(i),"prio",i);
  }

  for (int i=0; i < n; ++i) {
    vLabel tname = "get"+id(i);
    net.addTransition(tname, tname , PUBLIC);
    if (i != 0)
      net.addArc (PNet::ArcVal("prio",i), tname,TEST);
    net.addArc (PNet::ArcVal("prio",i+1), tname,INHIBITOR);

    tname = "set"+id(i);
    net.addTransition(tname, tname , PUBLIC);
    net.addArc (PNet::ArcVal("prio",0), tname,RESET);
    if (i != 0)
      net.addArc (PNet::ArcVal("prio",i), tname,OUTPUT);
    
  }

  vLabel tname = "inc";
  net.addTransition("inclessn", tname , PUBLIC);
  net.addArc (PNet::ArcVal("prio",n-1),"inclessn",INHIBITOR);
  net.addArc (PNet::ArcVal("prio",1),"inclessn",OUTPUT);

  tname = "inc";
  net.addTransition("incmodn", tname , PUBLIC);
  net.addArc (PNet::ArcVal("prio",n-1), "incmodn",TEST);
  net.addArc (PNet::ArcVal("prio",1), "incmodn",RESET);

  m.declareType (net);
}


void buildTokenRingFinal (ITSModel & m, int n) {
  Composite comp ("FullRing");

  for (int i=0; i<n; i++) {
    comp.addInstance (id(i),"Process",m);
  }
  for (int i=0; i<n; i++) {
    comp.updateStateDef("init",id(i),"init"+id(i));
  }

  for (int i=0; i < n-1; ++i) {
    for (int j=0; j < n; ++j) {
    // private = empty label
      vLabel sname = "internal"+id(i)+"_"+to_string(j);
      comp.addSynchronization(sname,"");
      comp.addSyncPart(sname,id(i),"get"+id(j));
      comp.addSyncPart(sname,id(i+1),"set"+id(j));
    }    
  }
  
  for (int i=0; i < n; ++i) {
    vLabel sname = "loop"+id(i);
    comp.addSynchronization(sname,"");
    comp.addSyncPart(sname,id(n-1),"get"+id(i));
    comp.addSyncPart(sname,id(0),"get"+id(i));
    comp.addSyncPart(sname,id(0),"inc");
  }

  m.declareType (comp);
}


extern "C" void loadModel (its::ITSModel & model, int N) {
  buildProcType(model,N);
  buildTokenRingFinal(model,N);

  model.setInstance("FullRing","main");
  model.setInstanceState("init");

}


int main (int argc, char **argv) {
  
  // create a model to hold net types.
  ITSModel model ;

  int n = atoi(argv[1]);
  buildProcType(model,n);

  buildTokenRingFinal(model,n);


  model.setInstance("FullRing","main");
  model.setInstanceState("init");
//  std::cerr << model << std::endl;

  State s = model.computeReachable(true);
  Statistic S = Statistic(s,"token ring "+to_string(n),CSV);  

  S.print_header(std::cout);
  S.print_line(std::cout);


//  exportDot(s, "token"+to_string(n));
}
