#include "Options.hh"

#include <string>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <fstream>

#if defined(ENABLE_DLL) 
// DL sym stuff
#include <dlfcn.h>
#endif


// for wibble string
#include "divine/wibble/string.h"

#include "SDD.h"
#include "MemoryManager.h"

#include "petri/Modular2ITS.hh"
// ITSModel parser
#include "ITSModelXMLLoader.hh"
// Cami parser
#include "petri/JSON2ITS.hh"
// JSon parser
#include "parser_json/parse_json.hh"
// romeo parser
#include "petri/XMLLoader.hh"
// Divine interaction
#include "divine/dveLoader.hh"
// GAL parser
#include "gal/parser/GALParser.hh"

#include "gal/GALType.hh"

using std::string;
using std::cerr;
using std::endl;
using std::ifstream;

namespace its {



  static void showUsageBoth() {
    usageInputOptions();
    usageSDDOptions();
  }

  static void (*usageHelp) (void) = &showUsageBoth;

  void setUsage( void (*usage) (void)) {
    usageHelp = usage;
  }

  static void showUsageHelp () {
    (* usageHelp) ();
  }

  static int run_system ( std::string command ) {
    int status = system( command.c_str() );
    if ( status != -1 &&  status != 0 ) {
      std::cerr <<  "Error running external command: \n" + command +"\n";
      return 1;
    }
    return 0;
  }

/** Consumes the options that are recognized in args, and treats them to build the Model.
 *  Options recognized by this options parser: 
 MANDATORY : -i Inputfile -t {CAMI|PROD|ROMEO|ITSXML|ETF} 
 OPTIONAL : --load-order {$f.json|$f.ord}      NB:extension of file is used to determine its type
 CONFIGURATION : --ddd|--sdd : privilege ddd or sdd in encoding. sdd is default
 For Scalar and circular : -ssD2, -ssDR, -ssDS   (default -ssD2 1)
*/
bool handleInputOptions (std::vector<const char *> & argv, ITSModel & model) {

  string pathinputff = "";
  enum InputType {NDEF,CAMI,PROD,ROMEO,UROMEO,ITSXML,ETF,DLL,NDLL,DVE,GAL_T, CGAL_T};
  InputType parse_t = NDEF;

  bool hasOrder=false;
  string pathorderff;

  bool hasJson = false;
  string pathjsonff;

#if defined(ENABLE_DLL) 
  // For use in NDLL case
  // int Nsize = -1;
#endif
  

  std::vector<const char *> argsleft;

  int argc = argv.size();
  for (int i=0;i < argc; i++) {

    /** INPUT FILE OPTIONS */
   if ( ! strcmp(argv[i],"-i") ) {
     if (++i > argc) 
       { cerr << "give argument value for input file name please after " << argv[i-1]<<endl; showUsageHelp() ;exit(1);}
     pathinputff = argv[i];
   } else if ( ! strcmp(argv[i],"-t") ) {
     if (++i > argc) 
       { cerr << "give argument value for Prod file name please after " << argv[i-1]<<endl;  showUsageHelp() ;exit(1);}
     
     if ( !strcmp(argv[i],"CAMI" ) ) {
       parse_t = CAMI;
     } else if ( !strcmp(argv[i],"PROD") ) {
       parse_t = PROD;
     } else if ( !strcmp(argv[i],"ROMEO") ) {
       parse_t = ROMEO;
     } else if ( !strcmp(argv[i],"UROMEO") ) {
       parse_t = UROMEO;
     } else if ( !strcmp(argv[i],"ITSXML") ) {
       parse_t = ITSXML;
     } else if ( !strcmp(argv[i],"ETF") ) {
       parse_t = ETF;
     } else if ( !strcmp(argv[i],"DLL") ) {
       parse_t = DLL;
     } else if ( !strcmp(argv[i],"NDLL") ) {
       parse_t = NDLL;
     } else if ( !strcmp(argv[i],"DVE") ) {
       parse_t = DVE;
     } else if ( !strcmp(argv[i],"GAL") ) {
       parse_t = GAL_T;
     } else if ( !strcmp(argv[i],"CGAL") ) {
       parse_t = CGAL_T;
     } else {
       cerr << "Unrecognized type "<< argv[i] <<" provided for input file after " << argv[i-1] << " one of {CAMI|PROD|ROMEO|UROMEO|ITSXML|ETF|DVE|DLL|NDLL|GAL|CGAL} is expected. " << endl;  showUsageHelp() ;exit(1);
     }

     /** ORDER FILE OPTIONS */
   } else if ( ! strcmp(argv[i],"--json-order") ) {
     if (++i > argc) 
       { cerr << "Give a file name containing a JSON variable order definition please after " << argv[i-1]<<endl;  showUsageHelp() ;exit(1);}
     pathjsonff = argv[i];
     hasJson = true;
   } else if ( ! strcmp(argv[i],"--print-limit") ) {
      if (++i > argc) 
       { cerr << "give numeric value in number of states for print-limit option " << argv[i-1]<<endl;  showUsageHelp() ; return false;}
      int threshold = atoi(argv[i]);
      model.setPrintLimit(threshold);
   } else if ( ! strcmp(argv[i],"--load-order") ) {
     if (++i > argc) 
       { cerr << "Give a file name containing a variable order definition please after " << argv[i-1]<<endl;  showUsageHelp() ;exit(1);}
     pathorderff = argv[i];
     hasOrder = true;
   } else if ( ! strcmp(argv[i],"--gen-order") ) {
     if (++i > argc) 
       { cerr << "Give description of the heuristic used after " << argv[i-1]<<endl;  showUsageHelp() ;exit(1);}
     // For parameters to Force tool
     orderHeuristicType orderHeuristic = DEFAULT;

     if ( !strcmp(argv[i],"DEFAULT") ) {
       orderHeuristic = DEFAULT;
     } else if ( !strcmp(argv[i],"FOLLOW") ) {
       orderHeuristic = FOLLOW;
     } else if ( !strcmp(argv[i],"FOLLOW_HALF") ) {
       orderHeuristic = FOLLOW_HALF;
     } else if ( !strcmp(argv[i],"FOLLOW_DOUBLE") ) {
       orderHeuristic = FOLLOW_DOUBLE;
     } else if ( !strcmp(argv[i],"FOLLOW_SQ") ) {
       orderHeuristic = FOLLOW_SQ;
     } else if ( !strcmp(argv[i],"FOLLOW_DYN") ) {
       orderHeuristic = FOLLOW_DYN;
     } else if ( !strcmp(argv[i],"FOLLOW_DYN_SQ") ) {
       orderHeuristic = FOLLOW_DYN_SQ;
     } else if ( !strcmp(argv[i],"FOLLOW_FDYN") ) {
       orderHeuristic = FOLLOW_FDYN;
     } else if ( !strcmp(argv[i],"FOLLOW_FDYN_SQ") ) {
       orderHeuristic = FOLLOW_FDYN_SQ;
     } else if ( !strcmp(argv[i],"SATUR") ) {
       orderHeuristic = SATUR;
     } else if ( !strcmp(argv[i],"LEXICO") ) {
       orderHeuristic = LEXICO;
     } else {
       cerr << "Unrecognized type " << argv[i] << " provided for variable ordering strategy after " << argv[i-1] << endl;
       showUsageHelp();
       exit(1);
     }
     model.setGALOrderStrategy(orderHeuristic);
     /** ENCODING STRATEGIES FOR SCALAR SETS */
   } else if (! strcmp(argv[i],"-ssD2") ) {
     if (++i > argc) 
       { cerr << "give argument value for scalar strategy " << argv[i-1]<<endl;  showUsageHelp() ; exit(1);}
     int grain = atoi(argv[i]);
     model.setScalarStrategy(DEPTH1,grain);
   }else if (! strcmp(argv[i],"-ssDR") ) {
     if (++i > argc) 
       { cerr << "give argument value for scalar strategy " << argv[i-1]<<endl;  showUsageHelp() ; exit(1);}
     int grain = atoi(argv[i]);
     model.setScalarStrategy(DEPTHREC,grain);   
   }else if (! strcmp(argv[i],"-ssDS") ) {
     if (++i > argc) 
       { cerr << "give argument value for scalar strategy " << argv[i-1]<<endl;  showUsageHelp() ; exit(1);}
     int grain = atoi(argv[i]);
     model.setScalarStrategy(SHALLOWREC,grain);

     /** ENCODING STRATEGIES FOR PETRI NETS */
   } else if (! strcmp(argv[i],"--ddd")   ) {
     model.setStorage(ddd_storage);
   } else if (! strcmp(argv[i],"--sdd")   ) {
     model.setStorage(sdd_storage);

     /** STUTTERING ON DEADLOCKS STRATEGIES
      ** \warning only valid for GAL and DVE */
   } else if (! strcmp(argv[i],"--stutter")   ) {
     model.setStutterOnDeadlock(true);

     /** LEFTOVER OPTIONS */
   } else {
     argsleft.push_back(argv[i]);
   }
 }

  /** return unparsed options */
  argv = argsleft;

  if (pathinputff == "") {
      std::cerr << "Please specify input problem with option -i.\n" ;
       showUsageHelp();
      exit(1);
  }

  /** Now treat the options to parse the actual input */
  switch (parse_t) {
  case PROD : 
    {
      vLabel nname;
      if (hasJson) {
	nname = RdPELoader::loadJsonProd (model,pathinputff,pathjsonff);
      } else {
	nname = RdPELoader::loadModularProd(model,pathinputff);
      }
      model.setInstance(nname, nname);
      model.setInstanceState("init");  
      break;
    }
  case CAMI : 
    {
      vLabel nname;
      if (hasJson) {
	nname = JSONLoader::loadJsonCami (model,pathinputff,pathjsonff);
      } else {
	nname = JSONLoader::loadCami(model,pathinputff);
      }
      model.setInstance(nname, nname);
      model.setInstanceState("init");  
      break;
    }
  case ETF : 
    {
      model.declareETFType(pathinputff);
      model.setInstance(pathinputff,"main");
      model.setInstanceState("init");      
      break;
    }
  case ITSXML : 
    {
      ITSModelXMLLoader::loadXML(pathinputff, model);
      break;
    }
  case ROMEO :
  case UROMEO :
    {
      
      TPNet * pnet ;
      if (parse_t == ROMEO) {
	pnet = XMLLoader::loadXML(pathinputff, hasJson);
      } else {
	pnet = XMLLoader::loadXML(pathinputff, true);
      }
      if (hasJson) {
	json::Hierarchie * hier = new json::Hierarchie();
	json::json_parse(pathjsonff, *hier);
	model.declareType(*pnet,hier);
      } else {
	model.declareType(*pnet);
      }
      model.setInstance(pnet->getName(),"main");
      model.setInstanceState("init");
      break;
    }
  case DVE :
    {
      Label modelName = model.declareDVEType(pathinputff);
      if (modelName != "") {
	model.setInstance(modelName, "main");
	model.setInstanceState("init");
      }
      break;
    }
  case GAL_T :
    {
      // do the parsing
      GAL * result = GALParser::loadGAL(pathinputff);
      model.declareType (*result);
      
      model.setInstance(result->getName(), "main");
      model.setInstanceState("init");
      break;
    }
  case CGAL_T :
    {
      // do the parsing
      GALParser::loadCGAL(pathinputff,model);
//       model.declareType (*result);
      

//       model.setInstance(result->getName(), "main");
//       model.setInstanceState("init");

      break;
    }

  case NDLL : {
#if defined(ENABLE_DLL) 
    char buff [1024]; // should be enough hopefully for a file name
    if (sscanf(pathinputff.c_str(),"%d:%s",&Nsize,buff) != 2) {
      std::cerr << "When using option NDLL, provide an input formatted as : size:library.so \n For instance : -t NDLL -i 20:philos.so"<<  std::endl;
      exit(1);
    }
    pathinputff = buff;

    //    std::cerr << Nsize << ":" << pathinputff << std::endl;

    // deliberately fall through to DLL case
  }
  case DLL :
    {
      void *handle;
      void (*loadModel)(its::ITSModel &,int);
      char *error;

      
      
      handle = dlopen (pathinputff.c_str(), RTLD_LAZY);
      if (!handle) {
	fputs (dlerror(), stderr);
	std::cerr << "\n When providing a DLL input, make sure it is in one of the LD_LIBRARY_PATH folders or in the current directory."<< std::endl;
	return false;
      }
      
      loadModel = (void (*)(its::ITSModel &,int)) dlsym(handle, "loadModel");
      if ((error = dlerror()) != NULL)  {
	fputs(error, stderr);
	std::cerr << "When providing a DLL input, it should contain a function void (*loadModel)(Model &) " << std::endl;
	return false;
      }
      
      // NSIZE shoudl simply be ignored as necessary if DLL (instead of NDLL) mode is used.
      (*loadModel) (model,Nsize);
      dlclose(handle);
      break;
#else
      // fallthru
    }
  case DLL :
    {
      std::cerr << "Input options by shared object are not available on Mingw/Win32 platform. Make sure you have activated --enable-dll in configure to have this input option available. Sorry."<<std::endl;
      exit(0);
    
#endif
    }
  case NDEF :
  default : 
    {
      std::cerr << "Please specify input problem type with option -t. Supported types are :  {CAMI|PROD|ROMEO|ITSXML|ETF...} \n" ;
      return false;
      break;
    }
  }

  if (hasOrder) {
   ifstream is (pathorderff.c_str());
   if (! model.loadOrder(is)) {
     std::cerr << "Problem loading provided order file :" << pathorderff << "\n";
     return false;
   } else {
     std::cout << "Successfully loaded order from file " << pathorderff << std::endl;
   }
 }

 return true;
}

void usageInputOptions() {
    cerr << " Instantiable Transition Systems SDD/DDD Analyzer; package " << PACKAGE_STRING <<endl;
    cerr << " This tool performs state-space analysis of Instantiable Transition Systems a.k.a. ITS \n"
	 << " The reachability set is computed using SDD/DDD of libDDD, the Hierarchical Set Decision Diagram library, \n"
	 << " Please see README file enclosed \n"
	 << " in the distribution for more details.\n"
	 << "(see Samples dir for documentation and examples). \n \nMANDATORY Options :" << endl;
    cerr<<  "    -i path : specifies the path to input model file" <<endl;
    cerr<<  "    -t {CAMI|PROD|ROMEO|ITSXML|ETF|DLL|NDLL|DVE|GAL|CGAL} : specifies format of the input model file : " <<endl;
    cerr<<  "             CAMI : CAMI format (for P/T nets) is the native Petri net format of CPN-AMI" <<endl;
    cerr<<  "             PROD : PROD format (for P/T nets) is the native format of PROD" <<endl;
    cerr<<  "             ROMEO : an XML format (for Time Petri nets) that is the native format of Romeo" <<endl;
    cerr<<  "             UROMEO : Romeo format with additional constraints: all places named, with different names." <<endl;
    cerr<<  "             ITSXML : a native XML format (for ANY kind of ITS) for this tool. These files allow to point to other files." <<endl;
    cerr<<  "             ETF : Extended Table Format is the native format used by LTSmin, built from many front-ends." <<endl;
    cerr<<  "             DLL : use a dynamic library that provides a function \"void loadModel (Model &,int)\" typically written using the manipulation APIs. See demo/ folder." <<endl;
    cerr<<  "             NDLL : same as DLL, but expect input formatted as size:lib.so. See demo/ folder." <<endl;
    cerr<<  "             DVE : Divine is a modelling language similar to Promela." <<endl;
    cerr<<  "             GAL : Guarded Action Language." << endl; 
    cerr<<  "             CGAL : Guarded Action Language + Composite/ITS textual syntax. File must contain a main declaration." << endl;
    cerr<< "\nAdditional Options and Settings:" << endl;
    cerr <<  "    --print-limit INT : set the threshold for full printout of states in traces. DD holding more states than threshold will not be printed. [DEFAULT:10 states]" << endl;
    cerr << "    --load-order path : load the variable order from the file designated by path. This order file can be produced with --dump-order. Note this option is not exclusive of --json-order; the model is loaded as usual, then the provided order is applied a posteriori. \n" ;
    cerr<< "\nPetri net specific options :" << endl;
    cerr<<  "    --json-order path : use a JSON encoded hierarchy description file for a Petri net model (CAMI, PROD or ROMEO), such as produced using Neoppod heuristic ordering tools. Note that this option modifies the way the model is loaded. \n " <<endl;    
    cerr<<  "    --sdd : privilege SDD storage (Petri net models only).[DEFAULT]" <<endl;
    cerr<<  "    --ddd : privilege DDD (no hierarchy) encoding (Petri net models only)." <<endl;
    cerr<< "\nScalar and Circular symmetric composite types options:" << endl;
    cerr<<  "    -ssD2 INT : (depth 2 levels) use 2 level depth for scalar sets. Integer provided defines level 2 block size. [DEFAULT: -ssD2 1]" <<endl;
    cerr<<  "    -ssDR INT : (depth recursive) use recursive encoding for scalar sets. Integer provided defines number of blocks at highest levels." <<endl;
    cerr<<  "    -ssDS INT : (depth shallow recursive) use alternative recursive encoding for scalar sets. Integer provided defines number of blocks at lowest level.\n" <<endl;
    cerr<< "\nGAL-based specific options (DVE and GAL):" << endl;
    cerr<<  "    --gen-order STRAT :  Invoke ordering heuristic to compute a static ordering." << endl;
    cerr<<  "                         STRAT should be one of the following [default DEFAULT]:" << endl;
    cerr<<  "       DEFAULT         : historical strategy, does not follow labels of 'call' statements"  << endl;
    cerr<<  "       FOLLOW          : follows the labels of 'call' statements"  << endl;
    cerr<<  "       FOLLOW_HALF     : follows the labels of 'call' statements, but with halved weight"  << endl;
    cerr<<  "       FOLLOW_DOUBLE   : follows the labels of 'call' statements, but with doubled weight"  << endl;
    cerr<<  "       FOLLOW_SQUARE   : same as FOLLOW, but uses energy-based costs"  << endl;
    cerr<<  "       FOLLOW_DYN      : follows the labels of 'call' statements, with a cost related to constraint' size"  << endl;
    cerr<<  "       FOLLOW_DYN_SQ   : same as FOLLOW_DYN, but uses energy-based costs"  << endl;
    cerr<<  "       FOLLOW_FDYN     : same as FOLLOW_DYN, but the cost is related to the size for all constraints (even with no 'call')"  << endl;
    cerr<<  "       FOLLOW_FDYN_SQ  : same as FOLLOW_FDYN, but uses energy-based costs"  << endl;
    cerr<<  "       LEXICO          : use the old strategy, based on lexicographical ordering of the variable"  << endl;
}

/** Consumes the options that are recognized in args, and treats them to configure libDDD
 *  Options recognized by this options parser: --no-garbage, --gc-threshold XXX (in kb), --fixpoint {BFS,DFS}
*/
bool handleSDDOptions (std::vector<const char *> & argv, bool & with_garbage) {

  std::vector<const char *> argsleft;

  int argc = argv.size();
  for (int i=0;i < argc; i++) {
   if (! strcmp(argv[i],"--no-garbage")   ) {
     with_garbage = false;  
   } else if ( ! strcmp(argv[i],"--gc-threshold") ) {
      if (++i > argc) 
       { cerr << "give numeric value in Kb for gc-threshold option " << argv[i-1]<<endl;  showUsageHelp() ; return false;}
      int threshold = atoi(argv[i]);
      MemoryManager::setGCThreshold (threshold);
   } else if ( ! strcmp(argv[i],"--fixpoint") ) {
      if (++i > argc) 
	{ cerr << "Expected one of {DFS|BFS} for fixpoint option :" << argv[i-1]<<endl;  showUsageHelp() ; return false;}
      if (  ! strcmp(argv[i],"DFS")) {
	Shom::setFixpointStrategy(Shom::DFS);
      } else if  (  ! strcmp(argv[i],"BFS")) {
	Shom::setFixpointStrategy(Shom::BFS);
      } else {
	cerr << "Expected one of {DFS|BFS} for fixpoint option :" << argv[i-1]<<endl;  showUsageHelp() ; return false;
      }
      /** LEFTOVER OPTIONS */
   } else {
     argsleft.push_back(argv[i]);
   }
    
  }

  /** return unparsed options */
  argv = argsleft;
  return true;
}

void usageSDDOptions() {
    cerr << " SDD specific options : " <<endl;
    cerr <<  "    --no-garbage : disable garbage collection (may be faster, more memory)" <<endl;
    cerr <<  "    --gc-threshold INT : set the threshold for first starting to do gc [DEFAULT:13000 kB=1.3GB]" <<endl;
    cerr <<  "    --fixpoint {BFS,DFS} : this options controls which kind of saturation algorithm is applied. Both are variants of saturation not really full DFS or BFS. [default: BFS]" <<endl;    
}


}
