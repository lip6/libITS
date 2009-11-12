#include <cstring>

// The ITS model referential
#include "ITSModel.hh"
#include "SDED.h"
// The Time Petri net declaration
#include "TPNet.hh"
#include "examples.hh"
#include "train.hh"
#include "fischer.hh"
#include "CSMACD.hh"
// romeo parser
#include "XMLLoader.hh"
// prod parser
#include "Modular2ITS.hh"
// ITSModel parser
#include "ITSModelXMLLoader.hh"

// SDD utilities to output stats and dot graphs
#include "util/dotExporter.h"
#include "statistic.hpp"

using namespace its;

static bool beQuiet = false;
static string pathdotff = "final";
static bool dodotexport=false;
static bool dodumporder = false;
static bool with_garbage=true;
static std::string modelName = "";

void exhibitModel (ITSModel & model) {
	// pretty print the model for inspection
  if (! beQuiet) {
    std::cout << model << std::endl;
  
    std::cout << model.getInitialState() << std::endl;
  }
  if (dodumporder) {
    model.printVarOrder(std::cout);
    exit(0);
  }
  // Compute reachable states
  State reachable = model.computeReachable(with_garbage);
  if (!beQuiet && reachable.nbStates() < 10)
    std::cout << reachable << std::endl;	
  // Print some stats : memory size, number of states ....
  Statistic S = Statistic(reachable, modelName , CSV); // can also use LATEX instead of CSV
  S.print_table(std::cout);

  // Export the SDD of final states to dot : generates files final.dot and d3final.dot
  if (dodotexport)
    exportDot(reachable,pathdotff);
}


