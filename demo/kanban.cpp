#include <iostream>
#include <fstream>
#include <cstdio>

#include "MemoryManager.h"
#include "util/dotExporter.h"
#include "IPN.hh"
#include "statistic.hpp"



// int -> string
std::string toString (int i) {
  char buff [16];
  sprintf (buff,"%d",i);
  return buff;
}
// float -> string
std::string toString (double i) {
  char buff [16];
  sprintf (buff,"%lf",i);
  return buff;
}
// name for types : Robin3 represents 3 elements
std::string typeName (const string & name,int n) {
  return name + toString(n);
}

void buildProcessType (IPNModel & m, int marking, int type) {
  UnitNet * res = new UnitNet("Process"+toString(type)) ;
 

 res->addPlace ("pM");
 res->addPlace ("pBack");
 res->addPlace ("pOut");
 res->addPlace ("p");

 res->openStateDef("default");
 res->updateStateDef("p",marking);
 res->closeStateDef();

 string tname;
 tname = "tredo";
 res->addTransition(tname, PRIVATE);
 res->addArc ("pM",tname,1,INPUT);
 res->addArc ("pBack",tname,1,OUTPUT);
 
 tname = "tback";
 res->addTransition(tname, PRIVATE);
 res->addArc ("pBack",tname,1,INPUT);
 res->addArc ("pM",tname,1,OUTPUT);
 
 tname = "tok";
 res->addTransition(tname, PRIVATE);
 res->addArc ("pM",tname,1,INPUT);
 res->addArc ("pOut",tname,1,OUTPUT);
 
 tname = "left";
 if (type != 0)
   res->addTransition(tname, PUBLIC);
 else
   res->addTransition(tname, PRIVATE);
 res->addArc ("p",tname,1,INPUT);
 res->addArc ("pM",tname,1,OUTPUT);

 tname = "right";
 if (type != 2)
   res->addTransition(tname, PUBLIC);
 else
   res->addTransition(tname, PRIVATE);
 res->addArc ("pOut",tname,1,INPUT);
 res->addArc ("p",tname,1,OUTPUT);

 m.addType(res);
}


void buildKanbanType (IPNModel & m, int marking) {
  buildProcessType(m,marking,0);
  buildProcessType(m,marking,1);
  buildProcessType(m,marking,2);

  CompositeNet * net = new CompositeNet(typeName("Kanban",marking));
  

  net->addInstance("p1","Process0",m);  
  net->addInstance("p2","Process1",m);  
  net->addInstance("p3","Process1",m);
  net->addInstance("p4","Process2",m);  

  // private syncs
  string tname;
  
/**
  tname = "leftP1";
  net->addSynchronization(tname,PRIVATE);
  net->addSyncPart(tname,"p1","left");

  tname = "rightP4";
  net->addSynchronization(tname,PRIVATE);
  net->addSyncPart(tname,"p4","right");
*/

  tname = "left";
  net->addSynchronization(tname,PRIVATE);
  net->addSyncPart(tname,"p3","left");
  net->addSyncPart(tname,"p2","left");
  net->addSyncPart(tname,"p1","right");


  
  tname = "right";
  net->addSynchronization(tname,PRIVATE);
  net->addSyncPart(tname,"p4","left");
  net->addSyncPart(tname,"p3","right");
  net->addSyncPart(tname,"p2","right");


  

  m.addType(net);
}


int main (int argc, char **argv) {
  // 2^pow philo, e.g. pow = 3 -> 8 philo
  int n ;
  //  print_usage();
  if (argc > 1) {
    n = atoi(argv[1]);
  } else {
    n = 5;
  }

  // create a model to hold net types.
  IPNModel model ;

  buildKanbanType(model,n);

  model.setInstance(typeName("Kanban",n),typeName("Kanban",n));
  
  
  SDD M0 = model.getInitialState();


  if (false && n <= 3) {
    std::cout << model << std::endl;
    std::cout << M0 << std::endl ;
  }


  MemoryManager::garbage();
  SDD Mfinal = model.computeReachable ();

  Statistic S = Statistic(Mfinal,model.getInstance()->getName(),CSV);  

  exportDot(Mfinal,"kanban");

  std::ofstream out ("stats.tex",std::ios_base::app) ; 

  S.print_line(out);
  S.print_header(std::cout);
  S.print_line(std::cout);

}
  
