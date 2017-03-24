#ifndef __STATE__CACHER__
#define __STATE__CACHER__

#include "its/TypeDecorator.hh"

namespace its {

  /** A proxy type implementation of a cache for a Type implementation. */
  class TypeCacher : public TypeDecorator {
    // a cache for state constructions
    typedef std::map<vLabel,State> state_cache_t;
    mutable state_cache_t state_cache;
    // a cache for succ constructions
    typedef std::vector<labels_t > tkey_t;
    mutable tkey_t trans_keys;
    mutable std::vector<Transition > trans_vals;
    // a cache for locals
    mutable Transition locals_;    
  public :
    // pass the concrete type and memory management
    TypeCacher (Type * concrete) : TypeDecorator(concrete), locals_(GSDD::top) {};
    ~TypeCacher() {}

    /** order */
    bool setVarOrder (labels_t vars) const { 
      state_cache.clear();
      trans_vals.clear();
      locals_ = Transition(GSDD::top);
        return TypeDecorator::setVarOrder(vars); 
    }
    

    /** state and transitions representation functions */
    /** Cache is used . */
    /** Local transitions */
    Transition getLocals () const ;
    
    /** Successors synchronization function : Bag(T) -> SHom.
     * The collection represented by the iterator should be a multiset
     * of transition labels of this type (as obtained through getTransLabels()).
     * Otherwise, an assertion violation will be raised !!
     * Cache is used
     * */
    Transition getSuccs (const labels_t & tau) const ;
    
    /** To obtain a representation of a labeled state */
    /** Cache is used */
    State getState(Label stateLabel) const;
    
  };

}

#endif
