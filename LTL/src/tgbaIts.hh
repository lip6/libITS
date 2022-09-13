#ifndef __TGBA_ITS_HH__
#define __TGBA_ITS_HH__

#include "its/TypeBasics.hh"
#include <spot/twa/twa.hh>
#include <spot/misc/bddlt.hh>
#include <spot/twaalgos/dot.hh>


#include <iosfwd>
#include <map>


namespace its {

/** Implement a type by adapting the TGBA interface.
 *  Using delegation to build a type from a TGBA */
  class TgbaType : public TypeBasics {
    // the concrete storage class
    spot::const_twa_ptr tgba_;
    // used to label SDD node of PNet.
    static const int DEFAULT_VAR = 0;
    // used to name this type
    static const std::string name_ ;
  protected :
    virtual labels_t getVarSet () const {
      return labels_t (1,"state");
    }
  public :
    TgbaType (const spot::const_twa_ptr& tgba_) ;

    /** Allow to visit the underlying type definition */
    void visit (class TypeVisitor * visitor) const {
    }

    /** This function is not relevant for TGBA implementation (afaics currently) */
    its::Transition getAPredicate(const std::string&) const
    { return its::Transition::id; }

    /** Return the set of local transitions, with their name, useful for displaying.*
     * Used in witness trace/counter example construction scenarios.
     **/
    void getNamedLocals (namedTrs_t & ntrans) const { return ; }


    /** the set InitStates of designated initial states */
    labels_t getInitStates () const {
      return labels_t(1,"init");
    }

    /** the type that designates a TGBA arc label : <apcond , acc> as a pair of bdd */
    typedef std::pair<bdd, spot::acc_cond::mark_t> tgba_arc_label_t;
    /** the type that designates a physical arc description : <source, target> as a pair of integer */
    typedef std::pair<int,int> tgba_arc_t;


    /** a data structure to store a set of physical arc descriptions */
    typedef std::vector<tgba_arc_t> tgba_arcs_t;
    typedef tgba_arcs_t::const_iterator tgba_arcs_it;


    struct less_than {
      bool operator()(const its::TgbaType::tgba_arc_label_t &g1, const its::TgbaType::tgba_arc_label_t &g2) const{
	return g1.first.id() < g2.first.id()
	  || ( (! (g1.first.id() > g2.first.id() )) && g1.second < g2.second);
      }
    };


    /** a data structure to store mappings of TGBA arc label to physical arcs that bear this label */
    typedef std::map<tgba_arc_label_t,tgba_arcs_t,less_than> arcs_t;
    typedef arcs_t::iterator arcs_it;

  private :
    /** the data structure instance used to store the appropriate tgba representation */
    arcs_t arcs_;

    /** A helper function to print the acceptance conditions bdd */
    void print_acc(spot::acc_cond::mark_t acc, std::ostream & os) const;
    /** A helper function to print the atomic prop condition formula bdd */
    void print_cond(bdd cond, std::ostream & os) const;
    /** A pretty print for tgba arc labels, wrapper that relies on print_acc and print_cond */
    vLabel get_arc_label (const tgba_arc_label_t & lab) const;


    /** A map to store string label to tgba_arc_label correspondance */
    typedef std::map<vLabel, tgba_arc_label_t> labmap_t;
    typedef labmap_t::iterator labmap_it;
    labmap_t labmap_;

    /** build the string labels and fill labmap */
    void build_labels();

    /** Build the transition for a structural arc */
    GHom buildTransition (const tgba_arc_t & arc) const;
    /** Build the Hom for a set of structural arcs */
    GHom buildTransitionSet (const tgba_arcs_t & arcs) const;
    /** Build the its::Transition for a given string label */
    Transition buildTransitionFromLabel (Label label) const;
  public :

    /** the set T of public transition labels : one for each distinct AP formula x acceptance set labeling the tgba arcs*/
    labels_t getTransLabels () const {
      labels_t ret;
      for (labmap_t::const_iterator it = labmap_.begin() ; it != labmap_.end() ; ++it) {
	ret.push_back(it->first);
      }
      return ret;
    }

    /** compute a vector of strings representing an acceptance mark */
    static labels_t getAcceptanceSet (spot::acc_cond::mark_t acc);

    tgba_arc_label_t getTransLabelDescription (Label trans) const {
      // example : trans = "a . !b x {}" => bdd of : a.!b,  empty set of acc
      // example : trans = "a  x {black,white}" => bdd of : a,  set of acc {black,white}

      labmap_t::const_iterator it = labmap_.find(trans);
      if (it == labmap_.end()) {
	std::cerr << "asked for unknown label : "<< trans << " in a TgbaIts Type" << std::endl;
	std::cerr << "fatal error, aborting sorry."<< std::endl;
	assert(false);
      }
      return it->second;
    }


    /** state and transitions representation functions */
    /** Local transitions : none, all transitions are synchronizable */
    Transition getLocals () const {
      return Shom::null;
    }

    /** Successors synchronization function : Bag(T) -> SHom.
     * The collection represented by the iterator should be a multiset
     * of transition labels of this type (as obtained through getTransLabels()).
     * Otherwise, an assertion violation will be raised !!
     * */
    Transition getSuccs (const labels_t & tau) const {
      Transition toret = GShom::id;
      for (labels_it it = tau.begin() ; it != tau.end() ; ++it ) {
	toret = buildTransitionFromLabel(*it)  & toret;
      }
      return toret;
    }

    /** To obtain a representation of a labeled state */
    State getState(Label stateLabel) const {
      // only one initial state
      assert (stateLabel == "init");
      // the initial state has id 1 by definition
      return State(DEFAULT_VAR, DDD(0, 1 ));
    }

    State getPotentialStates (State reachable) const {
      return reachable ;
    }

    /** Use delegation on net_ */
    Label getName() const {
      return name_;
    }

    std::ostream& print(std::ostream& os) const {
      spot::print_dot(os, tgba_);// TODO : use tgba_dump
      os << "Internal view (arcs):" << std::endl;
      for (arcs_t::const_iterator it = arcs_.begin() ; it != arcs_.end() ; ++it ) {
	os << get_arc_label(it->first);
	os << it->first.first.id() << "/" << it->first.second << "  ";
	os << "   : ";
	for (tgba_arcs_it jt = it->second.begin() ; jt != it->second.end() ; ++jt) {
	  os << jt->first << "->" << jt->second << " ;" ;
	}
	os << std::endl;
      }
      os << "Internal view (labmap):" << std::endl;
      for (labmap_t::const_iterator it = labmap_.begin() ; it != labmap_.end() ; ++it) {
	os << it->first << " mapsto " << get_arc_label(it->second) << "  ";
	os << it->second.first.id() << "/" << it->second.second << "  ";
	os << std::endl;
      }
      os << std::endl;
      return os;
    }

    /** Print a set of states, explicitly.
     *  Watch out, do not call on large its::State (>10^6). */
    long printState (State s, std::ostream & os, long limit) const {
      os << s << std::endl ;
      return s.nbStates();
    }


    /** Return a Transition that maps states to their observation class.
     *  Observation class is based on the provided set of observed variables,
     *  in standard "." separated qualified variable names.
     *  The returned Transition replaces the values of non-observed variables
     *  by their domain.
     **/
    Transition observe (labels_t obs, State potential) const {
      if (obs.empty()) {
	return potential;
      } else {
	return Transition::id;
      }
    }



  };

} // namespace its



#endif
