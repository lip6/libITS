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
std::string typeName (const string & name,int n) {
  return name + toString(n);
}

// name for types : Robin3 represents 3 elements
std::string ringName (int n) {
  return typeName("Ring",n);
}

// name for instances : Robin3 represents 3 elements
std::string instanceName (int n) {
  return typeName("i",n);
}


void buildUnitRingType (IPNModel & m, int nbPoles=3) {
  UnitNet * res = new UnitNet("UnitRing") ;

  for (int i=0; i < nbPoles; ++i) {
    res->addPlace (typeName("pole",i));
  }

  res->openStateDef("default");
  res->updateStateDef(typeName("pole",0),1);
  res->closeStateDef();

  string tname;
  for (int i=0; i < nbPoles; ++i) {
    for (int j=i+1; j < nbPoles; ++j) {
      // Move the ring from pole i to j
      string ij = toString(i) + "," + toString(j);
      tname = "move" + ij;
      res->addTransition(tname, PUBLIC);
      res->addArc (typeName("pole",i),tname,1,INPUT);
      res->addArc (typeName("pole",j),tname,1,OUTPUT);

      // Move the ring from pole j to i
      string ji = toString(j) + "," + toString(i);
      tname = "move" + ji;
      res->addTransition(tname, PUBLIC);
      res->addArc (typeName("pole",j),tname,1,INPUT);
      res->addArc (typeName("pole",i),tname,1,OUTPUT);

      // test absence of ring on i and j
      tname = "test" + ij;
      res->addTransition(tname, PUBLIC);
      res->addArc (typeName("pole",i),tname,1,INHIBITOR);
      res->addArc (typeName("pole",j),tname,1,INHIBITOR);

    }
  }

  m.addType(res);
}


void buildRingType (IPNModel & m, int nbPoles=3) {
  CompositeNet * net = new CompositeNet(ringName(1));

  string inst = "ring";
  net->addInstance(inst,"UnitRing",m);

  string tname;
  for (int i=0; i < nbPoles; ++i) {
    for (int j=i+1; j < nbPoles; ++j) {
      // Move the ring from pole i to j
      string ij = toString(i) + "," + toString(j);
      string ji = toString(j) + "," + toString(i);
      tname = "move" ;
      // OR sync
      net->addSynchronization(tname+ij, PUBLIC,false);
      net->addSyncPart(tname+ij,inst,tname+ij);
      net->addSyncPart(tname+ij,inst,tname+ji);

      // export test absence of ring on i and j
      tname = "test" + ij;
      net->addSynchronization(tname,PUBLIC);
      net->addSyncPart(tname,inst,tname);
    }
  }

  m.addType(net);
}


