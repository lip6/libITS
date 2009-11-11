#include "ITSModel.hh"
#include "Modular2ITS.hh"
#include "JSON2ITS.hh"
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
  std::cerr << "Document options here :\n\t-p [PROD file]\n\t-c [CAMI file]\n\t-j [JSON hierarchy definition file]\n\t-ctl [CTL formulas file]\n\t[--forward]\n\t[--no-garbage]\n\t[--ddd]\n\n" << std::endl;
}

int main (int argc, char ** argv) {  
  vis_init();
  string modelName;
  string pathprodff;
  string pathformff;
  string pathcamiff;
  string pathjsonff;
  bool doprodparse = false;
  bool docamiparse = false;
  bool dojsonparse = false;
  
  bool dofwtranslation = false;
  bool dobwtranslation = false;

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
   } else if ( ! strcmp(argv[i],"-j") ) {
     if (++i > argc) 
       { cerr << "give argument value for JSON file name please after " << argv[i-1]<<endl; usage() ;exit(1);;}
     pathjsonff = argv[i];
     dojsonparse = true;
   } else if (! strcmp(argv[i],"-ctl") ) {
     if (++i > argc) 
       { cerr << "give argument value for .ctl formula file name please after " << argv[i-1]<<endl; usage() ; exit(1);;}
     pathformff = argv[i];
   } else if (! strcmp(argv[i],"--forward")   ) {
     dofwtranslation = true;
   } else if (! strcmp(argv[i],"--backward")   ) {
     dobwtranslation = true;
     
   } else if (! strcmp(argv[i],"--no-garbage")   ) {
     with_garbage = false;  
   } else if (! strcmp(argv[i],"--ddd")   ) {
     model.setStorage(ddd_storage);
   } else {
     cerr << "Error : incorrect Argument : "<<argv[i] <<endl ; usage(); exit(1);
   }
 }
 
 if (!dobwtranslation && !dofwtranslation) {
   std::cout << "No direction supplied, using forward translation only."<< std::endl;
   dofwtranslation = true;
 }

 if (pathformff == "" || ( pathprodff == "" && pathcamiff == "") ) {
   fprintf(stderr, " Please provide both a ctl file name with -ctl and and a model file with -p (PROD format) or -c/-j (Cami/Json) \n");
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
      std::cerr << "You need to specify a hierarchy configuration file in JSON format." << std::endl;
      exit(1);
    } else {
      JSONLoader::loadJsonCami (model, pathcamiff, pathjsonff);
//         model.print(std::cerr);
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

}
