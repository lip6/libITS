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
// name for types : Robin3 represents 3 elements
std::string robinName (int n) {
  return "Robin" + toString(n);
}
// the i-th instance of a component is r(i-1)
std::string instanceName (int n) {
  return "r" + toString(n);
}

void buildResourceType (IPNModel & m) {
 UnitNet * res = new UnitNet("Resource") ;
 
 res->addPlace ("Res");

 res->openStateDef("default");
 res->updateStateDef("Res",1);
 res->closeStateDef();

 string tname;
 tname = "get";
 res->addTransition(tname, PUBLIC);
 res->addArc ("Res",tname,1,INPUT);

 tname = "return";
 res->addTransition(tname, PUBLIC);
 res->addArc ("Res",tname,1,OUTPUT);

 m.addType(res);
}

void buildRobinType (IPNModel & m, bool withOR= true) {
  UnitNet * net = new UnitNet(robinName(1)) ;

  std::cerr << "Building unit robin type " << std::endl;
  
  net->addPlace ("R");
  net->addPlace ("bufidle");
  net->addPlace ("buffull");
  net->addPlace ("pwait");
  net->addPlace ("pask");
  net->addPlace ("pok");
  net->addPlace ("pload");
  net->addPlace ("psend");

  net->openStateDef ("default");
  net->updateStateDef("bufidle",1);
  net->updateStateDef("pwait",1);
  net->closeStateDef();

  net->openStateDef ("first");
  net->updateStateDef("bufidle",1);
  net->updateStateDef("pask",1);
  net->closeStateDef();
  
  string tname;
  if (withOR)
    tname = "task";
  else 
    tname = "task0";
  net->addTransition(tname, PUBLIC);
  net->addArc ("pask",tname,1,INPUT);
  net->addArc ("R",tname,1,OUTPUT);
  net->addArc ("pok",tname,1,OUTPUT);

  if (withOR)
    tname = "tbuf";
  else 
    tname = "tbuf0";
  net->addTransition(tname, PUBLIC);
  net->addArc ("R",tname,1,INPUT);
  net->addArc ("bufidle",tname,1,INPUT);
  net->addArc ("buffull",tname,1,OUTPUT);

  tname = "t1load";
  net->addTransition(tname, PRIVATE);
  net->addArc ("buffull",tname,1,INPUT);
  net->addArc ("pok",tname,1,INPUT);
  net->addArc ("bufidle",tname,1,OUTPUT);
  net->addArc ("psend",tname,1,OUTPUT);

  tname = "t2load";
  net->addTransition(tname, PRIVATE);
  net->addArc ("buffull",tname,1,INPUT);
  net->addArc ("pload",tname,1,INPUT);
  net->addArc ("bufidle",tname,1,OUTPUT);
  net->addArc ("pwait",tname,1,OUTPUT);

  tname = "t1sendLocal";
  net->addTransition(tname, PUBLIC);
  net->addArc ("pok",tname,1,INPUT);
  net->addArc ("pload",tname,1,OUTPUT);

  tname = "t2sendLocal";
  net->addTransition(tname, PUBLIC);
  net->addArc ("psend",tname,1,INPUT);
  net->addArc ("pwait",tname,1,OUTPUT);

  tname = "sendNext";
  net->addTransition(tname, PUBLIC);
  net->addArc ("pwait",tname,1,INPUT);
  net->addArc ("pask",tname,1,OUTPUT);

  m.addType (net);
}

void buildResourceRobin (IPNModel & m, const std::string & robinType, int n=0, bool withOR=true) {

  CompositeNet *net = new CompositeNet ("ResourceRobin");
  
  net->addInstance("robins",robinType,m);
  net->addInstance("res","Resource",m);

  net->openStateDef("default");
  net->updateStateDef("robins","first");
  net->closeStateDef();

  if (withOR) {
    string tname = "tbuf";
    net->addSynchronization(tname,PRIVATE);
    net->addSyncPart(tname,"robins","tbuf");
    net->addSyncPart(tname,"res","return");

    tname = "task";
    net->addSynchronization(tname,PRIVATE);
    net->addSyncPart(tname,"robins","task");
    net->addSyncPart(tname,"res","get");  
  } else {
    for (int i=0; i < n ; ++i) {
      string tname = "tbuf"+toString(i);
      net->addSynchronization(tname,PRIVATE);
      net->addSyncPart(tname,"robins",tname);
      net->addSyncPart(tname,"res","return");
      
      tname = "task"+toString(i);
      net->addSynchronization(tname,PRIVATE);
      net->addSyncPart(tname,"robins",tname);
      net->addSyncPart(tname,"res","get");  
    }
  }

  m.addType (net);
}

