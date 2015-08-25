#include <fstream>
// The ITS model referential
#include "ITSModel.hh"
#include "Options.hh"
#include "SDD.h"

// SDD utilities to output stats and dot graphs
#include "util/dotExporter.h"
#include "statistic.hpp"

#include "mc/ctlCheck.hh"

#include "ctlp/visfd.h"
#include "ctlp/ctlp.h"
#include "ctlp/ctlpInt.h"

using std::string;
using std::cerr;
using std::endl;
using namespace its;


void usage () {
  cerr << "Instantiable Transition Systems SDD/DDD CTL Analyzer;" <<endl;
  cerr << "Mandatory options : -i -t to provide input model, -ctl to provide formulae" << std::endl; 
  usageInputOptions();
  usageSDDOptions();

  cerr << "This tool performs CTL verification on state-space of ITS" <<endl;
  cerr << " CTL specific options for  package " << PACKAGE_STRING << endl;
  cerr<<  "    -ctl [CTL formulas file]  MANDATORY : give path to a file containing CTL formulae \n";
  cerr<<  " Optionally, if the [CTL formulas file] provided is the string DEADLOCK, the tool will compute and return the number of deadlocks.\n";
  cerr<<  "    --witness to ask for a witness/counter-example path to be produced (may be much more difficult than just proving/disproving)\n";
  cerr<<  "    --precise to ask for more precise counter example traces, that include states\n";
  cerr<<  "    --fair-time to disallow infinite loops over elapse. More precisely, with this option, time elapse is applied directly after each discrete transition firing (and on initial state).\n";
  cerr<<  "    [--forward] to force forward CTL model-checking (default)\n";
  cerr<<  "    [--forward] to force forward CTL model-checking (default)\n";
  cerr<<  "    [--backward] to force backward CTL model-checking (classic algorithm from 10^20 states & beyond)\n";
  cerr<<  "    --quiet : limit output verbosity useful in conjunction with tex output --texline for batch performance runs" <<endl;
  cerr<<  "    --legend : show full table legend" <<endl;
  cerr<<  "    --help,-h : display this (very helpful) helping help text"<<endl;
  cerr<<  "Problems ? Comments ? contact " << PACKAGE_BUGREPORT <<endl;
}

