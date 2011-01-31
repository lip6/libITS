#include <fstream>
// The ITS model referential
#include "ITSModel.hh"
#include "SDD.h"
// romeo parser
#include "petri/XMLLoader.hh"
// prod parser
#include "petri/Modular2ITS.hh"
// ITSModel parser
#include "ITSModelXMLLoader.hh"
// Cami parser
#include "petri/JSON2ITS.hh"

// SDD utilities to output stats and dot graphs
#include "util/dotExporter.h"
#include "statistic.hpp"

#include "ctlCheck.hh"

#include "visfd.h"
#include "ctlp.h"
#include "ctlpInt.h"

using std::string;
using std::cerr;
using std::endl;
using namespace its;

static bool with_garbage=true;

void usage () {
  cerr << "Instantiable Transition Systems SDD/DDD CTL Analyzer; package " << PACKAGE_STRING <<endl;
  cerr << "This tool performs CTL verification ostate-space of ITS, including extended timed Petri Nets allowing \n"
       << "inhibitor,pre,post (hyper)arcs. " <<endl
       << " The reachability set is computed using SDD/DDD, the Hierarchical Set Decision Diagram library, \n"
       << " Please see README file enclosed \n"
       << "in the distribution for more details. Input is a ROMEO xml model file, a CAMI/JSON pair or an ITS XML set of files as prouced through Coloane.\n"
       << "(see Samples dir for documentation and examples). \n \nOptions :" << endl;
  cerr<<  "    -i path : specifies the path to input Romeo model " <<endl;
  cerr<<  "    -p path : specifies the path to input Prod format model with possible module info pnddd style (xxx.net)" <<endl;
  cerr<<  "    -xml path : use a XML encoded ITSModel file, as produced by Coloane or Romeo.\n" ;
  cerr<<  "    -c path : use a CAMI encoded ordinary P/T net, as produced by Coloane or Macao. Use -j in conjunction with this option.\n" ;
#ifdef HAVE_BOOST_FLAG  
  cerr<<  "    -j path : use a JSON encoded hierarchy description file for a CAMI model, as produced using PaToH.\n" ;
#endif
  cerr<<  "    -ctl [CTL formulas file]\n";
  cerr<<  "    [--forward] to force forward CTL model-checking (default)\n";
  cerr<<  "    [--backward] to force backward CTL model-checking (classic algorithm from 10^20 states & beyond)\n";
  cerr << "    --dump-order : dump the currently used variable order to stdout and exit. \n" ;
  cerr << "    --load-order path : load the variable order from the file designated by path. \n" ;
  cerr<<  "    --sdd : privilege SDD storage.[DEFAULT]" <<endl;
  cerr<<  "    --ddd : privilege DDD (no hierarchy) encoding." <<endl;
  cerr<<  "    --no-garbage : disable garbage collection (may be faster, more memory)" <<endl;
  cerr<<  "    -ssD2 INT : use 2 level depth for scalar sets. Integer provided defines level 2 block size." <<endl;
  cerr<<  "    -ssDR INT : use recursive encoding for scalar sets. Integer provided defines number of blocks at highest levels." <<endl;
  cerr<<  "    -ssDS INT : use alternative recursive encoding for scalar sets. Integer provided defines number of blocks at lowest level." <<endl;
  cerr<<  "    --quiet : limit output verbosity useful in conjunction with tex output --texline for batch performance runs" <<endl;
  cerr<<  "    --legend : show full table legend" <<endl;
  cerr<<  "    --help,-h : display this (very helpful) helping help text"<<endl;
  cerr<<  "Problems ? Comments ? contact " << PACKAGE_BUGREPORT <<endl;
}

