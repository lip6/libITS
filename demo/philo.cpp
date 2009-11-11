#include <iostream>
#include <fstream>
#include <cstdio>

#include "IPN.hh"
#include "statistic.hpp"


std::string toString (int i) {
  char buff [16];
  sprintf (buff,"%d",i);
  return buff;
}


void buildPhiloType (IPNModel & m) {
  UnitNet * philo = new UnitNet("Philo") ;
  
  philo->addPlace ("Idle");
  philo->addPlace ("WaitL");
  philo->addPlace ("WaitR");
  philo->addPlace ("HasL");
  philo->addPlace ("HasR");

  philo->openStateDef ("default");
  philo->updateStateDef("Idle",1);
  philo->closeStateDef();
  
  philo->addTransition("hungry", PRIVATE);
  philo->addArc ("Idle","hungry",1,INPUT);
  philo->addArc ("WaitL","hungry",1,OUTPUT);
  philo->addArc ("WaitR","hungry",1,OUTPUT);
  
  philo->addTransition("getLeft", PUBLIC);
  philo->addArc ("WaitL","getLeft",1,INPUT);
  philo->addArc ("HasL","getLeft",1,OUTPUT);

  philo->addTransition("getRight", PUBLIC);
  philo->addArc ("WaitR","getRight",1,INPUT);
  philo->addArc ("HasR","getRight",1,OUTPUT);

  philo->addTransition("eat", PUBLIC);
  philo->addArc ("HasL","eat",1,INPUT);
  philo->addArc ("HasR","eat",1,INPUT);
  philo->addArc ("Idle","eat",1,OUTPUT);

  m.addType (philo);


}

void buildForkType (IPNModel & m) {
  UnitNet * fork = new UnitNet("Fork") ;

  fork->addPlace ("fork");

  fork->openStateDef ("default");
  fork->updateStateDef("fork",1);
  fork->closeStateDef();

  fork->addTransition ("get",PUBLIC);
  fork->addArc ("fork","get",1,INPUT);

  fork->addTransition ("put",PUBLIC);
  fork->addArc("fork","put",1,OUTPUT);

  m.addType(fork);
}

void buildPhiloForkType (IPNModel & m) {
  // 2^0 philo = 1 philo
  CompositeNet *pf = new CompositeNet ("PhiloFork0");

  pf->addInstance ("p","Philo",m);
  pf->addInstance ("f","Fork",m);
  
  pf->addSynchronization("internal",PRIVATE);
  pf->addSyncPart("internal","p","getLeft");
  pf->addSyncPart("internal","f","get");

  pf->addSynchronization("getRight",PUBLIC);
  pf->addSyncPart("getRight","p","getRight");

  pf->addSynchronization("putRight",PUBLIC);
  pf->addSyncPart("putRight","p","eat");
  pf->addSyncPart("putRight","f","put");

  pf->addSynchronization("getFork",PUBLIC);
  pf->addSyncPart("getFork","f","get");

  pf->addSynchronization("putFork",PUBLIC);
  pf->addSyncPart("putFork","f","put");

  m.addType (pf);
}

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




int main (int argc, char **argv) {
  
  // create a model to hold net types.
  IPNModel model ;
  buildPhiloType (model);
  buildForkType (model);
  buildPhiloForkType (model);

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

}
