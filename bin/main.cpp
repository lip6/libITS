#include <iostream>
#include <fstream>
#include <cstring>

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
  cerr << "Instantiable Transition Systems SDD/DDD Analyzer; package " << PACKAGE_STRING <<endl;
  cerr << "This tool performs state-space generation of ITS, including extended timed Petri Nets allowing \n"
       << "inhibitor,pre,post (hyper)arcs. " <<endl
       << " The reachability set is computed using SDD/DDD, the Hierarchical Set Decision Diagram library, \n"
       << " Please see README file enclosed \n"
       << "in the distribution for more details. Input is a ROMEO xml model file, or one of the hard coded demo examples\n"
       << "(see Samples dir for documentation and examples). \n \nOptions :" << endl;
  cerr<<  "    -i path : specifies the path to input Romeo model " <<endl;
  cerr<<  "    -p path : specifies the path to input Prod format model with possible module info pnddd style (xxx.net)" <<endl;
  cerr<<  "    -xml path : use a XML encoded ITSModel file, as produced by Coloane.\n" ;
  cerr<<  "    -c path : use a CAMI encoded ordinary P/T net, as produced by Coloane or Macao. Use -j in conjunction with this option.\n" ;
  cerr<<  "    -j path : use a JSON encoded hierarchy description file for a CAMI model, as produced using PaToH.\n" ;
  cerr << "    --dump-order path : dump the currently used variable order to file designated by path and exit. \n" ;
  cerr << "    --load-order path : load the variable order from the file designated by path. \n" ;
  cerr<<  "    -d path : specifies the path prefix to construct dot state-space graph" <<endl;
  cerr<<  "    --sdd : privilege SDD storage." <<endl;
  cerr<<  "    --ddd : privilege DDD (no hierarchy) encoding.[DEFAULT]" <<endl;
  cerr<<  "    -bmc XXX : use limited depth BFS exploration, up to XXX steps from initial state." << endl;
  cerr<<  "    --no-garbage : disable garbage collection (may be faster, more memory)" <<endl;
  cerr<<  "    -ssD2 INT : use 2 level depth for scalar sets. Integer provided defines level 2 block size." <<endl;
  cerr<<  "    -ssDR INT : use recursive encoding for scalar sets. Integer provided defines number of blocks at highest levels." <<endl;
  cerr<<  "    -ssDS INT : use alternative recursive encoding for scalar sets. Integer provided defines number of blocks at lowest level." <<endl;
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

 string pathromeoff;
 string pathprodff;
 string pathXMLff;
 string pathCAMIff;
 string pathjsonff;
 string pathorderinff = "order";
 bool doromeoparse = false;
 bool doprodparse = false;
 bool doXMLITSparse = false;
 bool doCAMIparse = false;
 bool dojsonparse = false;
 bool doloadorder = false;

 its::storage method = ddd_storage ;
 // parse command line args to get the options

 // instanciate a Model
 ITSModel model;

 
 for (int i=1;i < argc; i++) {
   if ( ! strcmp(argv[i],"-i") ) {
     if (++i > argc) 
       { cerr << "give argument value for romeo xml file name please after " << argv[i-1]<<endl; usage() ;exit(1);}
     pathromeoff = argv[i];
     doromeoparse = true;
   } else if ( ! strcmp(argv[i],"-p") ) {
     if (++i > argc) 
       { cerr << "give argument value for Prod file name please after " << argv[i-1]<<endl; usage() ;exit(1);}
     pathprodff = argv[i];
     doprodparse = true;
   } else if ( ! strcmp(argv[i],"-xml") ) {
     if (++i > argc) 
       { cerr << "give argument value for ITSModel XML file name please after " << argv[i-1]<<endl; usage() ;exit(1);}
     pathXMLff = argv[i];
     doXMLITSparse = true;
   } else if ( ! strcmp(argv[i],"-c") ) {
     if (++i > argc) 
       { cerr << "give argument value for CAMI file name please after " << argv[i-1]<<endl; usage() ;exit(1);}
     pathCAMIff = argv[i];
     doCAMIparse = true;
   } else if ( ! strcmp(argv[i],"-j") ) {
     if (++i > argc) 
       { cerr << "give argument value for JSON file name please after " << argv[i-1]<<endl; usage() ;exit(1);}
     pathjsonff = argv[i];
     dojsonparse = true;
   } else if (! strcmp(argv[i],"-d") ) {
     if (++i > argc) 
       { cerr << "give argument value for .dot file name please after " << argv[i-1]<<endl; usage() ; exit(1);}
     pathdotff = argv[i];
     dodotexport = true;
   } else if (! strcmp(argv[i],"-bmc") ) {
     if (++i > argc) 
       { cerr << "give argument value for BMC depth " << argv[i-1]<<endl; usage() ; exit(1);}
     BMC = atoi(argv[i]); 
   } else if (! strcmp(argv[i],"-ssD2") ) {
     if (++i > argc) 
       { cerr << "give argument value for scalar strategy " << argv[i-1]<<endl; usage() ; exit(1);}
     int grain = atoi(argv[i]);
     model.setScalarStrategy(DEPTH1,grain);
   }else if (! strcmp(argv[i],"-ssDR") ) {
     if (++i > argc) 
       { cerr << "give argument value for scalar strategy " << argv[i-1]<<endl; usage() ; exit(1);}
     int grain = atoi(argv[i]);
     model.setScalarStrategy(DEPTHREC,grain);   
   }else if (! strcmp(argv[i],"-ssDS") ) {
     if (++i > argc) 
       { cerr << "give argument value for scalar strategy " << argv[i-1]<<endl; usage() ; exit(1);}
     int grain = atoi(argv[i]);
     model.setScalarStrategy(SHALLOWREC,grain);
   } else if (! strcmp(argv[i],"--help") || ! strcmp(argv[i],"-h")  ) {
     usage(); exit(0);
   } else if (! strcmp(argv[i],"--quiet")   ) {
     beQuiet = true;
   } else if (! strcmp(argv[i],"--ddd")   ) {
     method = ddd_storage;
     modelName = "d3:";
     model.setStorage(ddd_storage);
   } else if (! strcmp(argv[i],"--sdd")   ) {
     method = sdd_storage ;
     model.setStorage(sdd_storage);
   } else if (! strcmp(argv[i],"--no-garbage")   ) {
     with_garbage = false;  
   } else if (! strcmp(argv[i],"--dump-order")   ) {
     if (++i > argc) 
       { cerr << "give path value for dump-order " << argv[i-1]<<endl; usage() ; exit(1);}
     pathorderff = argv[i];     
     dodumporder = true;
   } else if (! strcmp(argv[i],"--load-order")   ) {
     if (++i > argc) 
       { cerr << "give path value for load-order " << argv[i-1]<<endl; usage() ; exit(1);}
     pathorderinff = argv[i];     
     doloadorder = true;
   } else {
     cerr << "Error : incorrect Argument : "<<argv[i] <<endl ; usage(); exit(0);
   }
 }
 
 
 if (doprodparse) {
   vLabel nname = RdPELoader::loadModularProd(model,pathprodff);
   modelName += pathprodff ;
   model.setInstance(nname, "main");
   model.setInstanceState("init");
 } else if ( doromeoparse) {
   // Parse the input file to build the system
   
   TPNet * pnet = XMLLoader::loadXML(pathromeoff, dojsonparse);
   if (dojsonparse) {
     json::Hierarchie * hier = new json::Hierarchie();
     json::json_parse(pathjsonff, *hier);
     model.declareType(*pnet,hier);
     //     model.print(std::cerr);
   } else {
     model.declareType(*pnet);
     modelName += pathromeoff ;
   }
   model.setInstance(pnet->getName(),"main");
   model.setInstanceState("init");	  
 } else if (doXMLITSparse) {
   ITSModelXMLLoader::loadXML(pathXMLff, model);
 } else if (doCAMIparse) {
    if (! dojsonparse) {
      std::cerr << "You need to specify a hierarchy configuration file in JSON format." << std::endl;
      exit(1);
    } else {
      JSONLoader::loadJsonCami (model, pathCAMIff, pathjsonff);
//         model.print(std::cerr);
    } 
 } else {
   std::cerr << "Please use -i, -p, -xml or -c option to specify input problem.\n" ;
   usage();
   exit(1);
 } 

 if (doloadorder) {
   ifstream is (pathorderinff.c_str());
   if (! model.loadOrder(is)) {
     std::cerr << "Problem loading provided order file :" << pathorderinff << "\n";
     exit(1);
   }
 }
	
 exhibitModel(model);

 return 0;
}

