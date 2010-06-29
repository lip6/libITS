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
  
  net.addPlace ("Idle");
  net.addPlace ("WaitL");
  net.addPlace ("WaitR");
  net.addPlace ("HasL");
  net.addPlace ("HasR");

  net.setMarking("init","Idle",1);

  vLabel tname = "hungry";
  net.addTransition(tname, "" , PRIVATE);
  net.addArc (PNet::ArcVal("Idle",1), tname,INPUT);
  net.addArc (PNet::ArcVal("WaitL",1), tname,OUTPUT);
  net.addArc (PNet::ArcVal("WaitR",1), tname,OUTPUT);
  
  tname = "getLeft";
  net.addTransition(tname, tname , PUBLIC);
  net.addArc (PNet::ArcVal("WaitL",1), tname,INPUT);
  net.addArc (PNet::ArcVal("HasL",1), tname,OUTPUT);

  tname = "getRight";
  net.addTransition(tname, tname , PUBLIC);
  net.addArc (PNet::ArcVal("WaitR",1), tname,INPUT);
  net.addArc (PNet::ArcVal("HasR",1), tname,OUTPUT);

  tname = "eat";
  net.addTransition(tname, tname , PUBLIC);
  net.addArc (PNet::ArcVal("HasL",1), tname,INPUT);
  net.addArc (PNet::ArcVal("HasR",1), tname,INPUT);
  net.addArc (PNet::ArcVal("Idle",1), tname,OUTPUT);
  
  m.declareType (net);
}

void buildForkType (ITSModel & m) {
  PNet net("Fork");

  net.addPlace ("fork");

  net.setMarking("init","fork",1);

  vLabel tname = "get";
  net.addTransition(tname, tname , PUBLIC);
  net.addArc (PNet::ArcVal("fork",1), tname,INPUT);

  tname = "put";
  net.addTransition(tname, tname , PUBLIC);
  net.addArc (PNet::ArcVal("fork",1), tname,OUTPUT);

  m.declareType(net);
}

void buildPhiloForkType (ITSModel & m) {
  // 2^0 philo = 1 philo
  Composite comp ("PhiloFork");

  comp.addInstance ("p","Philo",m);
  comp.addInstance ("f","Fork",m);
  
  comp.updateStateDef("init","p","init");
  comp.updateStateDef("init","f","init");

  // private = empty label
  vLabel sname = "internal";
  comp.addSynchronization(sname,"");
  comp.addSyncPart(sname,"p","getLeft");
  comp.addSyncPart(sname,"f","get");

  sname = "getRight";
  comp.addSynchronization(sname,sname);
  comp.addSyncPart(sname,"p","getRight");

  sname = "putRight";
  comp.addSynchronization(sname,sname);
  comp.addSyncPart(sname,"p","eat");
  comp.addSyncPart(sname,"f","put");

  sname = "getFork";
  comp.addSynchronization(sname,sname);
  comp.addSyncPart(sname,"f","get");

  sname = "putFork";
  comp.addSynchronization(sname,sname);
  comp.addSyncPart(sname,"f","put");

  m.declareType (comp);
}


