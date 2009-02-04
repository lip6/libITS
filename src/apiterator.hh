#ifndef AP_ITERATOR_HH
#define AP_ITERATOR_HH

#include <cassert>
#include <vector>
#include "bvec.h"

namespace sogits {



class APIterator {
public:
  typedef std::vector<int> varset_t;
  

  APIterator(const varset_t & vars)
    : cur(bvec_true(vars.size())), size(vars.size()), last(true) {
    int tabvar [size];
    std::copy(vars.begin() , vars.end() , tabvar );
    var = bvec_varvec(vars.size(), tabvar);
  }

  void first() {
    last = false;
    cur = bvec_false(size);
  }

  void next() {
    assert(!done());
    if (bvec_val(cur) == bvec_val(bvec_true(size)))
      last = true;
    else
      cur = cur + bvec_con(size, 1);
  }
  bool done() const {
    return last;
  }
  bdd current() const {
    assert(!done());
    return var == cur;
  }
private:
  bvec var, cur;
  int size;
  bool last;
};


class APIteratorFactory {
public :

  static void setAPVarSet (const APIterator::varset_t & vars) {
    vars_ = vars;
  }
  
  static APIterator create () {
    return APIterator(vars_);
  }

private :

  static APIterator::varset_t vars_ ;
};

}

#endif
