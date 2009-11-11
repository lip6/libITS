#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>

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
// name for types : Simple3 represents 3 elements
std::string simpleName (int n) {
  return "Simple" + toString(n);
}
// the i-th instance of a component is r(i-1)
std::string instanceName (int n) {
  return "i" + toString(n);
}


void buildResourceType (IPNModel & m, int n=1) {
 UnitNet * res = new UnitNet("Resource") ;
 
 res->addPlace ("Res");

 res->openStateDef("default");
 res->updateStateDef("Res",n);
 res->closeStateDef();

 string tname;
 tname = "get";
 res->addTransition(tname, PUBLIC);
 res->addArc ("Res",tname,1,INPUT);

 tname = "put";
 res->addTransition(tname, PUBLIC);
 res->addArc ("Res",tname,1,OUTPUT);

 m.addType(res);
}


void buildSimpleType (IPNModel & m) {
  UnitNet * net = new UnitNet(simpleName(1)) ;
 
 net->addPlace ("a");
 net->addPlace ("b");
 net->addPlace ("c");

 net->openStateDef("default");
 net->updateStateDef("c",1);
 net->closeStateDef();

 string tname;
 tname = "t1";
 net->addTransition(tname, PUBLIC);
 net->addArc ("a",tname,1,INPUT);
 net->addArc ("b",tname,1,OUTPUT);

 tname = "t2";
 net->addTransition(tname, PUBLIC);
 net->addArc ("b",tname,1,INPUT);
 net->addArc ("c",tname,1,OUTPUT);

 tname = "t3";
 net->addTransition(tname, PRIVATE);
 net->addArc ("c",tname,1,INPUT);
 net->addArc ("a",tname,1,OUTPUT);

 m.addType(net);
}


void buildResourceSimple (IPNModel & m, const std::string & netType) {

  CompositeNet *nsimple = new CompositeNet ("ResourceSimple");
  
  nsimple->addInstance("simples",netType,m);
  nsimple->addInstance("res","Resource",m);

  string tname = "t1";
  nsimple->addSynchronization(tname,PRIVATE);
  nsimple->addSyncPart(tname,"simples","t1");
  nsimple->addSyncPart(tname,"res","get");

  tname = "t2";
  nsimple->addSynchronization(tname,PRIVATE);
  nsimple->addSyncPart(tname,"simples","t2");
  nsimple->addSyncPart(tname,"res","put");  

  m.addType (nsimple);
}

void addORTransitions (CompositeNet * nsimple, int n) {
  // add OR synchro
  string tname = "t1";
  // isAND = false !!!
  nsimple->addSynchronization(tname,PUBLIC,false);
  for (int i= 0; i <n ; ++i)
    nsimple->addSyncPart(tname,instanceName(i),tname);

  tname = "t2";
  // isAND = false !!!
  nsimple->addSynchronization(tname,PUBLIC,false);
  for (int i= 0; i <n ; ++i)
    nsimple->addSyncPart(tname,instanceName(i),tname);
}

// in a context with n internal instances, create public "hooks" on left of 0 and right of n-1.
void addPublicTransitions (CompositeNet * nsimple, int n) {
  std::cerr << "Augmenting Nary n=" << n << " composition " << nsimple->getName() << " with puiblic visibility OR transitions "  << std::endl ;

  // export OR transitions 
  addORTransitions(nsimple,n);

}

// build a component with (n * grain) units, internally connected, no circularity
CompositeNet * buildNarySimpleType (IPNModel & m, int n, int grain=1, int complement=0) {
  // 2^0 philo = 1 philo
  int N = n*grain + complement;
  std::cerr << "Building Nary composition " << simpleName(N) << " as a (non circular) composition of " << n << " blocks of size " << grain  <<  " with complement =" << complement << std::endl ;
  CompositeNet *nsimple = new CompositeNet (simpleName(N));

  nsimple->addInstance(instanceName(0),simpleName(grain+complement),m);
  for (int i=1; i< n; ++i) {
    nsimple->addInstance(instanceName(i),simpleName(grain),m);
  }
  

  return nsimple;
}

// build a component with (n * grain) units, internally connected, no circularity
// with additionally public events for synchro at each edge
void buildGrainSimpleType (IPNModel & m,int n, int grain=1,int complement = 0) {
  // 2^0 philo = 1 philo  
  CompositeNet *nsimple = buildNarySimpleType (m,n,grain,complement);

  addPublicTransitions (nsimple,n);

  m.addType (nsimple);
}

void build2SimpleType (IPNModel & m,int n1, int n2, bool isCircular = false) {
  // 2^0 philo = 1 philo
  std::cerr << "Building type " << simpleName(n1+n2) << " as binary composition of "  << simpleName(n1) << " and " << simpleName(n2) << std::endl ;
  CompositeNet *nsimple = new CompositeNet (simpleName(n1+n2));
  
  nsimple->addInstance(instanceName(0),simpleName(n1),m);
  nsimple->addInstance(instanceName(1),simpleName(n2),m);

  addPublicTransitions (nsimple,2);
  
  m.addType (nsimple);
}


void buildBitWiseComposite (IPNModel & model,int grain)  {
  int grainleft = grain ;
  int done = -1;
  for (int i =0; i < 32  && grainleft ; ++i ) {
    if (i > 0)
      // skip 2 pow 0 as it is handled by buildSimpleType already
      build2SimpleType(model,1 << (i-1), 1 << (i-1));
    if (grainleft & 0x1) {
      // a bit
      if (done == -1)
	// first non zero bit
	done =  1 << i;
      else {
	// build a composite type
	build2SimpleType(model,done, 1 << i);
	done += 1 << i ;
      }	    
    }
    
    grainleft >>= 1;
  }
  assert (grain ==done);
}


