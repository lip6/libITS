#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>

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
// name for types : Ring3 represents 3 elements
std::string ringName (int n) {
  return "Ring" + toString(n);
}
// the i-th instance of a component is r(i-1)
std::string instanceName (int n) {
  return "r" + toString(n);
}


void buildRingType (IPNModel & m) {
  UnitNet * ring = new UnitNet(ringName(1)) ;

  std::cerr << "Building unit ring type " << std::endl;
  
  ring->addPlace ("C");
  ring->addPlace ("E");
  ring->addPlace ("A");
  ring->addPlace ("B");
  ring->addPlace ("D");
  ring->addPlace ("F");
  ring->addPlace ("G");
  ring->addPlace ("H");

  ring->openStateDef ("default");
  ring->updateStateDef("C",1);
  ring->updateStateDef("A",1);
  ring->closeStateDef();
  
  ring->addTransition("Get", PRIVATE);
  ring->addArc ("F","Get",1,INPUT);
  ring->addArc ("B","Get",1,INPUT);
  ring->addArc ("C","Get",1,OUTPUT);
  ring->addArc ("A","Get",1,OUTPUT);

  ring->addTransition("Put", PRIVATE);
  ring->addArc ("B","Put",1,INPUT);
  ring->addArc ("G","Put",1,INPUT);
  ring->addArc ("C","Put",1,OUTPUT);
  ring->addArc ("H","Put",1,OUTPUT);

  ring->addTransition("Other", PRIVATE);
  ring->addArc ("D","Other",1,INPUT);
  ring->addArc ("G","Other",1,OUTPUT);

  ring->addTransition("Owner", PRIVATE);
  ring->addArc ("D","Owner",1,INPUT);
  ring->addArc ("E","Owner",1,OUTPUT);

  ring->addTransition("GoOn", PRIVATE);
  ring->addArc ("E","GoOn",1,INPUT);
  ring->addArc ("F","GoOn",1,OUTPUT);

  ring->addTransition("Write", PRIVATE);
  ring->addArc ("E","Write",1,INPUT);
  ring->addArc ("G","Write",1,OUTPUT);

  ring->addTransition("FreeLocal", PUBLIC);
  ring->addArc ("A","FreeLocal",1,INPUT);
  ring->addArc ("B","FreeLocal",1,OUTPUT);

  ring->addTransition("FreeNext", PUBLIC);
  ring->addArc ("C","FreeNext",1,INPUT);
  ring->addArc ("E","FreeNext",1,OUTPUT);
  
  ring->addTransition("UsedLocal", PUBLIC);
  ring->addArc ("H","UsedLocal",1,INPUT);
  ring->addArc ("B","UsedLocal",1,OUTPUT);

  ring->addTransition("UsedNext", PUBLIC);
  ring->addArc ("C","UsedNext",1,INPUT);
  ring->addArc ("D","UsedNext",1,OUTPUT);

  m.addType (ring);
}

// in a context with n internal instances, connect 0 to 1, 1 to 2 etc.. n-2 to n-1 with private syncs.
void addInternalTransitions (CompositeNet * nring, int n) {
  std::cerr << "Augmenting Nary n=" << n << " composition " << nring->getName() << " with internal private transitions." << std::endl ;
  for (int i=0; i< n-1; ++i) {
    int next = i + 1  ;
    nring->addSynchronization("Free"+toString(i),PRIVATE);
    nring->addSyncPart("Free"+toString(i),instanceName(i),"FreeLocal");
    nring->addSyncPart("Free"+toString(i),instanceName(next),"FreeNext");
    
    nring->addSynchronization("Used"+toString(i),PRIVATE);
    nring->addSyncPart("Used"+toString(i),instanceName(i),"UsedLocal");
    nring->addSyncPart("Used"+toString(i),instanceName(next),"UsedNext");
  }
}

// in a context with n internal instances, create public "hooks" on left of 0 and right of n-1.
void addPublicTransitions (CompositeNet * nring, int n) {
  std::cerr << "Augmenting Nary n=" << n << " composition " << nring->getName() << " with public visibility syncs at edges "  << std::endl ;

  // export transitions 
  nring->addSynchronization("FreeLocal",PUBLIC);
  nring->addSyncPart("FreeLocal",instanceName(n-1),"FreeLocal");

  nring->addSynchronization("FreeNext",PUBLIC);
  nring->addSyncPart("FreeNext",instanceName(0),"FreeNext");
  
  nring->addSynchronization("UsedLocal",PUBLIC);
  nring->addSyncPart("UsedLocal",instanceName(n-1),"UsedLocal");

  nring->addSynchronization("UsedNext",PUBLIC);
  nring->addSyncPart("UsedNext",instanceName(0),"UsedNext");  
}

