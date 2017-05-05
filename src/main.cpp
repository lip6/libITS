// Copyright (C) 2004, 2009, 2010, 2011, 2012, 2016 Laboratoire
// d'Informatique de Paris 6 (LIP6), département Systèmes Répartis
// Coopératifs (SRC), Université Pierre et Marie Curie.
//
// This file is part of the Spot tutorial. Spot is a model checking
// library.
//
// Spot is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Spot is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Spot; see the file COPYING.  If not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <string>
#include <cstring>

#include <bddx.h>
#include <spot/tl/parse.hh>

#include "sogtgbautils.hh"
#include "train.hh"
#include "ddd/MemoryManager.h"

// prod parser
#include "its/Options.hh"

// fair CTL bricks
#include "tgbaIts.hh"
#include "fsltl.hh"
#include "fsltltesting.hh"

using namespace its;
using namespace sogits;
using std::cerr;
using std::string;
using std::endl;


void usage() {
  cerr << "Instantiable Transition Systems SDD/DDD LTL Analyzer;" <<endl;
  cerr << "Mandatory options : -i -t to provide input model, -ltl or -LTL to provide formulae" << std::endl;

  usageInputOptions();
  usageSDDOptions();

  cerr << "This tool performs LTL verification on state-space of ITS" <<endl;
  cerr << " LTL specific options for  package " << PACKAGE_STRING << endl;

  std::cerr << "  MANDATORY : specify a formula to check " << std::endl
	    << "  -LTL formula_file  formula read from formula_file, one per line, lines starting with # are ignored." << std::endl
	    << "  -ltl formula       specify the ltl formula as a string. Must be stuttering invariant for SOG and SOP variants." << std::endl
	    << "Actions:" << std::endl
            << "  -aALGO          apply the emptiness check algoritm ALGO"
            << std::endl
            << "  -SSOGTYPE       apply the SOG construction algoritm SOGTYPE={SOG,SLAP,SOP,FSOWCTY,FSOWCTY-TGTA,FSEL,BCZ99,SLAP-FST,SLAP-FSA} (SLAP-FST by default)\n"
	    << "                  The FST variants include a test for switching to fully symbolic emptiness check in terminal states.\n"
	    << "                  The FSA variants include a test for switching to fully symbolic emptiness check in any potentially accepting automaton state."
            << std::endl
            << "  --place-syntax           suppose that atomic properties are just names of variables: \"Idle\" will be interpreted as \"Idle=1\""
            << std::endl
            << "  -C              display the number of states and edges of the SOG"
            << std::endl
            << "  -c              check the formula" << std::endl
            << "  -e              display a sequence (if any) of the net "
            << "satisfying the formula (implies -c)" << std::endl
            << std::endl
            << "  -g              display the sog"
            << std::endl
            << "  -p              display the net"
            << std::endl
            << "  -s              show the formula automaton"
            << std::endl
            << "  -stutter-deadlock              stutter in deadlock states"
            << std::endl
            << "Options of the formula transformation:"
            << std::endl
	    << "  -dR3            disable the SCC reduction" << std::endl
	    << "  -R3f            enable full SCC reduction" << std::endl
            << "  -b              branching postponement"
            << " (false by default)" << std::endl
            << "  -l              fair-loop approximation"
            << " (false by default)" << std::endl
            << "  -x              try to produce a more deterministic automaton"
            << " (false by default)" << std::endl
            << "  -y              do not merge states with same symbolic "
            << "representation (true by default)"
            << std::endl
            << "Where ALGO should be one of:" << std::endl
            << "  Cou99(OPTIONS) (the default)" << std::endl
            << "  CVWY90(OPTIONS)" << std::endl
            << "  GV04(OPTIONS)" << std::endl
            << "  SE05(OPTIONS)" << std::endl
            << "  Tau03(OPTIONS)" << std::endl
            << "  Tau03_opt(OPTIONS)" << std::endl;
  exit(2);
}

