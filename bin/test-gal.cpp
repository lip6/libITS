#include <iostream>
#include <fstream>
#include <cstring>

#include <cstdlib>

// The ITS model referential
#include "ITSModel.hh"
#include "SDD.h"
#include "MemoryManager.h"

// SDD utilities to output stats and dot graphs
#include "util/dotExporter.h"
#include "statistic.hpp"


#include "TypeCacher.hh"
#include "gal/GALType.hh"

#include "philo.hh"


using namespace its;


int main (int argc, char **argv) {


  // build a philo GAL
  // should be : GALLoader.createGAL("philo.so")
  const GAL * philo = createGAL();

  std::cout << *philo << std::endl;


  // build a GALType on top
   pType type = new GALType(philo);
   pType philType = new TypeCacher(const_cast<Type*>(type));

//   // grab locals
   Transition locals = philType->getLocals();

//   // grab initial state
   State init = philType->getState("init");

//   // gen reachable
   Transition fix = fixpoint ( locals + Transition::id);
   State reach = fix(init);

//   // print stats
   Statistic stat (reach,"reach");
   stat.print_table(std::cout);



 return 0;
}

