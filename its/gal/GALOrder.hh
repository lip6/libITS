#ifndef __GAL__ORDER__HH__
#define __GAL__ORDER__HH__

#include "its/Ordering.hh"

namespace its {

  
  /** Used to map SDD variables (integers) to correspoding IntExpression (Var or const array access). */
  class GalOrder {
    typedef std::map<int, IntExpression> map_t;
    map_t map_;

    typedef std::map<std::string, int> array_size_t;
    array_size_t array_size;
  public :
    GalOrder (const VarOrder * vo) {
      for (size_t i=0; i < vo->size() ; ++i) {
	Variable vname = vo->getLabel(i);
        IntExpression mi = IntExpressionFactory::createVariable(vname);
        map_[i] = mi;
        if (mi.getType () == CONSTARRAY)
        {
          array_size_t::iterator it = array_size.find (vname.getArrayName ());
          if (it != array_size.end ())
          {
            if (vname.getIndex () > it->second)
              it->second = vname.getIndex ();
          }
          else
          {
            array_size[vname.getArrayName ()] = vname.getIndex ();
          }
        }
      }
    }
    // Return the <var,index> of the IntExpr var that represents this xDD variable.
    const IntExpression & getVar (int index) const {
      return map_.find(index)->second;
    }
    size_t size() const { return map_.size() ; }

    bool isValidAddress (Label aname, int index) const
    {
      array_size_t::const_iterator it = array_size.find (aname);
//      std::cerr << "calling isvalid for " << aname << "," << index << std::endl;
      assert (it != array_size.end ());
//      std::cerr << "compare " << it->second << " and " << index << std::endl;
      return it->second >= index && index >= 0;
    }
    
    int getIndex (const IntExpression & vname) const
    {
      for (map_t::const_iterator it = map_.begin ();
           it != map_.end (); ++it)
      {
        if (it->second.equals (vname))
          return it->first;
      }
      assert (false);
      return -1;
    }
  };



}



#endif