// in a context with n internal instances, connect n-1 to 0 with private syncs.
void addCircularTransitions (CompositeNet * nring, int n) {

    std::cerr << "Augmenting Nary n=" << n << " composition " << nring->getName() << " with circular private syncs"  << std::endl ;
  // close the loop
   nring->addSynchronization("Free"+toString(n-1),PRIVATE);
   nring->addSyncPart("Free"+toString(n-1),instanceName(n-1),"FreeLocal");
   nring->addSyncPart("Free"+toString(n-1),instanceName(0),"FreeNext");
  
   nring->addSynchronization("Used"+toString(n-1),PRIVATE);
   nring->addSyncPart("Used"+toString(n-1),instanceName(n-1),"UsedLocal");
   nring->addSyncPart("Used"+toString(n-1),instanceName(0),"UsedNext");  
  
}


// build a component with (n * grain) units, internally connected, no circularity
CompositeNet * buildNaryRingType (IPNModel & m, int n, int grain=1, int complement=0) {
  // 2^0 philo = 1 philo
  int N = n*grain + complement;
  std::cerr << "Building Nary composition " << ringName(N) << " as a (non circular) composition of " << n << " blocks of size " << grain  <<  " with complement =" << complement << std::endl ;
  CompositeNet *nring = new CompositeNet (ringName(N));

  nring->addInstance(instanceName(0),ringName(grain+complement),m);
  for (int i=1; i< n; ++i) {
    nring->addInstance(instanceName(i),ringName(grain),m);
  }
  addInternalTransitions(nring,n);

  return nring;
}


// create a loop that uses a buildNaryType strategy (without hierarchy)
void buildCircularRingType (IPNModel & m,int n, int grain=1, int complement=0) {
  // 2^0 philo = 1 philo
  CompositeNet *nring = buildNaryRingType (m,n,grain,complement);

  addCircularTransitions(nring,n);

  m.addType (nring);
}

// build a component with (n * grain) units, internally connected, no circularity
// with additionally public events for synchro at each edge
void buildGrainRingType (IPNModel & m,int n, int grain=1) {
  // 2^0 philo = 1 philo  
  CompositeNet *nring = buildNaryRingType (m,n,grain);

  addPublicTransitions (nring,n);

  m.addType (nring);
}

void build2RingType (IPNModel & m,int n1, int n2, bool isCircular = false) {
  // 2^0 philo = 1 philo
  std::cerr << "Building type " << ringName(n1+n2) << " as binary composition of "  << ringName(n1) << " and " << ringName(n2) << std::endl ;
  CompositeNet *nring = new CompositeNet (ringName(n1+n2));
  
  nring->addInstance(instanceName(0),ringName(n1),m);
  nring->addInstance(instanceName(1),ringName(n2),m);
 
  addInternalTransitions(nring,2); 
  if (! isCircular)
    addPublicTransitions (nring,2);
  else
    addCircularTransitions (nring,2);
  
  m.addType (nring);
}


void buildRingNPlus2 (IPNModel & m, int n) {
  CompositeNet *plus2 = new CompositeNet (ringName(n+2));
  std::cerr << "Building type " << ringName(n+2) << " as a ternary composition of "  << ringName(1) << ", " << ringName(n) << " and " << ringName(1) << std::endl ;  

  plus2->addInstance(instanceName(0),ringName(1),m);
  plus2->addInstance(instanceName(1),ringName(n),m);
  plus2->addInstance(instanceName(2),ringName(1),m);
  
  addInternalTransitions(plus2,3);
  addPublicTransitions (plus2,3);

  m.addType(plus2);
}


void closeRing (IPNModel & m,int n) {
  CompositeNet *loop = new CompositeNet ("Loop");
  
  std::cerr << "Building type " << "Loop" << " as a circular pair of synchros over a single component " << ringName(n) << std::endl ;  

  loop->addInstance(instanceName(0),ringName(n),m);
  addCircularTransitions(loop,1);

  m.addType (loop);
}

