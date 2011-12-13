#include "Options.hh"

#include <string>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <fstream>

// DL sym stuff
#include <dlfcn.h>


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

/** Consumes the options that are recognized in args, and treats them to build the Model.
 *  Options recognized by this options parser: 
 MANDATORY : -i Inputfile -t {CAMI|PROD|ROMEO|ITSXML|ETF} 
 OPTIONAL : --load-order {$f.json|$f.ord}      NB:extension of file is used to determine its type
 CONFIGURATION : --ddd|--sdd : privilege ddd or sdd in encoding. sdd is default
 For Scalar and circular : -ssD2, -ssDR, -ssDS   (default -ssD2 1)
*/
bool handleInputOptions (std::vector<const char *> & argv, ITSModel & model) {

  string pathinputff = "";
  enum InputType {NDEF,CAMI,PROD,ROMEO,ITSXML,ETF,DLL,NDLL,DVE};
  InputType parse_t = NDEF;

  bool hasOrder=false;
  string pathorderff;

  bool hasJson = false;
  string pathjsonff;

  // For use in NDLL case
  int Nsize = -1;

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
     } else {
       cerr << "Unrecognized type "<< argv[i] <<" provided for input file after " << argv[i-1] << " one of {CAMI|PROD|ROMEO|ITSXML|ETF} is expected. " << endl;  showUsageHelp() ;exit(1);
     }

     /** ORDER FILE OPTIONS */
   } else if ( ! strcmp(argv[i],"--json-order") ) {
     if (++i > argc) 
       { cerr << "Give a file name containing a JSON variable order definition please after " << argv[i-1]<<endl;  showUsageHelp() ;exit(1);}
     pathjsonff = argv[i];
     hasJson = true;
   } else if ( ! strcmp(argv[i],"--load-order") ) {
     if (++i > argc) 
       { cerr << "Give a file name containing a variable order definition please after " << argv[i-1]<<endl;  showUsageHelp() ;exit(1);}
     pathorderff = argv[i];
     hasOrder = true;


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
    {
      TPNet * pnet = XMLLoader::loadXML(pathinputff, hasJson);
      
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
      const GAL * gal = loadDve2Gal(pathinputff);
      model.declareType (*gal);
      
      model.setInstance(gal->getName(), "main");
      model.setInstanceState("init");
      break;
    }
  case NDLL : {
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
    }
  case NDEF :
  default : 
    {
      std::cerr << "Please specify input problem type with option -t. Supported types are :  {CAMI|PROD|ROMEO|ITSXML|ETF} \n" ;
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
    cerr<<  "    -t {CAMI|PROD|ROMEO|ITSXML|ETF} : specifies format of the input model file : " <<endl;
    cerr<<  "             CAMI : CAMI format (for P/T nets) is the native Petri net format of CPN-AMI" <<endl;
    cerr<<  "             PROD : PROD format (for P/T nets) is the native format of PROD" <<endl;
    cerr<<  "             ROMEO : an XML format (for Time Petri nets) that is the native format of Romeo" <<endl;
    cerr<<  "             ITSXML : a native XML format (for ANY kind of ITS) for this tool. These files allow to point to other files." <<endl;
    cerr<<  "             ETF : Extended Table Format is the native format used by LTSmin, built from many front-ends." <<endl;
    cerr<<  "             DLL : use a dynamic library that provides a function \"void loadModel (Model &,int)\" typically written using the manipulation APIs. See demo/ folder." <<endl;
    cerr<<  "             NDLL : same as DLL, but expect input formatted as size:lib.so. See demo/ folder." <<endl;
    cerr<< "\nAdditional Options and Settings:" << endl;
    cerr << "    --load-order path : load the variable order from the file designated by path. This order file can be produced with --dump-order. Note this option is not exclusive of --json-order; the model is loaded as usual, then the provided order is applied a posteriori. \n" ;
    cerr<< "\nPetri net specific options :" << endl;
    cerr<<  "    --json-order path : use a JSON encoded hierarchy description file for a Petri net model (CAMI, PROD or ROMEO), such as produced using Neoppod heuristic ordering tools. Note that this option modifies the way the model is loaded. \n " <<endl;    
    cerr<<  "    --sdd : privilege SDD storage (Petri net models only).[DEFAULT]" <<endl;
    cerr<<  "    --ddd : privilege DDD (no hierarchy) encoding (Petri net models only)." <<endl;
    cerr<< "\nScalar and Circular symmetric composite types options:" << endl;
    cerr<<  "    -ssD2 INT : (depth 2 levels) use 2 level depth for scalar sets. Integer provided defines level 2 block size. [DEFAULT: -ssD2 1]" <<endl;
    cerr<<  "    -ssDR INT : (depth recursive) use recursive encoding for scalar sets. Integer provided defines number of blocks at highest levels." <<endl;
    cerr<<  "    -ssDS INT : (depth shallow recursive) use alternative recursive encoding for scalar sets. Integer provided defines number of blocks at lowest level.\n" <<endl;
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
