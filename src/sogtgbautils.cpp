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


namespace sogits {

std::string* check_at_prop(ITSModel & m, 
                           const spot::ltl::formula* f, 
                           spot::ltl::atomic_prop_set*& sap) {
  sap = spot::ltl::atomic_prop_collect(f);

  if (sap) {
    spot::ltl::atomic_prop_set::iterator it;
    for(it = sap->begin(); it != sap->end(); ++it) {
      // 
      // Load into model m !  + check existence 
      if(false /* ! model.existsAnAP( ((*it)->name() )) ) */  ) {
        std::string* s = new std::string((*it)->name());
        delete sap;
        sap = 0;
        return s;
      } 
    }
  }
  return 0;
} //

void model_check(ITSModel & m,  
                 const spot::ltl::formula* f, const std::string& echeck_algo,
                 bool ce_expected, 
                 bool fm_exprop_opt, 
                 bool fm_symb_merge_opt, 
                 bool post_branching, 
                 bool fair_loop_approx) {
  
  spot::ltl::atomic_prop_set *sap;
  const std::string* s;
  
  if ((s = check_at_prop(m, f, sap))) {
    std::cout << "the atomic proposition '" << *s
        << "' does not correspond to any known proposition" << std::endl;
    delete s;
    return;
  }
  
  spot::bdd_dict dict;

  sog_tgba p(m, sap, &dict);

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

  spot::tgba_product prod(a, &p);

  spot::emptiness_check *ec =  echeck_inst->instantiate(&prod);
  timers.stop("construction");

  timers.start("emptiness check");
  spot::emptiness_check_result* res = ec->check();
  timers.stop("emptiness check");

  ec->print_stats(std::cout);
//  timers.print(std::cout);
  const spot::timer& tec = timers.timer("emptiness check");
  clock_t total = tec.utime() + tec.stime();
  std::cout << total << " ticks for the emptiness check" << std::endl;
  
  if (res) {
    if (ce_expected) {
      std::cout << "an accepting run exists" << std::endl;
      spot::tgba_run* run = res->accepting_run();
      if (run)
        {
          spot::print_tgba_run(std::cout, &prod, run);
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
  delete echeck_inst;
  delete a;
  if (tba)
    delete tba;
  delete sap;

  return;
} //


} // namespace

