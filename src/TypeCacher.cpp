#include "TypeCacher.hh"


namespace its {

   /** state and transitions representation functions */
    /** Cache is used . */
    /** Local transitions */
  Transition TypeCacher::getLocals () const {
	if (locals_ == GSDD::top) {
	  locals_ = concrete_->getLocals();
	}
	return locals_;
  }
    
    /** Successors synchronization function : Bag(T) -> SHom.
     * The collection represented by the iterator should be a multiset
     * of transition labels of this type (as obtained through getTransLabels()).
     * Otherwise, an assertion violation will be raised !!
     * Cache is used
     * */
  Transition TypeCacher::getSuccs (const labels_t & tau) const {
    	labels_t ctau = tau;
	// sort the argument to improve cache hits (canonical form)
	sort(ctau.begin(),ctau.end());
	tkey_t::const_iterator tcit = find(trans_keys.begin(), trans_keys.end(), ctau);
	if (tcit == trans_keys.end() )
	{
	  // cache miss
	  Transition res = concrete_->getSuccs(ctau);
	  // Update the cache : it is stored as two congruent vector of values for keys and vals
	  trans_keys.push_back(ctau);
	  trans_vals.push_back(res);
	  return res;
	} else {
	  // return the stored cache value
	  return trans_vals[tcit - trans_keys.begin()];
	}
  }
    
    /** To obtain a representation of a labeled state */
    /** Cache is used */
  State TypeCacher::getState(Label stateLabel) const {
    // check cache
    state_cache_t::const_iterator scit = state_cache.find(stateLabel);
    if (scit == state_cache.end()) {
      //cache miss
      State res = concrete_->getState(stateLabel);
      // update cache
      state_cache[stateLabel] = res;
      return res;
    } else {
      // cache hit
      return scit->second;
    }
    
  }


} // namespace
