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
#include "taalgos/tgba2ta.hh"
#include "taalgos/minimize.hh"
#include "tgbaalgos/sccfilter.hh"
#include "sogtgbautils.hh"
#include "apiterator.hh"
#include "slap.hh"
#include "dsog.hh"
#include "fsltl.hh"
#include "fsltltesting.hh"
#include "sogkripke.cpp"
#include "ta/tgtaproduct.hh"
#include "slaptgta.cpp"
#include "dsogtgta.hh"

#include "statistic.hpp"
#include "tgbaalgos/postproc.hh"

// #define trace std::cerr
#define trace if (0) std::cerr

namespace sogits
{

  void
  LTLChecker::model_check(sog_product_type sogtype)
  {
    if (!buildTgbaFromformula(sogtype))
      return;

    if (sogtype == FS_OWCTY || sogtype == FS_EL || sogtype == FS_OWCTY_TGTA)
      {
        fs_model_check(sogtype == FS_OWCTY || sogtype == FS_OWCTY_TGTA);
        return;
      }

    const char* err;
    spot::emptiness_check_instantiator* echeck_inst =
        spot::emptiness_check_instantiator::construct(echeck_algo_.c_str(),
            &err);
    if (!echeck_inst)
      {
        std::cerr << "Failed to parse argument of -a near `" << err << "'"
            << std::endl;
        exit(2);
      }
    tba_ = NULL;
    unsigned int n_acc = a_->number_of_acceptance_conditions();
    if (echeck_inst->max_acceptance_conditions() < n_acc)
      {
        tba_ = a_;
        a_ = new spot::tgba_tba_proxy(a_);
      }
    if (a_->number_of_acceptance_conditions()
        < echeck_inst->min_acceptance_conditions())
      {
        std::cerr << echeck_algo_ << " requires at least "
            << echeck_inst->min_acceptance_conditions()
            << " acceptance conditions." << std::endl;
        exit(1);
      }

    spot::tgba * prod = NULL;
    switch (sogtype)
      {
    case PLAIN_SOG:
      prod = new spot::tgba_product(a_, systgba_);
      break;
    case BCZ99:
      prod = new spot::tgba_product(a_, systgba_);
      break;
    case SLAP_NOFS:
      prod = new slap::slap_tgba(a_, *sogModel_, NOFS);
      break;
    case SLAP_FST:
      prod = new slap::slap_tgba(a_, *sogModel_, FST);
      break;
    case SLAP_FSA:
      prod = new slap::slap_tgba(a_, *sogModel_, FSA);
      break;
    case SOP:
      prod = new dsog::dsog_tgba(a_, *sogModel_);
      break;
    case SOG_TGTA:
      prod = new spot::tgta_product(new sogits::sog_kripke(systgba_),
          (spot::tgta*) a_);
      break;
    case SLAP_TGTA:
    case SLAP_DTGTA:
      prod = new slap::slap_tgta(a_, *sogModel_, NOFS);
      break;
    case SOP_TGTA:
    case SOP_DTGTA:
      prod = new dsog::dsog_tgta(a_, *sogModel_);
      break;
    case FS_OWCTY:
    case FS_EL:
    case FS_OWCTY_TGTA:
      // (case treated for compiler warning) should not happen, tested at top of function
      return;
      }

    if (display_)
      {
        spot::dotty_reachable(std::cout, prod);
        exit(0);
      }

    spot::emptiness_check *ec = echeck_inst->instantiate(prod);
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
    Statistic S = Statistic(d, ltl_string_, CSV); // can also use LATEX instead of CSV
    S.print_table(std::cout);

    if (res)
      {
        if (ce_expected_)
          {
            std::cout << "an accepting run exists" << std::endl;
            spot::tgba_run* run = res->accepting_run();
            if (run)
              {
                spot::print_tgba_run(std::cout, prod, run);
                delete run;
              }
          }
        else
          {
            std::cout
                << "an accepting run exists (use option '-e' to print it)"
                << std::endl;
          }
        delete res;
      }
    else
      {
        std::cout << "no accepting run found" << std::endl;
      }

    delete ec;
    delete prod;
    delete echeck_inst;

    return;
  } //

  LTLChecker::~LTLChecker()
  {
    delete sogModel_;
    delete systgba_;
    delete tgba_transformed_to_tgta_;
    delete a_;
    if (tba_)
      delete tba_;
    delete sap_;
  }

  void
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
    return;
  }

  bool
  LTLChecker::buildTgbaFromformula(sog_product_type sogtype)
  {
    // find all AP in the formula
    sap_ = spot::ltl::atomic_prop_collect(f_);

    sogModel_ = new sogIts(*model_);
    if (isPlaceSyntax)
      {
        sogModel_->setPlaceSyntax(isPlaceSyntax);
      }

    systgba_ = new sog_tgba(*sogModel_, &dict_, sogtype);

    if (sap_)
      {
        APIterator::varset_t vars;

        for (spot::ltl::atomic_prop_set::iterator it = sap_->begin(); it
            != sap_->end(); ++it)
          {
            // declare them in a spot dictionary
            int varnum = dict_.register_proposition(*it, systgba_);

            vars.push_back(varnum);
            // Load into model m !  + check existence
            // varnum will be used in subsequent interactions with the ITS model
            bool ret = sogModel_->setObservedAP((*it)->name(), varnum);

            if (!ret)
              {
                delete sap_;
                sap_ = 0;
                std::cout << "the atomic proposition '" << (*it)->name()
                    << "' does not correspond to any known proposition"
                    << std::endl;
                return false;
              }

            its::fsltlTestingModel * testingModel =
                dynamic_cast<its::fsltlTestingModel*> (model_);
            if (testingModel)
              {
                testingModel->getTestingModel()->setObservedAP((*it)->name(),
                    varnum);
              }

          }
        APIteratorFactory::setAPVarSet(vars);
      }

    timers.start("construction");
    a_ = spot::ltl_to_tgba_fm(f_, &dict_, fm_exprop_opt_, fm_symb_merge_opt_,
        post_branching_, fair_loop_approx_);


   // TGBA post processing (minimization of TGBA using simulation(bi-simulation), cosimulation, iterated_simulations,...)
    spot::postprocessor post;
    a_ = post.run(a_, f_);

    if (scc_optim_)
      { 
        const spot::tgba* n = spot::scc_filter(a_, scc_optim_full_);
        delete a_;
        a_ = n;
      }

    tgba_transformed_to_tgta_ = 0;
    if (sogtype == FS_OWCTY_TGTA || sogtype == SOG_TGTA || sogtype == SLAP_TGTA
        || sogtype == SOP_TGTA || sogtype
        == SLAP_DTGTA || sogtype == SOP_DTGTA)
      { 
        bdd atomic_props_set_bdd = bddtrue;
        for (spot::ltl::atomic_prop_set::const_iterator i = sap_->begin(); i
            != sap_->end(); ++i)
          { 
            bdd atomic_prop = bdd_ithvar((a_->get_dict())->var_map[*i]);
            atomic_props_set_bdd &= atomic_prop;
          }

        spot::tgta_explicit* tgta =
            spot::tgba_to_tgta(a_, atomic_props_set_bdd);



        // TGTA post processing  (only minimization using bi-simulation was implemented for TGTA)
          {
           tgba_transformed_to_tgta_ = a_;
           a_ = minimize_tgta(tgta);
           delete tgta;

          }


      }

    if (print_formula_tgba_)
      spot::dotty_reachable(std::cerr, a_);

    return true;
  }

} // namespace
