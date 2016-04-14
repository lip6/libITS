#include <iostream>

#include <spot/tl/formula.hh>
#include <spot/tl/apcollect.hh>
#include <spot/tl/parse.hh>
#include <spot/twaalgos/ltl2tgba_fm.hh>
#include <spot/twa/twaproduct.hh>
#include <spot/twaalgos/stats.hh>
#include <spot/twaalgos/emptiness.hh>
#include <spot/twaalgos/dot.hh>
#include <spot/twaalgos/degen.hh>
#include <spot/taalgos/tgba2ta.hh>
#include <spot/taalgos/minimize.hh>
#include <spot/twaalgos/sccfilter.hh>
#include "sogtgbautils.hh"
#include "apiterator.hh"
#include "slap.hh"
#include "dsog.hh"
#include "fsltl.hh"
#include "fsltltesting.hh"
#include "sogkripke.cpp"
#include <spot/ta/tgtaproduct.hh>
#include "slaptgta.cpp"
#include "dsogtgta.hh"

#include "statistic.hpp"
#include <spot/twaalgos/postproc.hh>

// #define trace std::cerr
#define trace if (0) std::cerr

namespace sogits
{

  bool
  LTLChecker::model_check(sog_product_type sogtype)
  {
    if (!buildTgbaFromformula(sogtype))
      {
        std::cerr << "Failed to build tgba " << std::endl;
	return false;
      }


    if (sogtype == FS_OWCTY || sogtype == FS_EL || sogtype == FS_OWCTY_TGTA)
      {	
        return fs_model_check(sogtype == FS_OWCTY || sogtype == FS_OWCTY_TGTA);
      }

    const char* err;
    auto echeck_inst =
      spot::make_emptiness_check_instantiator(echeck_algo_.c_str(), &err);
    if (!echeck_inst)
      {
        std::cerr << "Failed to parse argument of -a near `" << err << "'"
		  << std::endl;
        exit(2);
      }
    unsigned int n_acc = a_->num_sets();
    // FIXME: If a_ was a tgta_, this will replace it by a tba.
    if (echeck_inst->max_sets() < n_acc)
      a_ = ag_ = spot::degeneralize_tba(ag_);
    if (a_->num_sets() < echeck_inst->min_sets())
      {
        std::cerr << echeck_algo_ << " requires at least "
		  << echeck_inst->min_sets()
		  << " acceptance sets." << std::endl;
        exit(1);
      }

    spot::twa_ptr prod = nullptr;
    switch (sogtype)
      {
    case PLAIN_SOG:
      prod = otf_product(a_, systgba_);
      break;
    case BCZ99:
      prod = otf_product(a_, systgba_);
      break;
    case SLAP_NOFS:
      prod = std::make_shared<slap::slap_tgba>(a_, *sogModel_, NOFS);
      break;
    case SLAP_FST:
      prod = std::make_shared<slap::slap_tgba>(a_, *sogModel_, FST);
      break;
    case SLAP_FSA:
      prod = std::make_shared<slap::slap_tgba>(a_, *sogModel_, FSA);
      break;
    case SOP:
      prod = std::make_shared<dsog::dsog_tgba>(ag_, *sogModel_);
      break;
    case SOG_TGTA:
      prod = std::make_shared<spot::tgta_product>
	(std::make_shared<sogits::sog_kripke>(systgba_),
	 std::dynamic_pointer_cast<spot::tgta>(a_));
      break;
    case SLAP_TGTA:
    case SLAP_DTGTA:
      prod = std::make_shared<slap::slap_tgta>(a_, *sogModel_, NOFS);
      break;
    case SOP_TGTA:
    case SOP_DTGTA:
      prod = std::make_shared<dsog::dsog_tgta>(ag_, *sogModel_);
      break;
    case FS_OWCTY:
    case FS_EL:
    case FS_OWCTY_TGTA:
      // (case treated for compiler warning) should not happen, tested at top of function
      return false;
      }

    if (display_)
      {
        spot::print_dot(std::cout, prod);
        exit(0);
      }

    auto ec = echeck_inst->instantiate(prod);
    timers.stop("construction");

    timers.start("emptiness check");
    auto res = ec->check();
    timers.stop("emptiness check");

    ec->print_stats(std::cout);
    //  timers.print(std::cout);
    const spot::timer& tec = timers.timer("emptiness check");
    clock_t total = tec.utime() + tec.stime();
    std::cout << total << " ticks for the emptiness check" << std::endl;

    SDD d;
    Statistic S = Statistic(d, ltl_string_, CSV); // can also use LATEX instead of CSV
    S.print_table(std::cout);

    if (res)
      {
        if (ce_expected_)
          {
            std::cout << "an accepting run exists" << std::endl;
            auto run = res->accepting_run();
            if (run)
	      std::cout << *run;
          }
        else
          {
            std::cout
                << "an accepting run exists (use option '-e' to print it)"
                << std::endl;
          }
	return true;
      }
    else
      {
        std::cout << "no accepting run found" << std::endl;
	return false;
      }
  } //

