#ifndef SOG_TGBA_UTILS_HH
#define SOG_TGBA_UTILS_HH

#include <set>
#include <list>
#include <string>

#include "sogtgba.hh"
#include <spot/misc/timer.hh>
#include <spot/tl/formula.hh>
#include <spot/tl/apcollect.hh>
#include <spot/twa/bdddict.hh>

#include "its/ITSModel.hh"




namespace sogits {





  /// An enum to capture the strategies related to activation of Fully-Symbolic emptiness check.
  enum FSTYPE {
    NOFS, // < Do not use FS emptiness check at all
    FST, // < Use FS emptiness check, but only for terminal states (no successor except self, a priori => all acceptance conditions represented)
    FSA // < Use FS emptiness check for all states potentially accepting (self loop acceptance conds = full acceptance cond set)
  };



  class LTLChecker {

    spot::atomic_prop_set *sap_;
    spot::formula f_;

    // For bdd varnum to AP name in Spot
    spot::bdd_dict_ptr dict_;

    its::ITSModel * model_;
    sogIts * sogModel_;
    spot::twa_graph_ptr ag_;
    spot::twa_ptr a_;
    std::shared_ptr<sog_tgba> systgba_;
    // options
    std::string echeck_algo_;
    bool ce_expected_;
    bool fm_exprop_opt_;
    bool fm_symb_merge_opt_;
    bool post_branching_;
    bool fair_loop_approx_;
    std::string ltl_string_;
    bool display_;
    bool scc_optim_;
    bool scc_optim_full_;
    bool print_formula_tgba_;
    bool isPlaceSyntax;
    bool stutter_dead_;

    spot::timer_map timers;

    bool buildTgbaFromformula (sog_product_type sogtype);

    bool fs_model_check(bool isOWCTY);
  public :
    LTLChecker () : sap_(nullptr),
		    f_(nullptr),
		    dict_(spot::make_bdd_dict()),
		    model_(nullptr),
		    sogModel_(nullptr),
		    a_(nullptr),
		    systgba_(nullptr),
		    fm_exprop_opt_(false),
		    fm_symb_merge_opt_(true),
		    post_branching_(false),
		    fair_loop_approx_(false),
		    ltl_string_("formula"),
		    display_(false),
		    scc_optim_(true),
		    scc_optim_full_(false),
                    print_formula_tgba_(false),
		    isPlaceSyntax(false),
		    stutter_dead_(false){}

    ~LTLChecker();

    void setFormula (spot::formula f) {
      f_ = f;
    }

    void setAutomaton (spot::twa_graph_ptr  aut, spot::bdd_dict_ptr  dict);
    bool initializeAP (sog_product_type sogtype);

    void setModel (its::ITSModel * model) {
      model_ = model;
    }

    // True if for ascending compatibility issues, atomic properties should be reinterpreted
    // as if they were just the names of places, instead of comparisons.
    // e.g. reinterpret AP  "Idle0" as "Idle0 = 1"
    void setPlaceSyntax (bool val) { isPlaceSyntax = val; }

    void setOptions (const std::string& echeck_algo,
		     bool ce_expected,
		     bool fm_exprop_opt=false,
		     bool fm_symb_merge_opt=true,
		     bool post_branching=false,
		     bool fair_loop_approx=false,
		     const std::string & ltl_string ="formula",
		     bool dotdump = false,
		     bool scc_optim = true,
		     bool scc_optim_full = false,
		     bool print_formula_tgba = false,
		     bool stutter_dead = false)
    {
      echeck_algo_ = echeck_algo;
      ce_expected_ = ce_expected;
      fm_exprop_opt_ = fm_exprop_opt;
      fm_symb_merge_opt_ = fm_symb_merge_opt;
      post_branching_ = post_branching;
      fair_loop_approx_ = fair_loop_approx;
      ltl_string_ = ltl_string;
      display_ = dotdump;
      scc_optim_ = scc_optim;
      scc_optim_full_ = scc_optim_full;
      print_formula_tgba_ = print_formula_tgba;
      stutter_dead_  = stutter_dead;
    }

    /// \brief Check if the Petri net \a n can produce at least one infinite
    /// sequence accepted by the formula \a f.
    ///
    /// If this is the case and \a ce_expected is true, such a sequence is displayed.
    /// In other case, a message indicating if the test is satisfied or not is
    /// printed. The four last parameters specify options for the translation
    /// of the formula \a f in a TGBA as indicated for the function
    /// spot::ltl_to_tgba_fm.
    /// \dontinclude modelcheck.cpp
    /// \skipline void model_check
    /// \until } //
    bool model_check(sog_product_type sogtype);

  };

} // namespace

#endif