void buildBitWiseComposite (IPNModel & model,int grain)  {
  int grainleft = grain ;
  int done = -1;
  for (int i =0; i < 32  && grainleft ; ++i ) {
    if (i > 0)
      // skip 2 pow 0 as it is handled by buildRingType already
      build2RingType(model,1 << (i-1), 1 << (i-1));
    if (grainleft & 0x1) {
      // a bit
      if (done == -1)
	// first non zero bit
	done =  1 << i;
      else {
	// build a composite type
	build2RingType(model,done, 1 << i);
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
  if (model.findType(ringName(n)) != NULL)
    return ;
  else if (n < grain || grain == 1) {
    // use grain 1 for final level
      buildGrainRingType (model, n);
    return ;
  } else if (part > 1)
    buildNWiseComposite (model, part, grain * factor);

  if (rem != 0)
    buildNWiseComposite (model, part + rem, grain * factor);

  std::cerr << "Building a composite for n=" << n << " with grain="<<grain  << " blocks " 
	    << " of sizes " << (grain-1) << "*" << part << ".." << part + rem << std :: endl ;
  CompositeNet *nring = buildNaryRingType (model,grain,part,rem);
  
  addPublicTransitions(nring,grain);

  model.addType (nring);
}


enum Policy {DEPTH1, DEPTH1M1, RECFOLD, DEPTHREC, DEPTHPROG};

void print_usage() {
  std::cerr << " Ciardo's Slotted Ring protocol (see ICATPN'99). Encoded with SDD/DDD variants.\n"
	    << " Options : \n"
	    << " ./ringIPN [N=5]  [OPTION=DEPTH1,DEPTHN...] [GRAIN=1] \n"
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
  
  buildRingType (model);
  
  if (p == DEPTHREC) {
    buildNWiseComposite (model,n,grain);
    closeRing(model,n);
    std::cerr << " DEPTHREC " << std::endl;     
    model.setInstance("Loop","rec" + toString(grain) +"ring" +toString(n));
  } else if (p == DEPTHPROG) {
    double factor = 2;
    buildNWiseComposite (model,n,grain,factor);
    closeRing(model,n);
    std::cerr << " DEPTHPROG " << std::endl;     
    model.setInstance("Loop", toString(factor) + "prog" + toString(grain) +"ring" +toString(n));
  } else if (p == DEPTH1) {
    if (grain > 1)
      buildGrainRingType (model, grain);
    if (  n % grain != 0)
      if ( n / grain > 1 )
	buildGrainRingType (model, grain + (n % grain) );
      else 
	buildGrainRingType (model, n % grain );
    
    if ( n / grain > 1 )
      buildCircularRingType(model,n/grain,grain, n%grain);
    else 
      build2RingType (model,grain, n%grain,true);
    std::cerr << " DEPTH1 : n=" << n << " grain="<<grain << " n/grain=" << n/grain << " blocks " << std::endl;
    model.setInstance(ringName(n),"singleD"+toString(grain)+  "ring"+toString(n));
  } else if (p== RECFOLD) {
    buildBitWiseComposite (model,n);
    closeRing(model,n);
    model.setInstance("Loop","binaryRing"+toString(n));
  } else if (p == DEPTH1M1) {
    int small = n % 2;
    if (small == 0) {
      // build ring 2 containing 2 ring 1
      buildGrainRingType (model, 2);
      for (int i = 2 ; i + small < n ; i += 2) {
//	std::cout << " i == " <<i << std::endl;
	buildRingNPlus2(model,i+small);      
      }
    } else {
      // else, happy with default ring 1 as center brick
      for (int i = 0 ; i + small < n ; i += 2) {
//	std::cout << " i == " <<i << std::endl;
	buildRingNPlus2(model,i+small);      
      }
    }
    closeRing(model,n);
    model.setInstance("Loop","1N1ring"+toString(n));
  }
  if (n <= 3)
    std::cout << model << std::endl ;


  // State manipulations tests

  SDD M0 = model.getInstance()->getSDDState(0);
  Shom trans = model.getInstance()->getSDDTransRel (0);
  
  if (n <= 3)
    std::cout << M0 << std::endl ;

  SDD Mfinal = fixpoint(trans+GShom::id) (M0);
  Statistic S = Statistic(Mfinal,model.getInstance()->getName(),CSV);  

  std::ofstream out ("stats.tex",std::ios_base::app) ; 
  S.print_line(out);
  S.print_line(std::cout);
}
