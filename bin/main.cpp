#include <iostream>
#include <fstream>
#include <cstring>

#include "Options.hh"
// The ITS model referential
#include "ITSModel.hh"
#include "SDD.h"
#include "MemoryManager.h"
// romeo parser
#include "petri/XMLLoader.hh"
// prod parser
#include "petri/Modular2ITS.hh"
// ITSModel parser
#include "ITSModelXMLLoader.hh"
// Cami parser
#include "petri/JSON2ITS.hh"
#include "parser_json/parse_json.hh"


// SDD utilities to output stats and dot graphs
#include "util/dotExporter.h"
#include "statistic.hpp"

using namespace its;

static bool beQuiet = false;
static string pathdotff = "final";
static string pathorderff = "order";
static bool dodotexport=false;
static bool dodumporder = false;
static bool with_garbage=true;
static std::string modelName = "";
// if BMC use is wanted, will be >0
static int BMC = -1;


void exhibitModel (ITSModel & model) {
	// pretty print the model for inspection
  if (! beQuiet) {
    std::cout << "Model in internal textual format :" << std::endl;
    std::cout << model << std::endl;
  
    //   std::cout << model.getInitialState() << std::endl;
  }
  if (dodumporder) {
    ofstream os (pathorderff.c_str());
    model.printVarOrder(os);
    os.close();
    exit(0);
  }
  State reachable;
  if (BMC <0) {
    // Compute reachable states
    reachable = model.computeReachable(with_garbage);
  } else {
    Transition hnext = model.getNextRel() + Transition::id;
    reachable = model.getInitialState();
    State previous = reachable;
    for (int i=0; i < BMC ; ++i) {
      std::cout << "At depth " << i << " NbStates=" << reachable.nbStates() << std::endl;
      reachable = hnext (reachable);
      if (previous == reachable) {
	std::cout << "Full state space explored, with depth of "<< i << std::endl;
	break;
      }
      previous = reachable;
      MemoryManager::garbage();
    }
  }
  if (!beQuiet && reachable.nbStates() < 10)
    std::cout << reachable << std::endl;	
  // Print some stats : memory size, number of states ....
  Statistic S = Statistic(reachable, modelName , CSV); // can also use LATEX instead of CSV
  S.print_table(std::cout);
  std::cout << std::endl;
  // Export the SDD of final states to dot : generates files final.dot and d3final.dot
  if (dodotexport)
    exportDot(reachable,pathdotff);
}


void usage() {
  usageInputOptions();
  usageSDDOptions();
  
  cerr << "its-reach specific options for " << PACKAGE_STRING <<endl;
  cerr << "    --dump-order path : dump the currently used variable order to file designated by path and exit. \n" ;
  cerr<<  "    -d path : specifies the path prefix to construct dot state-space graph" <<endl;
  cerr<<  "    -bmc XXX : use limited depth BFS exploration, up to XXX steps from initial state." << endl;
  cerr<<  "    --quiet : limit output verbosity useful in conjunction with tex output --texline for batch performance runs" <<endl;
  cerr<<  "    --help,-h : display this (very helpful) helping help text"<<endl;
  cerr<<  "Problems ? Comments ? contact " << PACKAGE_BUGREPORT <<endl;
}

void bugreport () {
  cerr << "Timed Petri Net SDD/DDD Analyzer; package " << PACKAGE_STRING <<endl;
  cerr << "If you think your model is valid (i.e. it's a bug from us), please send your model files, specify the version you are running on, and we'll try to fix it." <<endl;
  cerr << "Bugreport contact : " << PACKAGE_BUGREPORT <<endl;
  cerr << "Sorry." << endl;
  exit(1);
}



int main (int argc, char **argv) {


 // instanciate a Model
 ITSModel model;

 // echo options of run
 std::cout << "its-reach command run as :\n" << std::endl;
 for (int i=0;i < argc; i++) {
   std::cout << argv[i] << "  ";
 }
 std::cout << std::endl;

 // Build the options vector
 std::vector<const char *> args;
 for (int i=1;i < argc; i++) {
   args.push_back(argv[i]);
 }

 // parse command line args to get the options 
 handleInputOptions (args, model);
 // we now should have the model defined.
 
 bool with_garbage = true;
 // Setup SDD specific settings
 handleSDDOptions (args, with_garbage);

 
 argc = args.size();
 for (int i=0;i < argc; i++) {
   if (! strcmp(args[i],"-d") ) {
     if (++i > argc) 
       { cerr << "give argument value for .dot file name please after " << args[i-1]<<endl; usage() ; exit(1);}
     pathdotff = args[i];
     dodotexport = true;
   } else if (! strcmp(args[i],"-bmc") ) {
     if (++i > argc) 
       { cerr << "give argument value for BMC depth " << args[i-1]<<endl; usage() ; exit(1);}
     BMC = atoi(args[i]); 
   } else if (! strcmp(args[i],"--help") || ! strcmp(args[i],"-h")  ) {
     usage(); exit(0);
   } else if (! strcmp(args[i],"--quiet")   ) {
     beQuiet = true;
   } else if (! strcmp(args[i],"--dump-order")   ) {
     if (++i > argc) 
       { cerr << "give path value for dump-order " << args[i-1]<<endl; usage() ; exit(1);}
     pathorderff = args[i];     
     dodumporder = true;
   } else {
     cerr << "Error : incorrect Argument : "<<args[i] <<endl ; usage(); exit(0);
   }
 }
 
 
	
 exhibitModel(model);

 return 0;
}