int main(int argc, const char *argv[]) {

  // external block for full garbage
  {



  bool check = false;
  bool print_rg = false;
  bool print_pn = false;

  bool ce_expected = false;
  bool fm_exprop_opt = false;
  bool fm_symb_merge_opt = true;
  bool post_branching = false;
  bool fair_loop_approx = false;
  bool print_formula_tgba = false;
  bool stutter_dead = false;

  bool scc_optim = true;
  bool scc_optim_full = false;

  std::vector<std::string> ltlprops ;
  std::string algo_string = "Cou99";

  sog_product_type sogtype = SLAP_FST;

  bool isPlaceSyntax = false;

  // echo options of run
  std::cout << "its-ltl command run as :\n" << std::endl;
  for (int i=0;i < argc; i++) {
    std::cout << argv[i] << "  ";
  }
  std::cout << std::endl;

  // Build the options vector
  std::vector<const char *> args;
  for (int i=1;i < argc; i++) {
    args.push_back(argv[i]);
  }

  std::vector<const char *> argsleft;
  argc = args.size();

  for (int i=0;i < argc; i++) {
    if (!strncmp(args[i], "-a", 2)) {
      algo_string = args[i]+2;
    }
    else if (!strcmp(args[i], "-b")) {
      post_branching = true;
    }
    else if (!strcmp(args[i], "-c")) {
      check = true;
    }
    else if (!strcmp(args[i], "-e")) {
      ce_expected = true;
    }
    else if (!strcmp(args[i], "-s")) {
      print_formula_tgba = true;
    }
    else if (!strncmp(args[i], "-ltl", 4)) {
      if (++i > argc)
	{ cerr << "give argument value for ltl formula please after " << args[i-1]<<endl; usage() ; exit(1);}
      ltlprops.emplace_back(args[i]);
    }
    else if (!strncmp(args[i], "-dR3", 4)) {
      scc_optim = false;
    }
    else if (!strncmp(args[i], "-R3f", 4)) {
      scc_optim = true;
      scc_optim_full = true;
    }
    else if (!strcmp(args[i], "--place-syntax")) {
      isPlaceSyntax = true;
    }
    else if (!strncmp(args[i], "-LTL", 4)) {
      if (++i > argc)
	{ cerr << "give argument value for ltl formula file please after " << args[i-1]<<endl; usage() ; exit(1);}
      std::ifstream fin(args[i]);
      if (!fin) {
          std::cerr << "Cannot open " << args[i]+2 << std::endl;
          exit(2);
      }
      std::string line;
      int nbprop = 0;
      while ( std::getline(fin, line)) {
	if ( line[0] == '#' || line == "" ) {
	  continue;
	} else {
	  ltlprops.emplace_back(line);
	  nbprop++;
	}
      } 
      if (nbprop == 0) {
	std::cerr << "Cannot read file " << args[i]<< std::endl;
	exit(2);
      } else {
	std::cout << "Read " << nbprop << " LTL properties " << std::endl;
      }
    }
    else if (!strcmp(args[i], "-g")) {
      print_rg = true;
    }
    else if (!strcmp(args[i], "-l")) {
      fair_loop_approx = true;
    }
    else if (!strcmp(args[i], "-p")) {
      print_pn = true;
    }
    else if (!strcmp(args[i], "-stutter-deadlock")) {
      stutter_dead = true;
    }
    else if (!strcmp(args[i], "-SSOG")) {
      sogtype = PLAIN_SOG;
    }
    else if (!strcmp(args[i], "-SBCZ99")) {
      sogtype = BCZ99;
    }
    else if (!strcmp(args[i], "-SSLAP")) {
      sogtype = SLAP_NOFS;
    }
    else if (!strcmp(args[i], "-SSLAP-FSA")) {
      sogtype = SLAP_FSA;
    }
    else if (!strcmp(args[i], "-SSLAP-FST")) {
      sogtype = SLAP_FST;
    }
    else if (!strcmp(args[i], "-SSOP")) {
      sogtype = SOP;
    }
    else if (!strcmp(args[i], "-SFSEL")) {
      sogtype = FS_EL;
    }
    else if (!strcmp(args[i], "-SFSOWCTY")) {
      sogtype = FS_OWCTY;
    }
    else if (!strcmp(args[i], "-SFSOWCTY-TGTA"))
      {
        sogtype = FS_OWCTY_TGTA;
      }
    else if (!strcmp(args[i], "-SSOG-TGTA"))
      {
        sogtype = SOG_TGTA;
      }
    else if (!strcmp(args[i], "-SSLAP-TGTA"))
      {
        sogtype = SLAP_TGTA;
      }
    else if (!strcmp(args[i], "-SSOP-TGTA"))
      {
        sogtype = SOP_TGTA;
      }
    else if (!strcmp(args[i], "-SSLAP-DTGTA"))
      {
        sogtype = SLAP_DTGTA;
      }
    else if (!strcmp(args[i], "-SSOP-DTGTA"))
      {
        sogtype = SOP_DTGTA;
      }
    else if (!strcmp(args[i], "-x")) {
      fm_exprop_opt = true;
    }
    else if (!strcmp(args[i], "-y")) {
      fm_symb_merge_opt = false;
    }
    else {
      argsleft.push_back(args[i]);
    }
  }
  args = argsleft;

  ITSModel * model;
   if (sogtype == FS_OWCTY || sogtype == FS_EL)
        {
          model = new fsltlModel();
        }
      else if (sogtype == FS_OWCTY_TGTA || sogtype == SLAP_TGTA || sogtype
          == SLAP_DTGTA)
        {
          model = new fsltlTestingModel();
        }
      else
        {
          model = new ITSModel();
        }

    // parse command line args to get the options
  if (! handleInputOptions (args, *model) ) {
    usage();
    return 1;
  }

  if (sogtype == FS_OWCTY_TGTA || sogtype == SLAP_TGTA || sogtype
          == SLAP_DTGTA)
        {
          if (((fsltlTestingModel*) model)->getTestingModel() == NULL)
            {
              std::cerr << "Please specify input problem type with option -t. Supported type is :  {ETF} \n" ;
              cerr << "The TGTA approaches can only be used with ETF models"
                  << endl;
              exit(2);
              return 1;
            }
        }
  // we now should have the model defined.
  string modelName = model->getInstance()->getType()->getName();

  bool with_garbage = true;
  // Setup SDD specific settings
  if (!handleSDDOptions (args, with_garbage)) {
    usage();
    return 1;
  }

  if (! args.empty()) {
    std::cerr << "Unrecognized command line arguments :" ;
    for (size_t i = 0; i < args.size(); ++i) {
      std::cerr << args[i] << " ";
    }
    std::cerr << std::endl;
    usage();
    return 1;
  }

  if (print_pn)
    std::cout << *model << std::endl;


//   // Parse and build the model !!!
//   loadTrains(2,model);
//   // Update the model to point at this model type as main instance
//   model.setInstance("Trains","main");
//   // The only state defined in the type "trains" is "init"
//   // This sets the initial state of the main instance
//   model.setInstanceState("init");
  int idform =0;
  for (const auto & ltl_string : ltlprops) {
    // Initialize spot
    spot::parsed_formula pf = spot::parse_infix_psl(ltl_string);
    if (pf.format_errors(std::cerr))
      return 1;
    
    if (check) {
      std::cout << "Checking formula " << idform << " : "  << ltl_string << std::endl;
      std::cout << "Formula " << idform << " simplified : "  << pf.f << std::endl;     


      LTLChecker checker;
      checker.setFormula(pf.f);
      checker.setModel(model);
      checker.setOptions(algo_string, ce_expected,
			 fm_exprop_opt, fm_symb_merge_opt,
			 post_branching, fair_loop_approx, "STATS", print_rg,
			 scc_optim, scc_optim_full, print_formula_tgba, stutter_dead);
      if (isPlaceSyntax) {
	checker.setPlaceSyntax(true);
      }
      bool res = checker.model_check(sogtype);
      std::cout << "Formula " << idform << " is ";
      if (res) {
	std::cout << "FALSE " ;
      } else {
	std::cout << "TRUE no " ;
      }
      std::cout << "accepting run found." << std::endl;
    }
    idform++;    
  }
  delete model;
  // external block for full garbage
  }
  MemoryManager::garbage();

  return 0;

}