void usage() {
  cerr << "Timed Petri Net SDD/DDD Analyzer; package " << PACKAGE_STRING <<endl;
  cerr << "This tool performs state-space generation of extended timed Petri Nets allowing \n"
       << "inhibitor,pre,post (hyper)arcs. " <<endl
       << " The reachability set is computed using SDD/DDD, the Hierarchical Set Decision Diagram library, \n"
       << " Please see README file enclosed \n"
       << "in the distribution for more details. Input is a ROMEO xml model file, or one of the hard coded demo examples\n"
       << "(see Samples dir for documentation and examples). \n \nOptions :" << endl;
  cerr<<  "    -i path : specifies the path to input Romeo model " <<endl;
  cerr<<  "    -p path : specifies the path to input Prod format model with possible module info pnddd style (xxx.net)" <<endl;
  cerr<<  "    -e exampleid : use a hard coded example. exampleid is an int, see HARDEXAMPLES.txt for details on this option.\n" ;
  cerr<<  "    -xml path : use a XML encoded ITSModel file, as produced by Coloane or Romeo.\n" ;
  cerr << "    --order : in conjunction with -e, use a custom order for some examples. \n" ;
  cerr << "    --dump-order : dump the currently used variable order to stdout and exit. \n" ;
  cerr<<  "    -d path : specifies the path prefix to construct dot state-space graph" <<endl;
  cerr<<  "    --sdd : privilege SDD storage." <<endl;
  cerr<<  "    --ddd : privilege DDD (no hierarchy) encoding.[DEFAULT]" <<endl;
  cerr<<  "    --no-garbage : disable garbage collection (may be faster, more memory)" <<endl;
  cerr<<  "    -ssD2 INT : use 2 level depth for scalar sets. Integer provided defines level 2 block size." <<endl;
  cerr<<  "    -ssDR INT : use recursive encoding for scalar sets. Integer provided defines number of blocks at highest levels." <<endl;
  cerr<<  "    -ssDS INT : use alternative recursive encoding for scalar sets. Integer provided defines number of blocks at lowest level." <<endl;
  cerr<<  "    --texhead : print tex header"  << endl;
  cerr<<  "    --texline : print tex line " <<endl ;
  cerr<<  "    --textail : print tex tail"  << endl;
  cerr<<  "    --tex : print full tex description" <<endl;
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
 bool doFullTex;
 bool doTexLine;
 bool doromeoparse = false;
 bool doprodparse = false;
 bool doXMLITSparse = false;
 bool dosimpleexample=false;
 bool usecustomorder=false;
 int numexample=0;
 its::storage method = ddd_storage ;
 // parse command line args to get the options

 // instanciate a Model
 ITSModel model;

 
 for (int i=1;i < argc; i++) {
   if ( ! strcmp(argv[i],"-i") ) {
     if (++i > argc) 
       { cerr << "give argument value for romeo xml file name please after " << argv[i-1]<<endl; usage() ;exit(1);;}
     pathromeoff = argv[i];
     doromeoparse = true;
   } else if ( ! strcmp(argv[i],"-p") ) {
     if (++i > argc) 
       { cerr << "give argument value for Prod file name please after " << argv[i-1]<<endl; usage() ;exit(1);;}
     pathprodff = argv[i];
     doprodparse = true;
   } else if ( ! strcmp(argv[i],"-xml") ) {
     if (++i > argc) 
       { cerr << "give argument value for ITSModel XML file name please after " << argv[i-1]<<endl; usage() ;exit(1);;}
     pathXMLff = argv[i];
     doXMLITSparse = true;
   } else if (! strcmp(argv[i],"-d") ) {
     if (++i > argc) 
       { cerr << "give argument value for .dot file name please after " << argv[i-1]<<endl; usage() ; exit(1);;}
     pathdotff = argv[i];
     dodotexport = true;
   } else if (! strcmp(argv[i],"-e") ) {
     if (++i > argc) 
       { cerr << "give integer argument value for example file name please after " << argv[i-1]<<endl; usage() ; exit(1);;}
     numexample = atoi(argv[i]);
     dosimpleexample = true;
   } else if (! strcmp(argv[i],"-ssD2") ) {
     if (++i > argc) 
       { cerr << "give argument value for scalar strategy " << argv[i-1]<<endl; usage() ; exit(1);;}
     int grain = atoi(argv[i]);
     model.setScalarStrategy(DEPTH1,grain);
   }else if (! strcmp(argv[i],"-ssDR") ) {
     if (++i > argc) 
       { cerr << "give argument value for scalar strategy " << argv[i-1]<<endl; usage() ; exit(1);;}
     int grain = atoi(argv[i]);
     model.setScalarStrategy(DEPTHREC,grain);   
   }else if (! strcmp(argv[i],"-ssDS") ) {
     if (++i > argc) 
       { cerr << "give argument value for scalar strategy " << argv[i-1]<<endl; usage() ; exit(1);;}
     int grain = atoi(argv[i]);
     model.setScalarStrategy(SHALLOWREC,grain);
   } else if (! strcmp(argv[i],"--help") || ! strcmp(argv[i],"-h")  ) {
     usage(); exit(0);
   } else if (! strcmp(argv[i],"--texhead")   ) {
     Statistic s = Statistic(SDD::one,"");
     s.print_header(cout);
     exit(0);
   } else if (! strcmp(argv[i],"--textail")   ) {
     Statistic s = Statistic(SDD::one,"");
	      s.print_trailer(cout);
	      exit(0);
   } else if (! strcmp(argv[i],"--tex")   ) {
     doFullTex = true;
   } else if (! strcmp(argv[i],"--texline")   ) {
     doTexLine = true;
   } else if (! strcmp(argv[i],"--quiet")   ) {
     beQuiet = true;
   } else if (! strcmp(argv[i],"--ddd")   ) {
     method = ddd_storage;
     modelName = "d3:";
     model.setStorage(ddd_storage);
   } else if (! strcmp(argv[i],"--order")   ) {
     usecustomorder = true;
   } else if (! strcmp(argv[i],"--sdd")   ) {
     method = sdd_storage ;
     model.setStorage(sdd_storage);
   } else if (! strcmp(argv[i],"--no-garbage")   ) {
     with_garbage = false;  
   } else if (! strcmp(argv[i],"--dump-order")   ) {
     dodumporder = true;
   } else {
     cerr << "Error : incorrect Argument : "<<argv[i] <<endl ; usage(); exit(0);
   }
 }
 
 
 if ( dosimpleexample ) {
   
   if (numexample > 100 && numexample < 200) {
     int nbtrains = numexample - 100;
     modelName += "trains " + to_string(nbtrains);
     loadTrains(nbtrains,model);
     // Update the model to point at this model type as main instance
     model.setInstance("Trains","main");
     // The only state defined in the type "trains" is "init"
     // This sets the initial state of the main instance
     model.setInstanceState("init");
   } else if ( (numexample > 200 && numexample < 300) || numexample > 1000 ) {
     int nbtrains = numexample - 200;
     if (nbtrains > 800)
       nbtrains -= 800;
     modelName += "fischer " + to_string(nbtrains);
     loadFischer(nbtrains,model);
     // Update the model to point at this model type as main instance
     model.setInstance("Fischer","main");
     // The only state defined in the type "trains" is "init"
     // This sets the initial state of the main instance
     model.setInstanceState("init");
   } else if (numexample > 300 && numexample < 400) {
     int nbtrains = numexample - 300;
     modelName += "fischer2pow " + to_string(nbtrains);
     loadFischer(nbtrains,model,true);
     // Update the model to point at this model type as main instance
     model.setInstance("Fischer","main");
     // The only state defined in the type "trains" is "init"
     // This sets the initial state of the main instance
     model.setInstanceState("init");
   } else if (numexample > 400 && numexample < 500) {
     int nbtrains = numexample - 400;
     modelName += "train2pow " + to_string(nbtrains);
     loadTrains(nbtrains,model,true);
     // Update the model to point at this model type as main instance
     model.setInstance("Trains","main");
     // The only state defined in the type "trains" is "init"
     // This sets the initial state of the main instance
     model.setInstanceState("init");
   } else if (numexample > 500 && numexample < 600) {
     int nbtrains = numexample - 500;
     modelName += "csmacd " + to_string(nbtrains);
     loadCsmacd(nbtrains,model);
     // Update the model to point at this model type as main instance
     model.setInstance("CSMACD","main");
     // The only state defined in the type "trains" is "init"
     // This sets the initial state of the main instance
     model.setInstanceState("init");
   } else {
     
     // build a Time Petri Net
     // The name of the type
     TPNet net ("test");
     
     if (numexample == 0) {
       // A test for inhibitor hyper arcs
       loadTrivial(net);
       modelName += "trivial " ;
     } else if (numexample == 1) {
       // Morgan's Ex1
       modelName += " ex1" ;
       loadExample1(net);
     } else if (numexample == 2) {
       // Morgan's Ex2
       modelName += " ex2" ;
       loadExample2(net);
     } else if (numexample == 3 ) {
       // Morgan's Ex
       modelName += " ex3" ;
       loadExample3(net);
     } else if (numexample == 14) {
       // Morgan's Ex14
       modelName += " ex14" ;
       loadExample14(net);
     } 
     
     /// Finished building the net
       model.declareType (net);
       
       // For custom var order
       if (usecustomorder) {
	 modelName += " :ord";
	 if (numexample == 1) {
	   // use with example 1
	   updateOrderEx1(model);
	 } else if (numexample == 3) {
	   // use with example 3
	   updateOrderEx3(model);
	 }
       }
       
       // Update the model to point at this model type as main instance
       model.setInstance("test","main");
       // The only state defined in the type "test" is "init"
       // This set the initial state of the main instance
       model.setInstanceState("init");
   } // small examples
 } else if (doprodparse) {
   vLabel nname = RdPELoader::loadModularProd(model,pathprodff);
   modelName += pathprodff ;
   model.setInstance(nname, "main");
   model.setInstanceState("init");
 } else if ( doromeoparse) {
   // No -e option
   // Parse the input file to build the system
   
   TPNet * pnet = XMLLoader::loadXML(pathromeoff);
   model.declareType(*pnet);
   modelName += pathromeoff ;
   model.setInstance(pnet->getName(),"main");
   model.setInstanceState("init");	  
 } else if (doXMLITSparse) {
   ITSModelXMLLoader::loadXML(pathXMLff, model);
 } else {
   std::cerr << "Please use -i, -p, -xml or -e option to specify input problem.\n" ;
   usage();
   exit(1);
 } 
	
 exhibitModel(model);
// SDED::pstats(false);
 return 0;
}

