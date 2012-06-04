#include "tgbaIts.hh"
#include "tgbaalgos/reachiter.hh"
#include "tgba/bddprint.hh"
#include "ltlvisit/tostring.hh"
#include "ltlast/atomic_prop.hh"
#include "misc/escape.hh"
#include "Hom_Basic.hh"

namespace its {

  const std::string TgbaType::name_ = "TGBA";


  namespace loadTGBA
  {
    
    class load_bfs: public spot::tgba_reachable_iterator_breadth_first
    {
      TgbaType::arcs_t & arcs_;
      int nbStates_;
    public:
      load_bfs(const spot::tgba* a, TgbaType::arcs_t & arcs)
	: tgba_reachable_iterator_breadth_first(a),arcs_(arcs),nbStates_(0)
      {
      }

      /// Called by run() to process a state.
      ///
      /// \param s The current state.
      /// \param n A unique number assigned to \a s.
      /// \param si The spot::tgba_succ_iterator for \a s.
      void process_state(const spot::state* s, int n,spot:: tgba_succ_iterator* si)
      {
	++nbStates_;
      }

      /// Called by run() to process a transition.
      ///
      /// \param in_s The source state
      /// \param in The source state number.
      /// \param out_s The destination state
      /// \param out The destination state number.
      /// \param si The spot::tgba_succ_iterator positionned on the current
      ///             transition.
      ///
      /// The in_s and out_s states are owned by the
      /// spot::tgba_reachable_iterator instance and destroyed when the
      /// instance is destroyed.
      void process_link(const spot::state* in_s, int in,
			const spot::state* out_s, int out,
			const spot::tgba_succ_iterator* si) 
      {
	TgbaType::tgba_arc_label_t lab (si->current_condition(), si->current_acceptance_conditions());
	TgbaType::tgba_arc_t arc (in,out);
	TgbaType::arcs_it it = arcs_.find(lab);
	if ( it != arcs_.end() ) {
	  // already some arcs matched to this key
	  it->second.push_back(arc);
	} else {
	  arcs_.insert(TgbaType::arcs_t::value_type(lab, TgbaType::tgba_arcs_t(1,arc)));
	}
      }

    };
  }

  TgbaType::TgbaType (const spot::tgba * tgba) : tgba_(tgba) 
  {
    loadTGBA::load_bfs b(tgba, arcs_);    
    b.run();
    build_labels();
  }

  labels_t TgbaType::getAcceptanceSet (bdd acc, const spot::tgba * tgba)  {
    labels_t ret;
    
    const spot::bdd_dict* d = tgba->get_dict();
    while (acc != bddfalse)
      {
	bdd cube = bdd_satone(acc);
	acc -= cube;
	const spot::ltl::formula* f = d->oneacc_to_formula(cube);
	std::string s = spot::ltl::to_string(f);
	if (is_atomic_prop(f) && s[0] == '"')
	  {
	    // Unquote atomic propositions.
	    s.erase(s.begin());
	    s.resize(s.size() - 1);
	  }
	ret.push_back(spot::escape_str(s));
      }

      
    std::sort(ret.begin(), ret.end());
    return ret;
  }
  /** compute a vector of strings representing a bdd of an acceptance set */
  labels_t TgbaType::getAcceptanceSet (bdd acc) const {
    return getAcceptanceSet(acc, tgba_);
  }

  /** A helper function to print the acceptance conditions bdd 
   * Code copy paste direct from spot::save.cc file */
  void TgbaType::print_acc(bdd acc, std::ostream & os) const {
    labels_t acc_set = getAcceptanceSet(acc);
    
    os << "<";
    labels_it it = acc_set.begin();

    if (it != acc_set.end()) {
      os << "\"" << *it << "\"";
    
      for (++it ; it != acc_set.end();  ++it ) {
	os << "," << "\"" << *it << "\"";
      }
    }
    os << ">";

  }

  /** A helper function to print the atomic prop condition formula bdd */
  void TgbaType::print_cond(bdd cond, std::ostream & os) const {
    /** code stolen from spot::save.cc */
    const spot::bdd_dict* d = tgba_->get_dict();
    spot::escape_str(os, bdd_format_formula(d, cond));
  }

  /** A pretty print for tgba arc labels, wrapper that relies on print_acc and print_cond */
  vLabel TgbaType::get_arc_label (const tgba_arc_label_t & lab) const {
    std::stringstream os;
    os << "<" ;
    print_cond(lab.first, os);
    os << " , " ;
    print_acc(lab.second, os);
    os << ">";
    return os.str();
  }

  void TgbaType::build_labels () {
    for (arcs_it it = arcs_.begin() ; it != arcs_.end() ; ++it) {      
      labmap_.insert(labmap_t::value_type(get_arc_label(it->first), it->first));
    }
  }
  

  /** Build the transition for a structural arc */
  GHom TgbaType::buildTransition (const tgba_arc_t & arc) const {
    /** an arc_t is simply a pair of integer */
    
    // test for self loop : optimizes away the write operation
    if (arc.first == arc.second) {
      // 0 is the DEFAULT_VAR
      return varEqState(0, arc.first);
    } else {
      return setVarConst(0, arc.second) & varEqState(0, arc.first);
    }
  }

  GHom  TgbaType::buildTransitionSet (const tgba_arcs_t & arcs) const {
    std::set<GHom> toadd;
    for (tgba_arcs_it it = arcs.begin() ; it != arcs.end() ; ++it ) {
      toadd.insert(buildTransition(*it));
    }
    return GHom::add(toadd);
  }


  /** Build the its::Transition for a given string label */
  Transition TgbaType::buildTransitionFromLabel (Label label) const {
    // grab the appropriate arc set
    tgba_arc_label_t tgbalab = getTransLabelDescription(label);
    arcs_t::const_iterator it = arcs_.find(tgbalab);
    return localApply(buildTransitionSet(it->second), 0);
  }

}