// in a context with n internal instances, connect 0 to 1, 1 to 2 etc.. n-2 to n-1 with private syncs.
void addInternalTransitions (CompositeNet * net, int n) {
  std::cerr << "Augmenting Nary n=" << n << " composition " << net->getName() << " with internal private transitions." << std::endl ;
  string tname;
  for (int i=0; i< n-1; ++i) {
    int next = i + 1  ;
    
    tname = "t1send"+toString(i);
    net->addSynchronization(tname,PRIVATE);
    net->addSyncPart(tname,instanceName(i),"t1sendLocal");
    net->addSyncPart(tname,instanceName(next),"sendNext");

    tname = "t2send"+toString(i);
    net->addSynchronization(tname,PRIVATE);
    net->addSyncPart(tname,instanceName(i),"t2sendLocal");
    net->addSyncPart(tname,instanceName(next),"sendNext");
  }
}


void addORTransitions (CompositeNet * net, int n) {
  // add OR synchro
  string tname = "task";
  // isAND = false !!!
  net->addSynchronization(tname,PUBLIC,false);
  for (int i= 0; i <n ; ++i)
    net->addSyncPart(tname,instanceName(i),tname);

  tname = "tbuf";
  // isAND = false !!!
  net->addSynchronization(tname,PUBLIC,false);
  for (int i= 0; i <n ; ++i)
    net->addSyncPart(tname,instanceName(i),tname);
}

// in a context with n internal instances, create public "hooks" on left of 0 and right of n-1.
void addPublicTransitions (CompositeNet * net, int n, bool withOR) {
  std::cerr << "Augmenting Nary n=" << n << " composition " << net->getName() << " with public visibility syncs at edges "  << std::endl ;

  // export transitions 
  string tname;
  tname = "t1sendLocal";
  net->addSynchronization(tname,PUBLIC);
  net->addSyncPart(tname,instanceName(n-1),tname);

  tname = "t2sendLocal";
  net->addSynchronization(tname,PUBLIC);
  net->addSyncPart(tname,instanceName(n-1),tname);

  tname = "sendNext";
  net->addSynchronization(tname,PUBLIC);
  net->addSyncPart(tname,instanceName(0),tname);

  if (withOR)
    addORTransitions(net,n);

}


// in a context with n internal instances, connect n-1 to 0 with private syncs.
void addCircularTransitions (CompositeNet * net, int n, bool withOR=true) {

    std::cerr << "Augmenting Nary n=" << n << " composition " << net->getName() << " with circular private syncs"  << std::endl ;
    string tname;
    tname = "t1send"+toString(n-1);
    net->addSynchronization(tname,PRIVATE);
    net->addSyncPart(tname,instanceName(n-1),"t1sendLocal");
    net->addSyncPart(tname,instanceName(0),"sendNext");

    tname = "t2send"+toString(n-1);
    net->addSynchronization(tname,PRIVATE);
    net->addSyncPart(tname,instanceName(n-1),"t2sendLocal");
    net->addSyncPart(tname,instanceName(0),"sendNext");

    if (withOR)
      addORTransitions (net,n);
}


// build a component with (n * grain) units, internally connected, no circularity
CompositeNet * buildNaryRobinType (IPNModel & m, int n, int grain=1, int complement=0, bool withOR=true) {
  // 2^0 philo = 1 philo
  int N = n*grain + complement;
  std::cerr << "Building Nary composition " << robinName(N) << " as a (non circular) composition of " << n << " blocks of size " << grain  <<  " with complement =" << complement << " and XOR activated=" << withOR << std::endl ;
  CompositeNet *net = new CompositeNet (robinName(N));

  for (int i=0 ; i < complement ; ++i) {
    net->addInstance(instanceName(i),robinName(grain+1),m);
  }
  for (int i=complement; i< n; ++i) {
    net->addInstance(instanceName(i),robinName(grain),m);
  }
  
  net->openStateDef("first");
  net->updateStateDef(instanceName(0),"first");
  net->closeStateDef();

  addInternalTransitions(net,n);

  if (! withOR ) {
    std::cerr << "Augmenting nary composition with "<< n*grain + complement <<  " public visibility sync transitions " << std::endl;
    int ntr = 0;
    for (int i=0 ; i < complement ; ++i) {
      for (int j= 0; j < grain+1 ; ++j) {
	string tname = "task"+toString(ntr);
	net->addSynchronization(tname,PUBLIC);
	net->addSyncPart(tname,instanceName(i),"task"+toString(j));
      
	tname = "tbuf"+toString(ntr);
	net->addSynchronization(tname,PUBLIC);
	net->addSyncPart(tname,instanceName(i),"tbuf"+toString(j));
	
	ntr++;
      }
    }
    for (int i=complement ; i < n ; ++i) {
      for (int j= 0; j < grain ; ++j) {
	string tname = "task"+toString(ntr);
	net->addSynchronization(tname,PUBLIC);
	net->addSyncPart(tname,instanceName(i),"task"+toString(j));
      
	tname = "tbuf"+toString(ntr);
	net->addSynchronization(tname,PUBLIC);
	net->addSyncPart(tname,instanceName(i),"tbuf"+toString(j));
	
	ntr++;
      }
    }
  }

  return net;
}