// build a component with (n * grain) units, internally connected
void buildNaryRingType (IPNModel & m, int n, int nbPoles, bool isFinal, int grain=1, int complement=0) {
  // 2^0 philo = 1 philo
  int N = n*grain + complement;
  std::cerr << "Building Nary composition " << ringName(N) << " as a (non circular) composition of " << n << " blocks of size " << grain  <<  " with complement =" << complement << std::endl ;

  // build internal composition, exports events of each instance + connections
  {
    CompositeNet *net = new CompositeNet (typeName("internalRing",N));

    net->addInstance(instanceName(n-1),ringName(grain+complement),m);
    for (int i=n-2; i >= 0; --i) {
      net->addInstance(instanceName(i),ringName(grain),m);
    }

    string tname;

    for (int i=0; i < nbPoles; ++i) {
      for (int j=i+1; j < nbPoles; ++j) {
	// Move the ring k from pole i to j or j to i
	string ij = toString(i) + "," + toString(j);
	for (int k=0; k <n ; k++) {
	  string ijk = ij + "," + toString(k);
	  tname = "move";
	  net->addSynchronization(tname+ijk, PUBLIC);
	  // iff i+1 to nth instance are not in conflict
	  for (int next = k+1 ; next < n ; ++next )
	    net->addSyncPart(tname+ijk,instanceName(next),"test"+ij);
	  // mov ith instance
	  net->addSyncPart(tname+ijk,instanceName(k),tname+ij);
	}

	// export test absence of ring on i and j
	tname = "test" + ij;
	net->addSynchronization(tname,PUBLIC);
	for (int k=0; k <n ; k++) {
	  net->addSyncPart(tname,instanceName(k),tname);
	}
      }
    }
    m.addType(net);
  }
  // internal composition built
  {
    CompositeNet * net =  new CompositeNet (typeName("Ring",N));

    net->addInstance("rings", typeName("internalRing",N),m);

    string tname;


    for (int i=0; i < nbPoles; ++i) {
      for (int j=i+1; j < nbPoles; ++j) {
	// Move the ring k from pole i to j or j to i
	string ij = toString(i) + "," + toString(j);
	tname = "move";
	// or synchro
	Visibility vis = ( isFinal ? PRIVATE : PUBLIC );
	net->addSynchronization(tname+ij, vis,false);
	for (int k=0; k <n ; k++) {
	  string ijk = ij + "," + toString(k);
	  net->addSyncPart(tname+ij,"rings",tname+ijk);
	}

	if (!isFinal) {
	  // export test absence of ring on i and j
	  tname = "test" + ij;
	  net->addSynchronization(tname, PUBLIC);
	  net->addSyncPart(tname,"rings",tname);
	}
      }
    }
    m.addType(net);
  }
}


void build2RingType (IPNModel & m,int n1, int n2, int nbPoles, bool isFinal) {
  // 2^0 philo = 1 philo
  std::cerr << "Building type " << ringName(n1+n2) << " as binary composition of "  << ringName(n1) << " and " << ringName(n2) << std::endl ;
  int grain,complement;
  if (n1 > n2) {
    grain = n2;
    complement = n1 - n2;
  } else {
    grain = n1;
    complement = n2 - n1;
  }
  buildNaryRingType (m,2,nbPoles,isFinal,grain,complement);
}

void buildBitWiseComposite (IPNModel & model,int grain, int nbPoles)  {
  int grainleft = grain ;
  int done = -1;
  bool isFinal;

  for (int i = 0 ; i < 32  && grainleft ; ++i ) {
	  if (i > 0) {
		  // skip 2 pow 0 as it is handled by buildRingType already
		  if ( 1 << i == grain)
			  isFinal =true;
		  else
			  isFinal = false;
		  build2RingType(model,1 << (i-1), 1 << (i-1), nbPoles, isFinal);
	  }
	  if (grainleft & 0x1) {
		  // a bit
		  if (done == -1)
			  // first non zero bit
			  done =  1 << i;
		  else {
			  // build a composite type
			  if ( (done + (1 << i) ) == grain)
				  isFinal = true;
			  build2RingType(model,done, 1 << i,nbPoles,isFinal);
			  done += 1 << i ;
		  }
	  }

    grainleft >>= 1;
  }
  assert (grain ==done);
}


enum Policy {NAIVE,REORDER,ANYDEST,ANYANY, ANYSENDER, DEPTH1, DEPTH1M1, RECFOLD, DEPTHREC, DEPTHPROG};

void print_usage() {
  std::cerr << " Client Server classical symmetric net (WN) example (see : Forte'05, Thierry-Mieg et al.). Encoded with SDD/DDD variants.\n"
	    << " Options : \n"
	    << " ./cliservIPN [NCLI=5] [NSERV=2] [OPTION=NAIVE,REORDER,ANYDEST,ANYANY, ANYSENDER] [GRAIN=1] \n"
	    << " where : NCLI,NSERV is model size in number of respectively clients and servers, \n"
	    << " \t GRAIN is size of small blocks assembled (only used for some strategies)\n"
	    << " \t DEPTH1 is similar to usual modular PN encoding, with N/GRAIN components assembled. If grain is not a multiple of N, the closest approximation is used. \n"
	    << " \t DEPTH1M1 is  built by adding one element at top and bottom in layered fashion (grain unused) \n"
	    << " \t RECFOLD is  built by using philo type bitwise recursive decomposition of n (grain unused) \n"
	    << " \t DEPTHPROG is  built by grain blocks decomposed into grain/2 blocks etc...  \n"
	    << " \t DEPTHREC is  built by using (more or less) grain variables at each level \n";
}

