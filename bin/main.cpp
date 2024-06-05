#include <iostream>
#include <fstream>
#include <cstring>

#include "its/Property.hh"
#include "its/Options.hh"
// The ITS model referential
#include "its/ITSModel.hh"
#include "ddd/SDD.h"
#include "ddd/MemoryManager.h"
// romeo parser
#include "its/petri/XMLLoader.hh"
// prod parser
#include "its/petri/Modular2ITS.hh"
// ITSModel parser
#include "its/ITSModelXMLLoader.hh"
// Cami parser
#include "its/petri/JSON2ITS.hh"
#include "its/parser_json/parse_json.hh"

// for stats
#include "MaxComputer.hh"
#include "ExactStateCounter.hh"
#include "ToTransRel.hh"

// SDD utilities to output stats and dot graphs
#include "ddd/util/dotExporter.h"
#include "ddd/statistic.hpp"

#include "EarlyBreakObserver.hh"
#include "SMTExporter.hh"
#include "Graph.hh"

#ifdef HASH_STAT
#include "its/gal/ExprHom.hpp"
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
static bool dostats = false;
static bool with_garbage = true;
static bool countEdges = false;
// Does the main instance use an "init" gadget
static  bool hasInitializationGadget = false;
static std::string modelName = "";
static std::string invariantExpr = "";
static bool doInvariant = false;
static bool doFrom = false;
// if BMC use is wanted, will be >0
static int BMC = -1;
static size_t fixobs_passes = 2;