// create a loop that uses a buildNaryType strategy (without hierarchy)
void buildCircularRobinType (IPNModel & m,int n, int grain=1, int complement=0, bool withOR=true) {
  // 2^0 philo = 1 philo
  CompositeNet *net = buildNaryRobinType (m,n,grain,complement,withOR);

  addCircularTransitions(net,n,withOR);

  m.addType (net);
}

// build a component with (n * grain) units, internally connected, no circularity
// with additionally public events for synchro at each edge
void buildGrainRobinType (IPNModel & m,int n, int grain=1, bool withOR=true) {
  // 2^0 philo = 1 philo  
  CompositeNet *net = buildNaryRobinType (m,n,grain,0,withOR);

  addPublicTransitions (net,n,withOR);

  m.addType (net);
}

void build2RobinType (IPNModel & m,int n1, int n2, bool isCircular = false, bool withOR=true) {
  // 2^0 philo = 1 philo
  std::cerr << "Building type " << robinName(n1+n2) << " as binary composition of "  << robinName(n1) << " and " << robinName(n2) << std::endl ;
  CompositeNet *net = new CompositeNet (robinName(n1+n2));
  
  net->addInstance(instanceName(0),robinName(n1),m);
  net->addInstance(instanceName(1),robinName(n2),m);

  net->openStateDef("first");
  net->updateStateDef(instanceName(0),"first");
  net->closeStateDef();
 
  addInternalTransitions(net,2); 
  if (! isCircular)
    addPublicTransitions (net,2,withOR);
  else
    addCircularTransitions (net,2,withOR);
  
  m.addType (net);
}


void buildRobinNPlus2 (IPNModel & m, int n, bool withOR =true) {
  CompositeNet *plus2 = new CompositeNet (robinName(n+2));
  std::cerr << "Building type " << robinName(n+2) << " as a ternary composition of "  << robinName(1) << ", " << robinName(n) << " and " << robinName(1) << std::endl ;  

  plus2->addInstance(instanceName(0),robinName(1),m);
  plus2->addInstance(instanceName(1),robinName(n),m);
  plus2->addInstance(instanceName(2),robinName(1),m);

  plus2->openStateDef("first");
  plus2->updateStateDef(instanceName(0),"first");
  plus2->closeStateDef();
  

  addInternalTransitions(plus2,3);
  addPublicTransitions (plus2,3,withOR);

  m.addType(plus2);
}


void closeRobin (IPNModel & m,int n, bool withOR=true) {
  CompositeNet *net = new CompositeNet ("Loop");
  
  std::cerr << "Building type " << "Loop" << " as a circular pair of synchros over a single component " << robinName(n) << std::endl ;  

  net->addInstance(instanceName(0),robinName(n),m);
  net->openStateDef("first");
  net->updateStateDef(instanceName(0),"first");
  net->closeStateDef();

  addCircularTransitions(net,1,withOR);
  if (!withOR) {
    std::cerr << "Exporting n transitions for the resource" << std::endl;
    for (int i=0 ; i < n ; ++i) {
      string tname = "task"+toString(i);
      net->addSynchronization(tname,PUBLIC);
      net->addSyncPart(tname,instanceName(0),"task"+toString(i));
      
      tname = "tbuf"+toString(i);
      net->addSynchronization(tname,PUBLIC);
      net->addSyncPart(tname,instanceName(0),"tbuf"+toString(i));
    }
  }

  m.addType (net);
}

void buildBitWiseComposite (IPNModel & model,int grain)  {
  int grainleft = grain ;
  int done = -1;
  for (int i =0; i < 32  && grainleft ; ++i ) {
    if (i > 0)
      // skip 2 pow 0 as it is handled by buildRobinType already
      build2RobinType(model,1 << (i-1), 1 << (i-1));
    if (grainleft & 0x1) {
      // a bit
      if (done == -1)
	// first non zero bit
	done =  1 << i;
      else {
	// build a composite type
	build2RobinType(model,done, 1 << i);
	done += 1 << i ;
      }	    
    }
    
    grainleft >>= 1;
  }
  assert (grain ==done);
}


