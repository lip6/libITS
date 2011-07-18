#include "Observe_Hom.hh"


using namespace std;

namespace its {


class _ObserveVars:public StrongHom {
  
  vars_t observed;
  GDDD potential;
public:
  _ObserveVars(vars_it obs_begin, vars_it obs_end, const GDDD & potential) : observed(obs_begin, obs_end), potential(potential) {}
  
  bool
  skip_variable(int vr) const
  {
    return false;
  }
  
  GDDD phiOne() const {
    return GDDD::one;
  }                   
  
  GHom phi(int vr, int vl) const {
    if (observed.empty()) {
      return potential;
    }
    if (vr == *observed.begin()) {
      // Return current value, drop first observed variable (it has been seen) and chop the head of potential (supposed to be single path in
      // SDD sense, i.e. one node per level/variable only. So all children of pot are the same.)
      return GHom(vr, vl , GHom( _ObserveVars(observed.begin()+1,observed.end(), potential.begin()->second)));
    } else {
      // grab top of potential into a node
      GDDD::Valuation val;
      val.reserve(potential.nbsons());
      for (GDDD::const_iterator it = potential.begin() ; it != potential.end() ; ++it) {
	val.push_back(GDDD::edge_t(it->first, GDDD::one));
      }
      /// return top of potential followed by recursive call to observe on bottom of tree.
      return GDDD(vr,val) ^ GHom( _ObserveVars(observed.begin(),observed.end(), potential.begin()->second) );
    }
  }
  
  size_t hash() const {
    size_t h = 31;
    for (vars_it it = observed.begin() ; it != observed.end() ; ++it ) {
      h = 73*h + *it*17;
    }
    return h  ^ potential.hash();
  }

  void mark() const{
    potential.mark();
  }

  /// Invert ???

  bool is_selector () const {
    return false;
  }

  void print (std::ostream & os) const {
    os << "[ Observe: " ;
    for (vars_it it = observed.begin() ; it != observed.end() ; ++it ) {
      os << *it <<  " ";
    }
    os << "]";
  }

  bool operator==(const StrongHom &s) const {
    _ObserveVars* ps = (_ObserveVars*)&s;
    return potential == ps->potential && observed == ps->observed ;
  }

  _GHom * clone () const {  return new _ObserveVars(*this); }
};


}
