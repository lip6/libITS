#ifndef __GAL__ORDER__HH__
#define __GAL__ORDER__HH__

#include "Ordering.hh"

namespace its {

  
  /** Used to map SDD variables (integers) to correspoding IntExpression (Var or const array access). */
  class GalOrder {
    typedef std::map<int, IntExpression> map_t;
    map_t map_;

  public :
    GalOrder (const VarOrder * vo) {
      for (size_t i=0; i < vo->size() ; ++i) {
	Variable vname = vo->getLabel(i);
	map_[i] = IntExpressionFactory::createVariable(vname);
      }
    }
    // Return the <var,index> of the IntExpr var that represents this xDD variable.
    const IntExpression & getVar (int index) const {
      return map_.find(index)->second;
    }
    size_t size() const { return map_.size() ; }

  };



}



#endif