int main (int argc, char ** argv) {  
  vis_init();
  string modelName;
  string pathprodff;
  string pathformff;
  string pathcamiff;
  string pathjsonff;
  string pathxmlff;
  string pathorderinff = "order";
  bool doprodparse = false;
  bool docamiparse = false;
  bool dojsonparse = false;
  bool doxmlparse = false;
  bool doloadorder = false;
  bool bequiet = false;

  bool dofwtranslation = false;
  bool dobwtranslation = false;

  bool showlegend = false;
  /// Parsed Model
  ITSModel model;

  
 for (int i=1;i < argc; i++) {
   if ( ! strcmp(argv[i],"-p") ) {
     if (++i > argc) 
       { cerr << "give argument value for Prod file name please after " << argv[i-1]<<endl; usage() ;exit(1);;}
     pathprodff = argv[i];
     doprodparse = true;
   } else if ( ! strcmp(argv[i],"-c") ) {
     if (++i > argc) 
       { cerr << "give argument value for Cami file name please after " << argv[i-1]<<endl; usage() ;exit(1);;}
     pathcamiff = argv[i];
     docamiparse = true;
#ifdef HAVE_BOOST_FLAG  
   } else if ( ! strcmp(argv[i],"-j") ) {
     if (++i > argc) 
       { cerr << "give argument value for JSON file name please after " << argv[i-1]<<endl; usage() ;exit(1);;}
     pathjsonff = argv[i];
     dojsonparse = true;
#endif
   } else if ( ! strcmp(argv[i],"-xml") ) {
     if (++i > argc) 
       { cerr << "give argument value for ITSModel XML file name please after " << argv[i-1]<<endl; usage() ;exit(1);}
     pathxmlff = argv[i];
     doxmlparse = true;
   } else if (! strcmp(argv[i],"-ctl") ) {
     if (++i > argc) 
       { cerr << "give argument value for .ctl formula file name please after " << argv[i-1]<<endl; usage() ; exit(1);;}
     pathformff = argv[i];
   } else if (! strcmp(argv[i],"--legend")   ) {
     showlegend = true;
   } else if (! strcmp(argv[i],"--forward")   ) {
     dofwtranslation = true;
   } else if (! strcmp(argv[i],"--backward")   ) {
     dobwtranslation = true;
     
   } else if (! strcmp(argv[i],"--no-garbage")   ) {
     with_garbage = false;  
   } else if (! strcmp(argv[i],"--ddd")   ) {
     model.setStorage(ddd_storage);
   }
   /* Avoid arror argument when user uses --sdd
    * By defaut, codage is SDD is ITSModel
    */
   else if (! strcmp(argv[i],"--sdd")   ) {
         model.setStorage(sdd_storage);
   } else if (! strcmp(argv[i],"--quiet")   ) {
     bequiet = true;
   } else if (! strcmp(argv[i],"--load-order")   ) {
     if (++i > argc) 
       { cerr << "give path value for load-order " << argv[i-1]<<endl; usage() ; exit(1);}
     pathorderinff = argv[i];     
     doloadorder = true;
   } else {
     cerr << "Error : incorrect Argument : "<<argv[i] <<endl ; usage(); exit(1);
   }
 }
 
 if (!dobwtranslation && !dofwtranslation) {
   std::cout << "No direction supplied, using forward translation only."<< std::endl;
   dofwtranslation = true;
 }

 if (pathformff == "" || ( pathprodff == "" && pathcamiff == "" && pathxmlff == "") ) {
   fprintf(stderr, " Please provide both a ctl file name with -ctl and and a model file with -p (PROD format) or -c/-j (Cami/Json) or -xml (XML Coloane mainModel.xml file) \n");
   return 1;
 }

  FILE * fp = fopen(pathformff.c_str(), "r");
  if (fp == NIL(FILE)) {
    (void)fprintf(vis_stderr, "** ctl error: Cannot open the file %s\n", argv[util_optind]);
    return 1;
  }


  if (doprodparse) {
    vLabel nname = RdPELoader::loadModularProd(model,pathprodff);
    modelName += pathprodff ;
    model.setInstance(nname, "main");
    model.setInstanceState("init");
  } else if (docamiparse) {
    if (! dojsonparse) {
#ifdef HAVE_BOOST_FLAG  
      std::cerr << "You need to specify a hierarchy configuration file in JSON format." << std::endl;
#else
      std::cerr << "BOOST which is necessary to parse JSON files has not been found on your system.." << std::endl;
#endif
      exit(1);
    } else {
#ifdef HAVE_BOOST_FLAG  
      JSONLoader::loadJsonCami (model, pathcamiff, pathjsonff);
//         model.print(std::cerr);
#endif
    }
  } else if (doxmlparse) {
    ITSModelXMLLoader::loadXML(pathxmlff, model);
  }

  if (! bequiet)
    std::cout << model << std::endl;


  if (doloadorder) {
    ifstream is (pathorderinff.c_str());
    if (! model.loadOrder(is)) {
      std::cerr << "Problem loading provided order file :" << pathorderinff << "\n";
      exit(1);
    }
  }

  // Build CTL context
  CTLChecker checker (model);

  /*
   * Parse the formulas in the file.
   */
  array_t *formulaArray = Ctlp_FileParseFormulaArray(fp);
  (void) fclose(fp);

  if (formulaArray == NIL(array_t)) {
    fflush(vis_stdout);
    fflush(vis_stderr);
    return 1;
  }
  
  array_t *forwardExistentialArray = NULL;
  if (dofwtranslation)
    forwardExistentialArray =
      Ctlp_FormulaArrayConvertToForward(formulaArray, 1, FALSE);

  array_t * convertedArray = Ctlp_FormulaArrayConvertToDAG(formulaArray);
  array_free(formulaArray);
  array_t * existentialConvertedArray =
    Ctlp_FormulaDAGConvertToExistentialFormDAG(convertedArray);
  

  // Compute reachable states
  State reachable = model.computeReachable(with_garbage);
  // Some traces
  //std::cout << "Transition relation " << model.getNextRel() << std::endl ;
  //std::cout << "Reverse Transition relation " << model.getNextRel().invert(reachable) << std::endl ;
 

  Statistic SS3 = Statistic(reachable, "reachable" , CSV);
  SS3.print_table(std::cout);
  std::cout << "\n\n";
  /*
   * Print each original formula and its corresponding converted formula.
   */
  for (int i = 0; i < array_n(convertedArray); i++) {
    Ctlp_Formula_t *formula;

    formula = array_fetch(Ctlp_Formula_t *, convertedArray, i);
    (void) fprintf(vis_stdout, "original formula: ");
    Ctlp_FormulaPrint(vis_stdout, formula);
    (void) fprintf(vis_stdout, "\n");

    if (dobwtranslation) {
      formula = array_fetch(Ctlp_Formula_t *, existentialConvertedArray, i);
      (void) fprintf(vis_stdout, "=> equivalent existential formula: ");
      Ctlp_FormulaPrint(vis_stdout, formula);
      (void) fprintf(vis_stdout, "\n");
      
      /*
	its::Transition formHom = checker.getHomomorphism(formula);
	//    std::cout << "Formula as homomorphism : " << formHom << std::endl;
	
	its::State verif = formHom (reachable) * reachable ;
	Statistic SS = Statistic(verif, "formula "+ to_string(i) , CSV); // can also use LATEX instead of CSV
	SS.print_line(std::cout);
      */
      
      its::State verif2 = checker.getStateVerifying(formula) ;
      Statistic SS2 = Statistic(verif2, "(state)formula "+ to_string(i) , CSV); // can also use LATEX instead of CSV
      SS2.print_line(std::cout);
      if (verif2 * checker.getInitialState() == checker.getInitialState() ) {
       std::cout << "Formula is TRUE !" << std::endl;
      } else {
	std::cout << "Formula is FALSE !" << std::endl;
      }
      //    std::cout << verif2 << std::endl;
      //   exportDot(verif2,"form.dot");
    }

    if (dofwtranslation) {
      formula = array_fetch(Ctlp_Formula_t *, forwardExistentialArray, i);
      (void) fprintf(vis_stdout, "=> equivalent forward existential formula: ");
      Ctlp_FormulaPrint(vis_stdout, formula);
      (void) fprintf(vis_stdout, "\n");
      
      its::State verif3 = checker.getStateVerifying(formula) ;
      Statistic SS3 = Statistic(verif3, "(forward)formula "+ to_string(i) , CSV); // can also use LATEX instead of CSV
      SS3.print_line(std::cout);
      if (verif3 == State::one) {
	std::cout << "Formula is TRUE !" << std::endl;
      } else {
	std::cout << "Formula is FALSE !" << std::endl;
      }

    }

    std::cout << "\n *************************************** \n\n";
  }

  if (showlegend) {
    SS3.print_legend(std::cout);
  }
}