void buildNWiseComposite (IPNModel & model,int n, int grain, double factor = 1.0, bool withOR=true)  {
  int part = n / grain;
  int rem = n % grain ;
  if (model.findType(robinName(n)) != NULL)
    return ;
  else if (n < grain || grain == 1) {
    // use grain 1 for final level
    buildGrainRobinType (model, n,1,withOR);
    return ;
  } else if (part > 1)
    buildNWiseComposite (model, part, grain * factor, factor, withOR);

  if (rem != 0)
    buildNWiseComposite (model, part + 1, grain * factor, factor, withOR);

  std::cerr << "Building a composite for n=" << n << " with grain="<<grain  << " blocks " 
	    << " of sizes " << (grain-1) << "*" << part << ".." << part + rem << std :: endl ;
  CompositeNet *net = buildNaryRobinType (model,grain,part,rem,withOR);
  
  addPublicTransitions(net,grain,withOR);

  model.addType (net);
}


enum Policy {DEPTH1, DEPTH1M1, RECFOLD, DEPTHREC, DEPTHPROG};

void print_usage() {
  std::cerr << " Ciardo's Slotted Robin protocol (see ICATPN'99). Encoded with SDD/DDD variants.\n"
	    << " Options : \n"
	    << " ./robinIPN [N=5]  [OPTION=DEPTH1,DEPTHN...] [GRAIN=1] \n"
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
  bool withOR = true;
  if (argc > 4) {
    withOR = false;
    printf("XOR transition mechanism disabled\n");
  }

  // create a model to hold net types.
  IPNModel model ;
  
  buildRobinType (model, withOR);
  buildResourceType (model);

  if (p == DEPTHREC) {
    buildNWiseComposite (model,n,grain,1.0,withOR);
    closeRobin(model,n,withOR);
    std::cerr << " DEPTHREC " << std::endl;     
    buildResourceRobin (model,"Loop",n,withOR);
    model.setInstance("ResourceRobin","rec" + toString(grain) +"robin" +toString(n));
  } else if (p == DEPTHPROG) {
    double factor = 2;
    buildNWiseComposite (model,n,grain,factor);
    closeRobin(model,n,withOR);
    std::cerr << " DEPTHPROG " << std::endl;     
    model.setInstance("Loop", toString(factor) + "prog" + toString(grain) +"robin" +toString(n));
  } else if (p == DEPTH1) {
    if (grain > 1)
      buildGrainRobinType (model, grain,1,withOR);
    if (  n % grain != 0)
      if ( n / grain > 1 )
	buildGrainRobinType (model, grain + 1 ,1 , withOR);
      else 
	buildGrainRobinType (model, n % grain , 1 , withOR);
    
    if ( n / grain > 1 )
      buildCircularRobinType(model,n/grain,grain, n%grain);
    else 
      build2RobinType (model,grain, n%grain,true);
    std::cerr << " DEPTH1 : n=" << n << " grain="<<grain << " n/grain=" << n/grain << " blocks " << std::endl;
    buildResourceRobin (model,robinName(n));
    model.setInstance("ResourceRobin","singleD"+toString(grain)+  "robin"+toString(n));
  } else if (p== RECFOLD) {
    buildBitWiseComposite (model,n);
    closeRobin(model,n,withOR);
    buildResourceRobin (model,"Loop");
    model.setInstance("ResourceRobin","binaryRobin"+toString(n));
  } else if (p == DEPTH1M1) {
    int small = n % 2;
    if (small == 0) {
      // build robin 2 containing 2 robin 1
      buildGrainRobinType (model, 2, 1, withOR);
      for (int i = 2 ; i + small < n ; i += 2) {
//	std::cout << " i == " <<i << std::endl;
	buildRobinNPlus2(model,i+small);      
      }
    } else {
      // else, happy with default robin 1 as center brick
      for (int i = 0 ; i + small < n ; i += 2) {
//	std::cout << " i == " <<i << std::endl;
	buildRobinNPlus2(model,i+small);      
      }
    }
    closeRobin(model,n,withOR);
    buildResourceRobin (model,"Loop");
    model.setInstance("ResourceRobin","1N1robin"+toString(n));
  }
  if (n <= 3)
    std::cout << model << std::endl ;


  // State manipulations tests

  SDD M0 = model.getInstance()->getSDDState(0);
  Shom trans = model.getInstance()->getSDDTransRel (0);
  
  if (n <= 0) {
    std::cout << M0 << std::endl ;
    Shom::pstats();
    Hom::pstats();
  }

  SDD Mfinal = fixpoint(trans+GShom::id) (M0);
  Statistic S = Statistic(Mfinal,model.getInstance()->getName(),CSV);  


  if (n <= 0) {
    Shom::pstats();
    Hom::pstats();
  }


  std::ofstream out ("stats.tex",std::ios_base::app) ; 

  S.print_line(out);
  S.print_header(std::cout);
  S.print_line(std::cout);
//  exportDot(Mfinal,"robin");
  
}
