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

#include "EarlyBreakObserver.hh"
#include "Property.hh"


#ifdef HASH_STAT
#include "gal/ExprHom.hpp"
#endif // HASH_STAT

#define trace if(!beQuiet) std::cerr
//#define trace std::cerr

using namespace its;

static bool beQuiet = false;
static string pathdotff = "final";
static string pathorderff = "order";
static bool dodotexport=false;
static bool dodumporder = false;
static bool dowitness = true; 
static bool with_garbage=true;
static std::string modelName = "";
// if BMC use is wanted, will be >0
static int BMC = -1;
static size_t fixobs_passes = 5000;


State exhibitModel (ITSModel & model) {
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
//    State layer;
    for (int i=0; i < BMC ; ++i) {
      std::cout << "Elapsed (" << process::getTotalTime() << ")";
      std::cout << "At depth " << i << " NbStates=" << reachable.nbStates() << std::endl;
      reachable = hnext (reachable);
      if (previous == reachable) {
	std::cout << "Full state space explored, with depth of "<< i << std::endl;
	break;
      }
//      layer = reachable - previous;
      previous = reachable;
      MemoryManager::garbage();
    }

//     Type::namedTrs_t nlocs ;
//     model.getInstance()->getType()->getNamedLocals(nlocs);
//     std::cout << "Successors :" << std::endl;
//     for (Type::namedTrs_it it = nlocs.begin() ; it != nlocs.end() ; ++it) {
//       std::cout << "By " << it->first << std::endl;
//       std::cout << "number of succs : " << it->second(reachable).nbStates()<< std::endl;
//     }

//     std::cout << layer.nbStates() << " States at distance " << BMC << " from initial :\n" ;
//     model.getInstance()->getType()->printState(layer, std::cout);
    std::cout << std::endl;
  }
  if (!beQuiet && reachable.nbStates() < 10)
    std::cout << reachable << std::endl;	
  // Print some stats : memory size, number of states ....
  Statistic S = Statistic(reachable, modelName , CSV); // can also use LATEX instead of CSV
  S.print_table(std::cout);
  cout.precision(40);
  std::cout << " Total reachable state count : " << S.getNbStates() << std::endl;
  std::cout << std::endl;

#ifdef HASH_STAT
  std::cout << " Some stats on unique table and cache :" << std::endl;
  its::IntExpressionFactory::printStats (std::cout);
  its::BoolExpressionFactory::printStats (std::cout);
  its::query_stats ();
  MemoryManager::pstats();
#endif // HASH_STAT

  // Export the SDD of final states to dot : generates files final.dot and d3final.dot
  if (dodotexport)
    exportDot(reachable,pathdotff);
  return reachable;
}


void usage() {
  usageInputOptions();
  usageSDDOptions();
  
  cerr << "its-reach specific options for " << PACKAGE_STRING <<endl;
  cerr << "    --dump-order path : dump the currently used variable order to file designated by path and exit. \n" ;
  cerr<<  "    -d path : specifies the path prefix to construct dot state-space graph" <<endl;
  cerr<<  "    -bmc XXX : use limited depth BFS exploration, up to XXX steps from initial state." << endl;
  cerr<<  "    --quiet : limit output verbosity useful in conjunction with tex output --texline for batch performance runs" <<endl;
  cerr<<  "    -reachable XXXX : test if there are reachable states that verify the provided boolean expression over variables" <<endl;
  cerr<<  "    -reachable-file XXXX.prop : evaluate reachability properties specified by XXX.prop." <<endl;
  cerr<<  "    --nowitness : disable trace computation and just return a yes/no answer (faster)." <<endl;
  cerr<<  "    -manywitness XXX : compute several traces (up to integer XXX) and print them." <<endl;
  cerr<<  "    --fixpass XXX : test for reachable states after XXX passes of fixpoint (default: 5000), use 0 to build full state space before testing" <<endl;
  cerr<<  "    --help,-h : display this (very helpful) helping help text"<<endl;
  cerr<<  "Problems ? Comments ? contact " << PACKAGE_BUGREPORT <<endl;
}

void bugreport () {
  cerr << "ITS SDD/DDD Analyzer; package " << PACKAGE_STRING <<endl;
  cerr << "If you think your model is valid (i.e. it's a bug from us), please send your model files, specify the version you are running on, and we'll try to fix it." <<endl;
  cerr << "Bugreport contact : " << PACKAGE_BUGREPORT <<endl;
  cerr << "Sorry." << endl;
  exit(1);
}





