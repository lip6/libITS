#ifndef __TGBA_ITS_HH__
#define __TGBA_ITS_HH__

#include "TypeBasics.hh"
#include "PNet.hh"
#include "tgba/tgba.hh"
#include <iosfwd>
#include <map>


namespace its {

/** Implement a type by adapting the TGBA interface.
 *  Using delegation to build a type from a TGBA */
  class TgbaType : public TypeBasics {
    // the concrete storage class
    spot::tgba * tgba_;
    // used to label SDD node of PNet.
    static const int DEFAULT_VAR = 0;
    // used to name this type
    static const std::string name_ ;
  protected :
    virtual labels_t getVarSet () const {
      return labels_t (1,"state");
    }
  public :
    /** the set InitStates of designated initial states */
    labels_t getInitStates () const {
      return labels_t(1,"init");
    }

    /** the set T of public transition labels : one for each distinct AP formula x acceptance set labeling the tgba arcs*/
    labels_t getTransLabels () const {
      // TODO : navigate through the automaton and grab the required info
      // put it into a hash table : <bdd apcond, bdd acc> -> set< pair<state *, state *> >  (or perhaps using int instead of state*)
      return labels_t();
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
    virtual Transition getSuccs (const labels_t & tau) const {
      // TODO : use the hash table filled up during getTransLabels
      // and union each hom produced by values in the map
      return GShom::id;
    }

    /** To obtain a representation of a labeled state */
    State getState(Label stateLabel) const {
      // only one initial state
      assert (stateLabel == "init");
      // the initial state has id 0 by definition
      return State(DEFAULT_VAR, DDD(0,0));
    }

    /** Use delegation on net_ */
    Label getName() const {
      return name_;
    }

    std::ostream& print(std::ostream& os) const {
      // TODO : use tgba_dump
      return os;
    }

  };

} // namespace its

#endif