  LTLChecker::~LTLChecker()
  {
    delete sogModel_;
    delete sap_;
  }

  bool
  LTLChecker::fs_model_check(bool isOWCTY)
  {
    its::fsltlModel * fsmodel = (its::fsltlModel *) model_;
    fsmodel->setSogModel(sogModel_);
    fsmodel->declareType(a_);

    fsmodel->buildComposedSystem();

    its::State res;
    if (isOWCTY)
      {
        res = fsmodel->findSCC_owcty();
      }
    else
      {
        res = fsmodel->findSCC_el();
      }
    Statistic S = Statistic(res, ltl_string_, CSV); // can also use LATEX instead of CSV
    S.print_table(std::cout);

    if (res != its::State::null)
      std::cout << "an accepting run exists" << std::endl;
    else
      std::cout << "no accepting run found" << std::endl;
    return  (res != its::State::null);
  }

  bool
  LTLChecker::buildTgbaFromformula(sog_product_type sogtype)
  {
    // find all AP in the formula
    sap_ = spot::atomic_prop_collect(f_);

    sogModel_ = new sogIts(*model_);
    sogModel_->setStutterDeadlock(stutter_dead_);
    sogModel_->setPlaceSyntax(isPlaceSyntax);
    
    systgba_ = std::make_shared<sog_tgba>(*sogModel_, dict_, sogtype);

    if (sap_)
      {
        APIterator::varset_t vars;

        for (auto a: *sap_)
          {
            // declare them in a spot dictionary
	    int varnum = systgba_->register_ap(a);

            vars.push_back(varnum);
            // Load into model m !  + check existence
            // varnum will be used in subsequent interactions with the ITS model
            bool ret = sogModel_->setObservedAP(a.ap_name(), varnum);

            if (!ret)
              {
                delete sap_;
                sap_ = 0;
                std::cout << "the atomic proposition '" << a.ap_name()
                    << "' does not correspond to any known proposition"
                    << std::endl;
                return false;
              }

            its::fsltlTestingModel * testingModel =
                dynamic_cast<its::fsltlTestingModel*> (model_);
            if (testingModel)
              {
                testingModel->getTestingModel()->setObservedAP(a.ap_name(),
							       varnum);
              }

          }
        APIteratorFactory::setAPVarSet(vars);
      }

    timers.start("construction");
    ag_ = spot::ltl_to_tgba_fm(f_, dict_, fm_exprop_opt_, fm_symb_merge_opt_,
			       post_branching_, fair_loop_approx_);


   // TGBA post processing (minimization of TGBA using simulation(bi-simulation), cosimulation, iterated_simulations,...)
    spot::postprocessor post;
    ag_ = post.run(ag_, f_);

    if (scc_optim_)
      ag_ = spot::scc_filter(ag_, scc_optim_full_);

    a_ = ag_;

    spot::tgta_explicit_ptr tgta = nullptr;
    if (sogtype == FS_OWCTY_TGTA || sogtype == SOG_TGTA || sogtype == SLAP_TGTA
        || sogtype == SOP_TGTA || sogtype
        == SLAP_DTGTA || sogtype == SOP_DTGTA)
      {
        bdd atomic_props_set_bdd = bddtrue;
        for (auto i: *sap_)
          {
            bdd atomic_prop = bdd_ithvar((ag_->get_dict())->var_map[i]);
            atomic_props_set_bdd &= atomic_prop;
          }

        auto tgta = spot::tgba_to_tgta(ag_, atomic_props_set_bdd);
        // TGTA post processing  (only minimization using bi-simulation was implemented for TGTA)
	a_ = spot::minimize_tgta(tgta);
      }

    if (print_formula_tgba_)
      spot::print_dot(std::cerr, a_);

    return true;
  }

} // namespace
