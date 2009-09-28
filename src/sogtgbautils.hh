#ifndef SOG_TGBA_UTILS_HH
#define SOG_TGBA_UTILS_HH

#include <set>
#include <list>
#include <string>

#include "ltlast/formula.hh"
#include "ltlast/atomic_prop.hh"
#include "ltlvisit/apcollect.hh"

#include "ITSModel.hh"

namespace sogits {

/// \brief Display some statistics concerning the symbolic observation
/// graph constructed for \a n when considering the observable
/// transitions \a obs_tr.
///
/// \dontinclude countmarkings.cpp
/// \skipline void count_markings
/// \until } //
  void count_markings(const its::ITSModel & m,  const spot::ltl::formula* f);

/// \brief Display the symbolic observation graph constructed for \a n
/// when considering the observable transitions \a obs_tr.
///
/// \dontinclude printrg.cpp
/// \skipline void print_reachability_graph
/// \until } //
  void print_reachability_graph(const its::ITSModel & m, const spot::ltl::formula* f);

/// \brief Check if the atomic propositions in \a f are transitions of the Petri
/// net \a p. Return a pointer on the first atomic proposition which is not
/// a transition name if any and 0 otherwise.
///
/// \dontinclude modelcheck.cpp
/// \skipline const std::string* check_at_prop
/// \until } //
  std::string* check_at_prop(its::ITSModel & m,
                           const spot::ltl::formula* f,
                           spot::ltl::atomic_prop_set*& sap);



  enum sog_product_type {
    PLAIN_SOG, // < The basic algorithm with static parameters.
    SLOG, // < The (Symbolic) Local Obs Graph algorithm
    DSOG // < The Dynamic Symbolic Obs Graph algorithm
  } ;

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
  void model_check(its::ITSModel & m,
		   const spot::ltl::formula* f,
		   sog_product_type sogtype,
		   const std::string& echeck_algo,
		   bool ce_expected,
		   bool fm_exprop_opt=false,
		   bool fm_symb_merge_opt=true,
		   bool post_branching=false,
		   bool fair_loop_approx=false,
		   const std::string & ltl_string ="formula",
		   bool dotdump = false);


} // namespace

#endif
