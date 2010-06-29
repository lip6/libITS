#include <iostream>


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
#include "tgbaalgos/dotty.hh"
#include "tgba/tgbatba.hh"

#include "tgbaalgos/sccfilter.hh"


#include "sogtgba.hh"
#include "sogtgbautils.hh"
#include "apiterator.hh"
#include "slap.hh"
#include "dsog.hh"
#include "fsltl.hh"

#include "statistic.hpp"


// #define trace std::cerr
#define trace if (0) std::cerr

namespace sogits {

  void LTLChecker::model_check(sog_product_type sogtype) {
    if (! buildTgbaFromformula())
      return;

    if (sogtype == FSLTL) {
      fs_model_check();
      return;
    }

    const char* err;
    spot::emptiness_check_instantiator* echeck_inst =
      spot::emptiness_check_instantiator::construct(echeck_algo_.c_str(), &err);
    if (!echeck_inst) {
      std::cerr << "Failed to parse argument of -a near `"
		<< err <<  "'" << std::endl;
      exit(2);
    }
    tba_ = NULL;
    unsigned int n_acc = a_->number_of_acceptance_conditions();
    if (echeck_inst->max_acceptance_conditions() < n_acc) {
      tba_ = a_;
      a_ = new spot::tgba_tba_proxy(a_);
    }
    if (a_->number_of_acceptance_conditions()
	< echeck_inst->min_acceptance_conditions()) {
      std::cerr << echeck_algo_ << " requires at least "
		<< echeck_inst->min_acceptance_conditions()
		<< " acceptance conditions." << std::endl;
      exit(1);
    }


    spot::tgba * prod = NULL;
    switch (sogtype) {
    case PLAIN_SOG :
      prod = new spot::tgba_product(a_, systgba_);
      break;
    case SLAP_NOFS :
      prod = new slap::slap_tgba(a_, *sogModel_,NOFS);
      break;
    case SLAP_FST :
      prod = new slap::slap_tgba(a_, *sogModel_,FST);
      break;
    case SLAP_FSA :
      prod = new slap::slap_tgba(a_, *sogModel_,FSA);
      break;
    case DSOG :
      prod = new dsog::dsog_tgba(a_, *sogModel_);
      break;
    case FSLTL :
      // (case treated for compiler warning) should not happen, tested at top of function
      return;
    }
    

    if (display_)
      {
	spot::dotty_reachable(std::cout, prod);
	exit(0);
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
    Statistic S = Statistic(d, ltl_string_ , CSV); // can also use LATEX instead of CSV
    S.print_table(std::cout);

    if (res) {
      if (ce_expected_) {
	std::cout << "an accepting run exists" << std::endl;
	spot::tgba_run* run = res->accepting_run();
	if (run)
	  {
	    spot::print_tgba_run(std::cout, prod, run);
	    delete run;
	  }
      } else {
	std::cout << "an accepting run exists (use option '-e' to print it)"
		  << std::endl;
      }
      delete res;
    } else {
      std::cout << "no accepting run found" << std::endl;
    }

    delete ec;
    delete prod;
    delete echeck_inst;

    return;
  } //

  LTLChecker::~LTLChecker () {
    delete sogModel_;
    delete systgba_;
    delete a_;
    if (tba_)
      delete tba_;
    delete sap_;
  }

  void LTLChecker::fs_model_check() {
    its::fsltlModel * fsmodel = (its::fsltlModel *) model_;
    fsmodel->setSogModel(sogModel_);
    fsmodel->declareType (a_);
    fsmodel->buildComposedSystem();
    its::State res = fsmodel->findSCC_owcty();
    Statistic S = Statistic(res, ltl_string_ , CSV); // can also use LATEX instead of CSV
    S.print_table(std::cout);

    if (res != its::State::null)
      std::cout << "an accepting run exists" << std::endl;
    else
      std::cout << "no accepting run found" << std::endl;
    return;
  }

  bool LTLChecker::buildTgbaFromformula () {
    // find all AP in the formula
    sap_ = spot::ltl::atomic_prop_collect(f_);


    sogModel_ = new sogIts (*model_);
    systgba_ = new sog_tgba(*sogModel_, &dict_);

    if (sap_) {
      APIterator::varset_t vars ;

      for(spot::ltl::atomic_prop_set::iterator  it = sap_->begin(); it != sap_->end(); ++it) {
	// declare them in a spot dictionary
	int varnum = dict_.register_proposition(*it, systgba_);

	vars.push_back(varnum);
	// Load into model m !  + check existence
	// varnum will be used in subsequent interactions with the ITS model
	bool ret =  sogModel_->setObservedAP ( (*it)->name() , varnum );

	if ( ! ret  ) {
	  delete sap_;
	  sap_ = 0;
	  std::cout << "the atomic proposition '" <<  (*it)->name()
		    << "' does not correspond to any known proposition" << std::endl;
	  return false;
	}
      }
      APIteratorFactory::setAPVarSet(vars);
    }


    timers.start("construction");
    a_ = spot::ltl_to_tgba_fm(f_, &dict_, fm_exprop_opt_, fm_symb_merge_opt_, post_branching_, fair_loop_approx_);

    if (scc_optim_)
      {
	const spot::tgba* n = spot::scc_filter(a_, scc_optim_full_);
	delete a_;
	a_ = n;
      }

    if (print_formula_tgba_)
      spot::dotty_reachable(std::cerr, a_);


    return true;
  }

} // namespace