int main (int argc, char **argv) {
  // 2^pow philo, e.g. pow = 3 -> 8 philo
  int nbRing,nbPoles ;
  //  print_usage();
  if (argc > 1) {
    nbRing = atoi(argv[1]);
  } else {
    nbRing = 5;
  }
  if (argc > 2) {
    nbPoles = atoi(argv[2]);
  } else {
    nbPoles = 3;
  }

  Policy p = RECFOLD;
  int argi = 3;
  if (argc > argi) {
    if (! strcmp (argv[argi],"DEPTH1"))
      p = DEPTH1;
    else if (! strcmp (argv[argi],"NAIVE"))
      p = NAIVE;
    else if (! strcmp (argv[argi],"ANYDEST"))
      p = ANYDEST;
    else if (! strcmp (argv[argi],"ANYANY"))
      p = ANYANY;
    else if (! strcmp (argv[argi],"ANYSENDER"))
      p = ANYSENDER;
    else if (! strcmp (argv[argi],"RECFOLD"))
      p = RECFOLD;
/*    else if (! strcmp (argv[argi],"DEPTHPROG"))
      p = DEPTHPROG;
 */
    else {
      printf ("bad strategy name ! %s",argv[argi]);
      print_usage();
      exit(1);
    }
  }

  int grain;
  if (argc > 4) {
    grain = atoi(argv[4]);
  } else {
    grain = 1;
  }

  // create a model to hold net types.
  IPNModel model ;
  buildUnitRingType(model,nbPoles);
  buildRingType (model,nbPoles);


  string strat ;

  if (p == DEPTH1) {
     if (grain > 1)
       buildNaryRingType (model, grain, nbPoles, false, 1 , 0);
     if (  nbRing % grain != 0)
       if ( nbRing / grain > 1 )
	 buildNaryRingType (model, grain + (nbRing % grain), nbPoles, false );
       else
	 buildNaryRingType (model, nbRing % grain, nbPoles, false);

     if ( nbRing / grain > 1 )
       buildNaryRingType(model,nbRing/grain,nbPoles, true, grain, nbRing%grain);
     else
       build2RingType (model,grain, nbRing%grain,nbPoles,true);
     std::cerr << " DEPTH1 : n=" << nbRing << " grain="<<grain << " n/grain=" << nbRing/grain << " blocks " << std::endl;
     model.setInstance(ringName(nbRing),"singleD"+toString(grain)+  "hanoi"+toString(nbRing));
  } else if (p== RECFOLD) {
    buildBitWiseComposite (model,nbRing,nbPoles);
    model.setInstance(ringName(nbRing),"binaryHanoi"+toString(nbRing)+"."+toString(nbPoles));
  }

  if (nbRing <= 3)
    std::cout << model << std::endl ;


  // State manipulations tests

  SDD M0 = model.getInstance()->getSDDState(0);
  Shom trans = model.getInstance()->getSDDTransRel (0);

  if (nbRing <= 3) {
    std::cout << M0 << std::endl ;
    Shom::pstats();
    Hom::pstats();
  }

  SDD Mfinal = fixpoint(trans+GShom::id) (M0);
  std::cout << "coucou"<< std::endl;
  Statistic S = Statistic(Mfinal,model.getInstance()->getName(),CSV);


  if (nbRing <= 3) {
    Shom::pstats();
    Hom::pstats();
  }


  std::ofstream out ("stats.tex",std::ios_base::app) ;

  S.print_line(out);
  S.print_header(std::cout);
  S.print_line(std::cout);

//  exportDot(Mfinal,"cliServ"+toString(nc)+"_"+toString(ns)+strat);
}
