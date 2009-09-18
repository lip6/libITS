#include <iostream>

#include "misc/timer.hh"
#include "ltlast/atomic_prop.hh"
#include "ltlvisit/apcollect.hh"
#include "ltlenv/environment.hh"
#include "ltlparse/public.hh"
#include "tgbaalgos/ltl2tgba_fm.hh"
#include "tgba/tgbaproduct.hh"
#include "tgbaalgos/gtec/gtec.hh"
#include "tgbaalgos/magic.hh"
#include "tgbaalgos/stats.hh"
#include "tgbaalgos/emptiness.hh"
#include "tgba/tgbatba.hh"

#include "sogtgba.hh"
#include "sogtgbautils.hh"
#include "apiterator.hh"
#include "slog.hh"
#include "dsog.hh"

#include "statistic.hpp"


namespace sogits {

  void model_check(its::ITSModel & model_,
		   const spot::ltl::formula* f,
		   sog_product_type sogtype,
		   const std::string& echeck_algo,
		   bool ce_expected,
		   bool fm_exprop_opt,
		   bool fm_symb_merge_opt,
		   bool post_branching,
		   bool fair_loop_approx, const std::string & ltl_string) {

  // find all AP in the formula
  spot::ltl::atomic_prop_set *sap = spot::ltl::atomic_prop_collect(f);

  // For bdd varnum to AP name in Spot
  spot::bdd_dict dict;

  sogIts model = model_;
  sog_tgba systgba(model, &dict);

  if (sap) {
    APIterator::varset_t vars ;

    for(spot::ltl::atomic_prop_set::iterator  it = sap->begin(); it != sap->end(); ++it) {
       // declare them in a spot dictionary
      int varnum = dict.register_proposition(*it, &systgba);

      vars.push_back(varnum);
      // Load into model m !  + check existence
      // varnum will be used in subsequent interactions with the ITS model
      bool ret =  model.setObservedAP ( (*it)->name() , varnum );

      if ( ! ret  ) {
        delete sap;
        sap = 0;
	std::cout << "the atomic proposition '" <<  (*it)->name()
        << "' does not correspond to any known proposition" << std::endl;
	return;
      }
    }
    APIteratorFactory::setAPVarSet(vars);
  }


  spot::timer_map timers;
  timers.start("construction");
  spot::tgba* a = spot::ltl_to_tgba_fm(f, &dict, fm_exprop_opt,
                         fm_symb_merge_opt, post_branching, fair_loop_approx);

  const char* err;
  spot::emptiness_check_instantiator* echeck_inst =
      spot::emptiness_check_instantiator::construct(echeck_algo.c_str(), &err);
  if (!echeck_inst) {
    std::cerr << "Failed to parse argument of -a near `"
        << err <<  "'" << std::endl;
    exit(2);
  }
  spot::tgba* tba = NULL;
  unsigned int n_acc = a->number_of_acceptance_conditions();
  if (echeck_inst->max_acceptance_conditions() < n_acc) {
    tba = a;
    a = new spot::tgba_tba_proxy(a);
  }
  if (a->number_of_acceptance_conditions()
      < echeck_inst->min_acceptance_conditions()) {
      std::cerr << echeck_algo << " requires at least "
          << echeck_inst->min_acceptance_conditions()
          << " acceptance conditions." << std::endl;
      exit(1);
  }


  spot::tgba * prod = NULL;
  switch (sogtype) {
  case PLAIN_SOG :
    prod = new spot::tgba_product(a, &systgba);
    break;
  case SLOG :
    prod = new slog::slog_tgba(a, model);
    break;
  case DSOG :
    prod = new dsog::dsog_tgba(a, model);
    break;
  }

  spot::emptiness_check *ec =  echeck_inst->instantiate(prod);
  timers.stop("construction");

  timers.start("emptiness check");
  spot::emptiness_check_result* res = ec->check();
  timers.stop("emptiness check");

  ec->print_stats(std::cout);
//  timers.print(std::cout);
  const spot::timer& tec = timers.timer("emptiness check");
  clock_t total = tec.utime() + tec.stime();
  std::cout << total << " ticks for the emptiness check" << std::endl;

  SDD d;
  Statistic S = Statistic(d, ltl_string , CSV); // can also use LATEX instead of CSV
  S.print_table(std::cout);

  if (res) {
    if (ce_expected) {
      std::cout << "an accepting run exists" << std::endl;
      spot::tgba_run* run = res->accepting_run();
      if (run)
        {
          spot::print_tgba_run(std::cout, prod, run);
          delete run;
        }
    }
    else {
      std::cout << "an accepting run exists (use option '-e' to print it)"
                << std::endl;
    }
    delete res;
  }
  else {
    std::cout << "no accepting run found" << std::endl;
  }

  delete ec;
  delete prod;
  delete echeck_inst;
  delete a;
  if (tba)
    delete tba;
  delete sap;

  return;
} //


} // namespace