its::State clearState (its::State in);

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

  //  std::cout << " Next rel :" << model.getNextRel() << std::endl;
  State reachable;
  if (BMC <0) {
    if (! doInvariant) {
      // Compute reachable states
      reachable = model.computeReachable(with_garbage);
    } else {
      Transition filter = model.getPredicate(invariantExpr);
      Transition trel = fixpoint ( ( filter & model.getNextRel()) + Transition::id, true );
      reachable = trel (model.getInitialState());
    }
  } else {
    Transition hnext = model.getNextRel() + Transition::id;
    if (doInvariant) {
      hnext = (model.getPredicate(invariantExpr) & model.getNextRel()) + Transition::id ;
    }
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
  if (!beQuiet) {
    std::cout << " Some reachable states : " ;
    model.printSomeStates(reachable, std::cout);
  }
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
  cerr<<  "    -trace XXX : try to replay a trace, XXX is given as a space separated list of transition names, as used in path outputs." << endl;
  cerr<<  "    --quiet : limit output verbosity useful in conjunction with tex output --texline for batch performance runs" <<endl;
  cerr<<  "    --stats : produce stats on max sum of variables (i.e. maximum tokens in a marking for a Petri net), maximum variable value (bound for a Petri net)" <<endl;
  cerr<<  "    --edgeCount : produce stats on the size of the transition relation, i.e. the number of unique source to target state pairs it contains." <<endl;
  cerr<<  "    -maxbound XXXX,YYYY : return the maximum value for each variable in the list (comma separated)" <<endl;
  cerr<<  "    -reachable XXXX : test if there are reachable states that verify the provided boolean expression over variables" <<endl;
  cerr<<  "    -reachable-from XXXX : Consider that initial states are (reachable from initial) states satisfying the given predicate." <<endl;
  cerr<<  "    -with-invariant XXXX : only states satisfying the provided boolean invariant are considered successors of a state." <<endl;
  cerr<<  "    -reachable-file XXXX.prop : evaluate reachability properties specified by XXX.prop." <<endl;
  cerr<<  "    --init-gadget : suppose that the initial state is actually a precursor of initial states, i.e. initial states are successors of the initial state. (prototype flag, only used in manywitness scenario currently, may be replaced by another mechanism in future).)" << endl;
  cerr<<  "    --nowitness : disable trace computation and just return a yes/no answer (faster)." <<endl;
  cerr<<  "    -manywitness XXX : compute several traces (up to integer XXX) and print them." <<endl;
  cerr<<  "    --fixpass XXX : test for reachable states after XXX passes of fixpoint (default: 5000), use 0 to build full state space before testing" <<endl;
  cerr<<  "    Witness-graph flags : output the state space graph for the region of interest, replaces production of witness traces" <<endl;
  cerr<<  "    -wgo PATHPREFIX : generate dot output and decide where to output the witness graphs (default : $CWD/wg)" <<endl;
  cerr<<  "    -wgoDD PATHPREFIX : generate dot output of the witness graph DD, decide where to output the witness graphs DD (default : $CWD/wgDD)" <<endl;
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
 vLabel fromExpr="";
 vLabel boundsExpr="";
 vLabel reachFile="";
 vLabel traceStr = "";
 vLabel smtpath = "";
 vLabel wgopath = "wg";
 vLabel wgoDDpath = "wgDD";
 bool dosmtexport = false;
 bool dographO = false;
 bool dographDD = false;
 bool dograph = false;
 argc = args.size();
 int nbwitness=0;
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
   } else if (! strcmp(args[i],"-wgo") ) {
     if (++i > argc)
       { cerr << "give argument value for witness graph path " << args[i-1]<<endl; usage() ; exit(1);}
     wgopath = args[i];
     dograph = true;
     dographO = true;
   } else if (! strcmp(args[i],"-wgoDD") ) {
     if (++i > argc)
       { cerr << "give argument value for witness graph DD path " << args[i-1]<<endl; usage() ; exit(1);}
     wgoDDpath = args[i];
     dograph = true;
     dographDD = true;
   } else if (! strcmp(args[i],"-exportsmt") ) {
     if (++i > argc) 
       { cerr << "give argument value for SMT export file " << args[i-1]<<endl; usage() ; exit(1);}
     dosmtexport = true;
     dographDD = true;
     smtpath = args[i]; 
   } else if (! strcmp(args[i],"--help") || ! strcmp(args[i],"-h")  ) {
     usage(); exit(0);
   } else if (! strcmp(args[i],"--quiet")   ) {
     beQuiet = true;
   } else if (! strcmp(args[i],"--stats")   ) {
     dostats = true;
   } else if (! strcmp(args[i],"--edgeCount")   ) {
     countEdges = true;
   } else if (! strcmp(args[i],"--nowitness")   ) {
     dowitness = false;
   } else if (! strcmp(args[i],"--init-gadget")   ) {
     hasInitializationGadget = true;
   } else if (! strcmp(args[i],"-reachable") ) {
     if (++i > argc) 
       { cerr << "give a boolean expression over model variables for reachable criterion " << args[i-1]<<endl; usage() ; exit(1);}
     reachExpr = args[i];
   } else if (! strcmp(args[i],"-reachable-from") ) {
     if (++i > argc) 
       { cerr << "give a boolean expression over model variables for reachable criterion " << args[i-1]<<endl; usage() ; exit(1);}
     fromExpr = args[i];
     doFrom = true;
   } else if (! strcmp(args[i],"-with-invariant") ) {
     if (++i > argc) 
       { cerr << "give a boolean expression over model variables for reachable criterion " << args[i-1]<<endl; usage() ; exit(1);}
     invariantExpr = args[i];
     doInvariant = true;
   } else if (! strcmp(args[i],"-maxbound") ) {
     if (++i > argc) 
       { cerr << "give a comma separated list of variables for maxbounds criterion " << args[i-1]<<endl; usage() ; exit(1);}
     boundsExpr = args[i];
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
   } else if (! strcmp(args[i],"-trace") ) {
     if (++i > argc)
     { cerr << "give trace after " << args[i-1] << endl; usage(); exit(1); }
     traceStr = args[i];
   } else {
     cerr << "Error : incorrect Argument : "<<args[i] <<endl ; usage(); return 1;
   }
 }

 std::vector<Property> props;

 if (reachFile != "") {
   std::cout << "Loading property file " << reachFile << "."<< std::endl; 
   loadProps(reachFile,props);
 }
 
 if (reachExpr != "") {
   props.push_back(Property(reachExpr,reachExpr,REACH));
 } 
 
 
 if (traceStr != "") {
   labels_t path ;
   stringstream ss (traceStr);
   vLabel str;
   while (ss >> str) {
     path.push_back(str);
   }
   
   model.playPath(path);
   return 0;
   
 }

 // Test that we don't have several props to check, otherwise do not set up interrupt.
 if (fixobs_passes != 0 && !doFrom && !dowitness && nbwitness==0 && std::find_if(props.begin(),props.end(),[](Property & p){ return p.getType() == BOUNDS ;}) == props.end() ) {
   // This observer interrupts computation if the predicate is found
   fobs::set_fixobserver (new EarlyBreakObserver (fixobs_passes, props, model, true )); // !beQuiet
 }
	
 State reachable = exhibitModel(model);

 if (dosmtexport) {
   SMTExporter smtexp (smtpath);
   if (smtexp.exportDD(model,reachable)) {
     if (props.size() > 0) {
       std::cout << "Verifying " << props.size() << " reachability properties."<< std::endl; 
     }

     for (std::vector<Property>::const_iterator it = props.begin() ; it != props.end() ; ++it ) {
       smtexp.exportPredicate(it->getName(), model, it->getPred());
     }
   }
   smtexp.close();
   return 0;
 }

 if (dostats) {
 // short scopes since these classes cost a cache to maintain.
  {
   MaxComputer mc ;
   MaxComputer::stat_t stat = mc.compute(reachable);
   mc.printStats(stat, std::cout);
  }
  {
   ExactStateCounter mc ;
   ExactStateCounter::stat_t stat = mc.compute(reachable);
   mc.printStats(stat, std::cout);	  
  }
 }
 if (countEdges)  {
   ExactStateCounter mc ;   
   mpz_class total = 0;
   its::State rel = getTransRel(model);
   total = mc.compute(rel);     
   
   // // declarations common to cluster and transition approaches
   // Type::namedTrs_t namedTrs;
   // model.getNamedLocals(namedTrs);
   // size_t tsize = namedTrs.size();
   
   // // transitions approach
   // int done = 0;
   // for (Type::namedTrs_it it=namedTrs.begin(); it != namedTrs.end() ; ++it) {
   //   its::State succs = (it->second)(model.computeReachable());
   //   total += mc.compute(succs);
   //   ++done;
   //   if (MemoryManager::should_garbage()) {
   // 	 std::cerr << "Done " << done << "/" << tsize << " transitions." << std::endl;
   // 	 mc.clear();
   // 	 MemoryManager::garbage();
   //   }
   // }
   
   // // CLUSTER APPROACH 
   // std::list<its::Transition> clusters;
   // for (Type::namedTrs_it it=namedTrs.begin(); it != namedTrs.end() ; ++it) {
   //   bool added = false;
   //   for (std::list<its::Transition>::iterator jt = clusters.begin() ; jt != clusters.end() ; ++jt ) {
   // 	 if ( commutative(*jt, it->second) ) {
   // 	   *jt = *jt + it->second;
   // 	   added = true;
   // 	   break;
   // 	 }
   //   }
   //   if (!added) {
   // 	 clusters.push_back(it->second);
   //   }
   // }
   // std::cerr << "Built " << clusters.size() << " clusters from "<< tsize << " transitions. " << std::endl;
   
   
   // int done = 0;
   // for (std::list<its::Transition>::iterator jt = clusters.begin() ; jt != clusters.end() ; ++jt ) {
   //   its::State succs = (*jt)(model.computeReachable());
   //   total += mc.compute(succs);
   //   ++done;
   //   if (MemoryManager::should_garbage()) {
   // 	 std::cerr << "Done " << done << "/" << clusters.size() << " clusters." << std::endl;
   // 	 mc.clear();
   // 	 MemoryManager::garbage();
   //   }
   // }
   
   std::cout << "Total edges in reachability graph : " << total << std::endl;
 }
 

 if (boundsExpr != "") {
    std::istringstream iss(boundsExpr);
    vLabel token;
    while (std::getline(iss, token, ',')) {     
      props.push_back(Property(token,token,BOUNDS));
    }
 } 
  
 if (props.size() > 0) {
   std::cout << "Verifying " << props.size() << " reachability properties."<< std::endl; 
 }
 
 
 its::State initialSituation = model.getInitialState();
 if (doFrom) {
   Transition predicate = model.getPredicate(fromExpr);
   //     std::cerr << "built pred :" << predicate << std::endl;
   State verify = predicate.has_image(reachable);
   bool isVerify =  ! ( verify == State::null );
   
   if (isVerify) {
     std::cout << "There are reachable states satisfying the *reachable-from* predicate :" << fromExpr << std::endl;
     initialSituation = predicate (reachable);
     std::cout << "Starting traces in one of the  " << initialSituation.nbStates() << " reachable states in which your \"from\" predicate is true." <<std::endl;
     Transition trans = model.getNextRel();
     if (doInvariant) {
       trans = model.getPredicate(invariantExpr) & trans;
     }
     // top-level = true for garbage collection
     Transition transrel = fixpoint(trans+GShom::id,true);
     reachable = transrel (initialSituation);
     std::cout << "Considering only states reachable from these situations; total   " << reachable.nbStates() << " reachable states." <<std::endl;
   } else {
     std::cout << "reachable-from predicate is not validated in any reachable state : " << fromExpr  << " does not hold." << std::endl;
     std::cout << "Interrupting computation of witnesses." << std::endl;
     props.clear();
   }   
 }
 int pindex = 0;
 for (std::vector<Property>::const_iterator it = props.begin() ; it != props.end() ; ++it, pindex++ ) {
   
   if (MemoryManager::should_garbage()) {
     MemoryManager::garbage();
   }
   bool isVerify = false;

   State verify = State::null;
   
   if (it->getType() == BOUNDS) {
     std::istringstream iss(it->getPred());
     labels_t tokens;
     vLabel token;
     while (std::getline(iss, token, '+')) {     
       tokens.push_back(token);
     }
     its::State init = initialSituation;
     its::State clear = clearState(init);
     // substitutes values not in target set by a potential forced to only : 0
     its::Transition getter = model.getInstance()->getType()->observe(tokens, clear);
     // apply it to state space
     its::State res = getter (reachable);
     // apply a min/max stats computer to result
     MaxComputer mc (false);
     MaxComputer::stat_t stat = mc.compute(res);
     mc.printStats(stat, std::cout);
     std::cout << "Bounds property "<< it->getName() << " :" << stat.first <<  " <= " << it->getPred() << " <= " << stat.second << std::endl;  
   
   } else if (it->getType() == INVARIANT) {
     Transition predicate = model.getPredicate("!(" + it->getPred() + ")");
     //     predicate = ! predicate;
     verify = predicate.has_image(reachable);
     isVerify = predicate.has_image(reachable) == State::null;
     if (isVerify ) {
       std::cout << "Invariant property " << it->getName() << " is true." << std::endl;
     } else {
       std::cout << "Invariant property " << it->getName() << " does not hold." << std::endl;
       if (dowitness || nbwitness > 0 || dograph) {
	 std::cout << "Reachable states that do not respect the invariant will be exhibited." << std::endl;
	 // to build a trace.
	 verify = predicate (reachable);
       }
     }

   } else if (it->getType() == NEVER) {
     Transition predicate = model.getPredicate(it->getPred());
     verify = predicate.has_image(reachable);
     isVerify = verify == State::null;

     if (isVerify) {
       std::cout << "Never property " << it->getName() << " is true." << std::endl;
     } else {
       std::cout << "Never property " << it->getName() << " does not hold." << std::endl;
       if (dowitness || nbwitness > 0 || dograph) {
	 std::cout << "Reachable states where the predicate is true will be exhibited." << std::endl;
	 // to build a trace.
	 verify = predicate (reachable);
       }
     }
   } else {
     Transition predicate = model.getPredicate(it->getPred());
     //     std::cerr << "built pred :" << predicate << std::endl;
     verify = predicate.has_image(reachable);
     isVerify =  ! ( verify == State::null );

     if (isVerify) {
       std::cout << "Reachability property " << it->getName() << " is true." << std::endl;
       if (dowitness || nbwitness > 0 || dograph) {
	 std::cout << "Reachable states where the predicate is true will be exhibited." << std::endl;
	 verify = predicate (reachable);
	 std::cout << "There are " << verify.nbStates() << " reachable states in which your predicate is true." <<std::endl;
       }
     } else {
       std::cout << "Reachability property " << it->getName() << " does not hold." << std::endl;
       std::cout << "No reachable states exhibit your property : " << it->getName() <<std::endl;
     }
   }
   
 
   if (dowitness && verify != State::null) {
     std::cout << "computing trace..." <<endl;
     path_t path = model.findPath(initialSituation, verify, reachable, false);
     model.printPath(path, std::cout,true);
   }
   if (nbwitness >= 1 && verify != State::null) {
     std::cout << "computing up to "<< nbwitness<<  " traces..." <<endl;
     // gadget the first time
     State init = initialSituation;
     if (hasInitializationGadget) {
       init = model.getNextRel() (initialSituation);
     }
     model.printPaths(init, verify, reachable, nbwitness);
   }
   if (dograph && verify != State::null) {
	   labels_t vars;
   	   model.getInstance()->getType()->addFlatVarSet(vars,"");
   	   vLabel pwg = wgopath + "_" + to_string(pindex) +".dot";
   	   vLabel pwgDD = wgoDDpath + "_" + to_string(pindex);
	   GraphBuilder gb (pwg, pwgDD, dographO, dographDD ,vars);
	   Type::namedTrs_t trn;
	   model.getNamedLocals(trn);
	   Transition predfp = fixpoint( model.getPredRel(reachable) + Transition::id);
	   State toplot = predfp (verify);
	   if (dographO) {
		   std::cout << "Building witness graph (size=" << toplot.nbStates() << " vertices) for property " << it->getName() << " in file : " << pwg << endl;
	   }
	   if (dographDD) {
		   std::cout << "Building witness graph DD for property " << it->getName() << " in file : " << pwgDD << endl;
	   }
	   plotGraph (toplot, trn,& gb);
   } else if (dograph) {
	   std::cout << "No witness graph built for true property " << it->getName() << " produced." <<endl;
   }
   std::cout << std::endl;
   
   Statistic Scheck = Statistic(verify, it->getName() , CSV); // can also use LATEX instead of CSV
   cout.precision(6);
   Scheck.print_table(std::cout);

#ifdef HASH_STAT
  std::cout << " Some stats on unique table and cache :" << std::endl;
  its::IntExpressionFactory::printStats (std::cout);
  its::BoolExpressionFactory::printStats (std::cout);
  its::query_stats ();
  MemoryManager::pstats();
#endif // HASH_STAT

 }

 return 0;
}