int main_noex (int argc, char **argv) {


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

 setUsage(&usage);
 // parse command line args to get the options 
 if (! handleInputOptions (args, model) ) {
   return 1;
 }
 // we now should have the model defined.
 modelName = model.getInstance()->getType()->getName();
 
 bool with_garbage = true;
 // Setup SDD specific settings
 if (!handleSDDOptions (args, with_garbage)) {
   return 1;
 }

 vLabel reachExpr="";
 vLabel reachFile="";
 
 argc = args.size();
 int nbwitness=1;
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
   } else if (! strcmp(args[i],"--nowitness")   ) {
     dowitness = false;
   } else if (! strcmp(args[i],"-reachable") ) {
     if (++i > argc) 
       { cerr << "give a boolean expression over model variables for reachable criterion " << args[i-1]<<endl; usage() ; exit(1);}
     reachExpr = args[i];
   } else if (! strcmp(args[i],"-manywitness") ) {
     if (++i > argc) 
       { cerr << "give an integer limit to number of traces " << args[i-1]<<endl; usage() ; exit(1);}
     nbwitness = atoi(args[i]);
     dowitness = false;
   } else if (! strcmp(args[i],"-reachable-file") ) {
     if (++i > argc) 
       { cerr << "Give a file name containing reachability queries. " << args[i-1]<<endl; usage() ; exit(1);}
     reachFile = args[i];
   } else if (! strcmp(args[i],"--dump-order")   ) {
     if (++i > argc) 
       { cerr << "give path value for dump-order " << args[i-1]<<endl; usage() ; exit(1);}
     pathorderff = args[i];     
     dodumporder = true;
   } else if (! strcmp(args[i],"--fixpass") ) {
     if (++i > argc)
     { cerr << "give number of passes in fixpoint after " << args[i-1] << endl; usage(); exit(1); }
     fixobs_passes = atoi(args[i]);
   } else {
     cerr << "Error : incorrect Argument : "<<args[i] <<endl ; usage(); return 1;
   }
 }
 
  if (fixobs_passes != 0 && reachExpr != "")
  {
    Transition predicate = model.getPredicate(reachExpr);
    fobs::set_fixobserver (new EarlyBreakObserver (fixobs_passes, predicate, ! beQuiet));
  }
 
	
 State reachable = exhibitModel(model);

 std::vector<Property> props;

 if (reachFile != "") {
   std::cout << "Loading property file " << reachFile << "."<< std::endl; 
   loadProps(reachFile,props);
 }

 if (reachExpr != "") {
   props.push_back(Property(reachExpr,reachExpr,false));
 }
 
 if (props.size() > 0) {
   std::cout << "Verifying " << props.size() << " reachability properties."<< std::endl; 
 }

 for (std::vector<Property>::const_iterator it = props.begin() ; it != props.end() ; ++it ) {
   
   Transition predicate = model.getPredicate(it->getPred());
   State verify = predicate (reachable);

   if (it->isInvariant()) {
     if (verify == reachable) {
       std::cout << "Invariant property " << it->getName() << " is true." << std::endl;
     } else {
       std::cout << "Invariant property " << it->getName() << " does not hold." << std::endl;
       std::cout << "Reachable states that do not respect the invariant will be exhibited." << std::endl;
     }
     // to build a trace.
     verify = reachable - verify;
   } else {
     if (verify == State::null) {
       std::cout << "Never property " << it->getName() << " is true." << std::endl;
     } else {
       std::cout << "Never property " << it->getName() << " does not hold." << std::endl;
       std::cout << "Reachable states that satisfy the never predicate will be exhibited." << std::endl;
     }
   }
   
   if (verify != State::null) {
     std::cout << "There are " << verify.nbStates() << " reachable states that exhibit your property : " << it->getName() <<std::endl;
     
     if (dowitness) {
       std::cout << "computing trace..." <<endl;
       path_t path = model.findPath(model.getInitialState(), verify, reachable,false);
       model.printPath(path, std::cout,true);
     }
     if (nbwitness >= 1) {
       std::cout << "computing up to "<< nbwitness<<  " traces..." <<endl;
       model.printPaths(model.getInitialState(), verify, reachable,nbwitness);
     }
     std::cout << std::endl;
   }
   
   Statistic Scheck = Statistic(verify, reachExpr , CSV); // can also use LATEX instead of CSV
   cout.precision(6);
   Scheck.print_table(std::cout);

 }

 return 0;
}



int main (int argc, char **argv) { 
  try {
    return main_noex (argc, argv);
  } catch (const char * ex) {
    std::cerr << "An unexpected exception occurred : " << ex << std::endl;
    return 1;
  } catch (std::string err) {
    std::cerr << "An unexpected exception occurred : " << err << std::endl;
    return 1;
  }
}