void buildPhiloCircular (ITSModel & m, int n) {
  CircularSet cs ("Philos");

  cs.setSize(n);

  cs.setInstance("pf","PhiloFork",m);

  cs.createStateDef("init","init");
  
  vLabel sname = "get";
  cs.addSynchronization(sname,"");
  cs.addSyncPart(sname,CircularSet::CURRENT,labels_t(1,"getRight"));
  cs.addSyncPart(sname,CircularSet::NEXT,labels_t(1,"getFork"));

  sname = "put";
  cs.addSynchronization(sname,"");
  cs.addSyncPart(sname,CircularSet::CURRENT,labels_t(1,"putRight"));
  cs.addSyncPart(sname,CircularSet::NEXT,labels_t(1,"putFork"));

  cs.print(std::cout);
  m.declareType(cs);
}
/*
void buildPhiloCompose (IPNModel & m, int n) {
  CompositeNet *pf = new CompositeNet ("PhiloFork" + toString(n));

  pf->addInstance ("p1","PhiloFork"+ toString(n-1) ,m);
  pf->addInstance ("p2","PhiloFork"+ toString(n-1) ,m);

  pf->addSynchronization("internalGet",PRIVATE);
  pf->addSyncPart("internalGet","p1","getRight");
  pf->addSyncPart("internalGet","p2","getFork");

  pf->addSynchronization("internalPut",PRIVATE);
  pf->addSyncPart("internalPut","p1","putRight");
  pf->addSyncPart("internalPut","p2","putFork");


  pf->addSynchronization("getRight",PUBLIC);
  pf->addSyncPart("getRight","p2","getRight");

  pf->addSynchronization("putRight",PUBLIC);
  pf->addSyncPart("putRight","p2","putRight");

  pf->addSynchronization("getFork",PUBLIC);
  pf->addSyncPart("getFork","p1","getFork");

  pf->addSynchronization("putFork",PUBLIC);
  pf->addSyncPart("putFork","p1","putFork");

  m.addType (pf);
}


void buildTableType (IPNModel & m, const std::string & nphilo) {
  CompositeNet *table = new CompositeNet ("Table");

  table->addInstance("phils",nphilo,m);
  
  table->addSynchronization("get",PRIVATE);
  table->addSyncPart("get","phils","getFork");
  table->addSyncPart("get","phils","getRight");

  table->addSynchronization("put",PRIVATE);
  table->addSyncPart("put","phils","putFork");
  table->addSyncPart("put","phils","putRight");

  m.addType (table);
}

*/




int main (int argc, char **argv) {
  
  // create a model to hold net types.
  ITSModel model ;
  buildPhiloType (model);
  buildForkType (model);
  buildPhiloForkType (model);

  if (argc >=3) {
    int i = 2;
    if (! strcmp(argv[i],"-ssD2") ) {
     if (++i > argc) 
       { cerr << "give argument value for scalar strategy " << argv[i-1]<<endl; usage() ; exit(1);}
     int grain = atoi(argv[i]);
     model.setScalarStrategy(DEPTH1,grain);
    }else if (! strcmp(argv[i],"-ssDR") ) {
      if (++i > argc) 
	{ cerr << "give argument value for scalar strategy " << argv[i-1]<<endl; usage() ; exit(1);}
      int grain = atoi(argv[i]);
      model.setScalarStrategy(DEPTHREC,grain);   
    }else if (! strcmp(argv[i],"-ssDS") ) {
      if (++i > argc) 
	{ cerr << "give argument value for scalar strategy " << argv[i-1]<<endl; usage() ; exit(1);}
      int grain = atoi(argv[i]);
      model.setScalarStrategy(SHALLOWREC,grain);
    }
  }
  	
  int n = 10;
  if (argc >= 2)
    n = atoi(argv[1]);
  buildPhiloCircular(model,n);

  model.setInstance("Philos","main");
  model.setInstanceState("init");
  std::cerr << model << std::endl;

  State s = model.computeReachable(true);
  Statistic S = Statistic(s,"Philos "+to_string(n),CSV);  

  S.print_header(std::cout);
  S.print_line(std::cout);

  /*
  // 2^pow philo, e.g. pow = 3 -> 8 philo
  int pow ;
  if (argc > 1) {
    pow = atoi(argv[1]);
  } else {
    pow = 3;
  }

  for (int i = 1; i <= pow ; ++i)
    buildPhiloCompose(model,i);    

  buildTableType(model,"PhiloFork"+toString(pow));

  model.setInstance("Table","philo2pow"+toString(pow));

  if (pow <= 3)
    std::cout << model << std::endl ;


  // State manipulations tests

  SDD M0 = model.getInstance()->getSDDState(0);
  Shom trans = model.getInstance()->getSDDTransRel (0);


  if (pow <= 3)
    std::cout << M0 << std::endl ;

  Shom fix = fixpoint(trans+GShom::id);
  SDD Mfinal = fix (M0);
  Statistic S = Statistic(Mfinal,model.getInstance()->getName(),CSV);  
  //  S.print_table(std::cout);

  std::ofstream out ("stats.tex",std::ios_base::app) ; 

  S.print_line(out);
  S.print_header(std::cout);
  S.print_line(std::cout);
  */
}