int main (int argc, char ** argv) {  

  vis_init();

  // instanciate a Model
  ITSModel model;
  
  // echo options of run
  std::cout << "its-ctl command run as :\n" << std::endl;
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
  if (! handleInputOptions (args, model) ) {
    usage();
    return 1;
  }
  // we now should have the model defined.
  string modelName = model.getInstance()->getType()->getName();
  
  bool with_garbage = true;
  // Setup SDD specific settings
  if (!handleSDDOptions (args, with_garbage)) {
    usage();
    return 1;
  }
  

  bool bequiet = false;

  bool dofwtranslation = false;
  bool dobwtranslation = false;

  bool doDeadlocks = false;

  bool doWitness = false;

  bool showlegend = false;

  bool befairtime = false;

  bool isPrecise = false;

  string pathformff;
  
  argc = args.size();
  for (int i=0;i < argc; i++) {
    if (! strcmp(args[i],"-ctl") ) {
      if (++i > argc) 
	{ cerr << "give argument value for .ctl formula file name please after " << args[i-1]<<endl; usage() ; exit(1);}
     pathformff = args[i];
   } else if (! strcmp(args[i],"--legend")   ) {
     showlegend = true;
   } else if (! strcmp(args[i],"--forward")   ) {
     dofwtranslation = true;
   } else if (! strcmp(args[i],"--backward")   ) {
     dobwtranslation = true;
   } else if (! strcmp(args[i],"--witness")   ) {
     doWitness = true;
   } else if (! strcmp(args[i],"--quiet")   ) {
     bequiet = true;
   } else if (! strcmp(args[i],"--precise")   ) {
     isPrecise = true;
   } else if (! strcmp(args[i],"--fair-time")   ) {
     befairtime = true;
   } else {
     cerr << "Error : incorrect Argument : "<<args[i] <<endl ; usage(); exit(1);
   }
 }
 
  if (!dobwtranslation && !dofwtranslation) {
    std::cout << "No direction supplied, using forward translation only."<< std::endl;
    dofwtranslation = true;
  }


  array_t *formulaArray = NULL;
  if (pathformff == "") {
    std::cerr << "Please provide an input file containing formulae with -ctl option. \n"<< std::endl;
    usage();
    return 1;
  } else if ( pathformff == "DEADLOCK" ) {
    doDeadlocks = true;
    dofwtranslation = false;
    dobwtranslation = false;
  } else {
    FILE * fp = fopen(pathformff.c_str(), "r");
    if (fp == NIL(FILE)) {
      (void)fprintf(vis_stderr, "** ctl error: Cannot open the file %s\n", pathformff.c_str());
      usage();
      return 1;
    } else {
      /*
       * Parse the formulas in the file.
       */
      formulaArray = Ctlp_FileParseFormulaArray(fp);
      (void) fclose(fp);
      
      if (formulaArray == NIL(array_t)) {
	fflush(vis_stdout);
	fflush(vis_stderr);
	return 1;
      } else {
	std::cout << "Parsed " << formulaArray->num << " CTL formulae." << std::endl;
      }
    }
  }


  if (! bequiet)
    std::cout << model << std::endl;


  // Build CTL context
  CTLChecker checker (model);

  if (befairtime) {
    checker.setFairTime(befairtime);
  }
  
  
  // Compute reachable states
  State reachable = model.computeReachable(with_garbage);
  // Some traces
  //std::cout << "Transition relation " << model.getNextRel() << std::endl ;
  //std::cout << "Reverse Transition relation " << model.getNextRel().invert(reachable) << std::endl ;
 

  Statistic SS3 = Statistic(reachable, "reachable" , CSV);
  SS3.print_table(std::cout);
  std::cout << "\n\n";

  if (doDeadlocks) {
    State dead = checker.getReachableDeadlocks();
    Statistic SSdead = Statistic(dead, "dead" , CSV);
    SSdead.print_table(std::cout);
    std::cout << "\n";

    std::cout << "System contains "<< dead.nbStates() << " deadlocks !" << std::endl;    

    if (doWitness &&dead.nbStates() > 0) {
      std::cout << "Computing a witness path..."<< std::endl;
      its::path_t path = checker.findPath(checker.getInitialState(), dead, checker.getReachable(), isPrecise);
      checker.printPath(path, std::cout, isPrecise);
      for (labels_it it = path.getPath().begin() ; it != path.getPath().end() ; ++it ) {
	std::cout << *it << ", ";
      }
      std::cout << "DEADLOCK"<< std::endl;
    }
    return 0;
  }

  /*
   * Print each original formula and its corresponding converted formula.
   */
  array_t *forwardExistentialArray = NULL;
  if (dofwtranslation) {
    std::cout << "Converting to forward existential form..." << std::flush ; 
    forwardExistentialArray = Ctlp_FormulaArrayConvertToForward(formulaArray, 1, FALSE);
    std::cout << "Done !" << std::endl;
  }

  array_t * convertedArray = Ctlp_FormulaArrayConvertToDAG(formulaArray);
  array_free(formulaArray);
  array_t * existentialConvertedArray =
    Ctlp_FormulaDAGConvertToExistentialFormDAG(convertedArray);


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
      if (doWitness) {
	its::State explained = checker.explain(checker.getInitialState(),formula,std::cout);
	Statistic SS4 = Statistic(explained, "witness stats ", CSV); // can also use LATEX instead of CSV
	SS4.print_line(std::cout);
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
      if (doWitness) {
	its::State explained = checker.explain(checker.getInitialState(),formula,std::cout);
	Statistic SS4 = Statistic(explained, "witness stats ", CSV); // can also use LATEX instead of CSV
	SS4.print_line(std::cout);
      }
    }

    std::cout << "\n *************************************** \n\n";
  }

  if (showlegend) {
    SS3.print_legend(std::cout);
  }
}