int main (int argc, char **argv) { 

  // Reserve 16K of memory that can be deleted just in case we run out of memory
  char* _emergencyMemory = new char[16384];
  try {
    return main_noex (argc, argv);
  } catch (const char * ex) {
    std::cerr << "An unexpected exception occurred : " << ex << std::endl;
    return 1;
  } catch (std::string & err) {
    std::cerr << "An unexpected exception occurred : " << err << std::endl;
    return 1;
  } catch(std::bad_alloc & ex) {
    // Delete the reserved memory so we can print an error message before exiting
    delete[] _emergencyMemory;

    MemoryManager::pstats();
    cerr << "Out of memory error !";
    cin.get();
    exit(1);
  }

}


class ClearHomDDD : public StrongHom {
public:
  size_t hash() const {
    return 419;
  }
  _GHom* clone() const {
    return new ClearHomDDD(*this);
  }

  GHom phi(int var, int val) const {
    return GHom(var, 0, this);
  }
  GDDD phiOne() const{return GDDD::one;}
  bool operator==(const StrongHom &h) const { return true; }
};

DDD clearDDD (const DDD & in) {
  Hom cl = Hom(ClearHomDDD());
  return  cl(in);
}

class SClearHom : public StrongShom {
public :
  size_t hash() const {
    return 31;
  }
  _GShom * clone () const {
    return new SClearHom(*this);
  }

  GShom phi(int var,const DataSet& val) const {
    if (typeid(val) == typeid(const GSDD&)) {
      GSDD v2 = Shom(*this) ((const GSDD &)val);
      return GShom(var, v2, this);
    } else {
      DDD v2 = clearDDD( (const DDD&) val);
      return GShom(var, v2, this);
    }
  }

  GSDD phiOne() const {
    return GSDD::one;
  }
  bool operator==(const StrongShom &h) const {
    return true;
  }
};

its::State clearState (its::State in) {
  its::Transition hom = its::Transition(SClearHom());
  return hom(in);
}
