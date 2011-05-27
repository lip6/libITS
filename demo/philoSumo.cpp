#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "ITSModel.hh"
#include "petri/PNet.hh"
#include "composite/Composite.hh"
#include "scalar/CircularSet.hh"

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

void buildPhiloType (ITSModel & m) {
  PNet net ("Philo") ;
  
  net.addPlace ("Think");
  net.addPlace ("Catch1");
  net.addPlace ("Catch2");
  net.addPlace ("Eat");
  net.addPlace ("Fork");

  net.setMarking("init","Think",1);
  net.setMarking("init","Fork",1);

  vLabel tname = "FF1a";
  net.addTransition(tname, "getLeft" , PUBLIC);
  net.addArc (PNet::ArcVal("Think",1), tname,INPUT);
  net.addArc (PNet::ArcVal("Catch1",1), tname,OUTPUT);

  tname = "FF1b";
  net.addTransition(tname, "" , PRIVATE);
  net.addArc (PNet::ArcVal("Think",1), tname,INPUT);
  net.addArc (PNet::ArcVal("Fork",1), tname,INPUT);
  net.addArc (PNet::ArcVal("Catch2",1), tname,OUTPUT);

  tname = "FF2a";
  net.addTransition(tname, "" , PRIVATE);
  net.addArc (PNet::ArcVal("Catch1",1), tname,INPUT);
  net.addArc (PNet::ArcVal("Fork",1), tname,INPUT);
  net.addArc (PNet::ArcVal("Eat",1), tname,OUTPUT);

  tname = "FF2b";
  net.addTransition(tname, "getLeft" , PUBLIC);
  net.addArc (PNet::ArcVal("Catch2",1), tname,INPUT);
  net.addArc (PNet::ArcVal("Eat",1), tname,OUTPUT);
  
  tname = "release";
  net.addTransition(tname, tname , PUBLIC);
  net.addArc (PNet::ArcVal("Eat",1), tname,INPUT);
  net.addArc (PNet::ArcVal("Fork",1), tname,OUTPUT);
  net.addArc (PNet::ArcVal("Think",1), tname,OUTPUT);


  tname = "get";
  net.addTransition(tname, tname , PUBLIC);
  net.addArc (PNet::ArcVal("Fork",1), tname,INPUT);

  tname = "put";
  net.addTransition(tname, tname , PUBLIC);
  net.addArc (PNet::ArcVal("Fork",1), tname,OUTPUT);

  m.declareType (net);
}


void buildPhiloCircular (ITSModel & m, int n) {
  CircularSet cs ("Philos");

  cs.setSize(n);

  cs.setInstance("pf","Philo",m);

  cs.createStateDef("init","init");
  
  vLabel sname = "get";
  cs.addSynchronization(sname,"");
  cs.addSyncPart(sname,CircularSet::CURRENT,labels_t(1,"get"));
  cs.addSyncPart(sname,CircularSet::NEXT,labels_t(1,"getLeft"));

  sname = "put";
  cs.addSynchronization(sname,"");
  cs.addSyncPart(sname,CircularSet::CURRENT,labels_t(1,"put"));
  cs.addSyncPart(sname,CircularSet::NEXT,labels_t(1,"release"));

//  cs.print(std::cout);
  m.declareType(cs);
}

extern "C" void loadModel (its::ITSModel & model, int N) {

  buildPhiloType (model);
  buildPhiloCircular(model,N);

  model.setInstance("Philos","main");
  model.setInstanceState("init");

}