void buildNWiseComposite (IPNModel & model,int n, int grain, double factor = 1.0)  {
  int part = n / grain;
  int rem = n % grain ;
  if (model.findType(simpleName(n)) != NULL)
    return ;
  else if (n < grain || grain == 1) {
    // use grain 1 for final level
      buildGrainSimpleType (model, n);
    return ;
  } else if (part > 1)
    buildNWiseComposite (model, part, grain * factor);

  if (rem != 0)
    buildNWiseComposite (model, part + rem, grain * factor);

  std::cerr << "Building a composite for n=" << n << " with grain="<<grain  << " blocks " 
	    << " of sizes " << (grain-1) << "*" << part << ".." << part + rem << std :: endl ;
  CompositeNet *nsimple = buildNarySimpleType (model,grain,part,rem);
  
  addPublicTransitions(nsimple,grain);

  model.addType (nsimple);
}


enum Policy {DEPTH1, DEPTH1M1, RECFOLD, DEPTHREC, DEPTHPROG};

void print_usage() {
  std::cerr << " Ciardo's Slotted Simple protocol (see ICATPN'99). Encoded with SDD/DDD variants.\n"
	    << " Options : \n"
	    << " ./simpleIPN [N=5]  [OPTION=DEPTH1,DEPTHN...] [GRAIN=1] \n"
	    << " where : N is model size in number of participants, \n"
	    << " \t GRAIN is size of small blocks assembled (only used for some strategies)\n"
	    << " \t DEPTH1 is similar to usual modular PN encoding, with N/GRAIN components assembled. If grain is not a multiple of N, the closest approximation is used. \n"
	    << " \t DEPTH1M1 is  built by adding one element at top and bottom in layered fashion (grain unused) \n"
	    << " \t RECFOLD is  built by using philo type bitwise recursive decomposition of n (grain unused) \n"
	    << " \t DEPTHPROG is  built by grain blocks decomposed into grain/2 blocks etc...  \n"
	    << " \t DEPTHREC is  built by using (more or less) grain variables at each level \n";  
}

int main (int argc, char **argv) {
  // 2^pow philo, e.g. pow = 3 -> 8 philo
  int n ;
  print_usage();
  if (argc > 1) {
    n = atoi(argv[1]);
  } else {
    n = 5;
  }
  Policy p = RECFOLD; 
  if (argc > 2) {
    if (! strcmp (argv[2],"DEPTH1"))
      p = DEPTH1;
    else if (! strcmp (argv[2],"DEPTH1M1"))
      p = DEPTH1M1;
    else if (! strcmp (argv[2],"DEPTHREC"))
      p = DEPTHREC;
    else if (! strcmp (argv[2],"RECFOLD"))
      p = RECFOLD;
    else if (! strcmp (argv[2],"DEPTHPROG"))
      p = DEPTHPROG;
    else {
      printf ("bad strategy name ! %s",argv[2]);
      print_usage();
      exit(1);
    }
  }
  int grain;
  if (argc > 3) {
    grain = atoi(argv[3]); 
  } else {
    grain = 1;
  }

  // create a model to hold net types.
  IPNModel model ;
  
  buildSimpleType (model);
  buildResourceType (model,1);

  if (p == DEPTHREC) {
    buildNWiseComposite (model,n,grain);
    std::cerr << " DEPTHREC " << std::endl;     
    buildResourceSimple (model,simpleName(n));
    model.setInstance("ResourceSimple","rec" + toString(grain) +"simple" +toString(n));
  } else if (p == DEPTHPROG) {
    double factor = 2;
    buildNWiseComposite (model,n,grain,factor);
    std::cerr << " DEPTHPROG " << std::endl;     
    model.setInstance("Loop", toString(factor) + "prog" + toString(grain) +"simple" +toString(n));
  } else if (p == DEPTH1) {
    if (grain > 1)
      buildGrainSimpleType (model, grain);
    if (  n % grain != 0)
      if ( n / grain > 1 )
	buildGrainSimpleType (model, grain + (n % grain) );
      else 
	buildGrainSimpleType (model, n % grain );
    
    if ( n / grain > 1 ) 
      buildGrainSimpleType(model,n/grain,grain, n%grain);
    else 
      build2SimpleType (model,grain, n%grain,true);
    std::cerr << " DEPTH1 : n=" << n << " grain="<<grain << " n/grain=" << n/grain << " blocks " << std::endl;
    buildResourceSimple (model,simpleName(n));
    model.setInstance("ResourceSimple","singleD"+toString(grain)+  "simple"+toString(n));
  } else if (p== RECFOLD) {
    buildBitWiseComposite (model,n);
    buildResourceSimple (model,"Loop");
    model.setInstance("ResourceSimple","binarySimple"+toString(n));
  }

  if (n <= 3)
    std::cout << model << std::endl ;


  // State manipulations tests

  SDD M0 = model.getInstance()->getSDDState(0);
  Shom trans = model.getInstance()->getSDDTransRel (0);
  
  if (n <= 3) {
    std::cout << M0 << std::endl ;
    Shom::pstats();
    Hom::pstats();
  }

  SDD Mfinal = fixpoint(trans+GShom::id) (M0);
  Statistic S = Statistic(Mfinal,model.getInstance()->getName(),CSV);  


  if (n <= 3) {
    Shom::pstats();
    Hom::pstats();
  }


  std::ofstream out ("stats.tex",std::ios_base::app) ; 

  S.print_line(out);
  S.print_header(std::cout);
  S.print_line(std::cout);

  exportDot(Mfinal,"simple"+toString(n));  
}
