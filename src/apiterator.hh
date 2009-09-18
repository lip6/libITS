#ifndef AP_ITERATOR_HH
#define AP_ITERATOR_HH

#include <cassert>
#include <vector>
#include "bvec.h"

namespace sogits {


  /** This class is designed to iterate over 2^AP : provide the AP
   *  this class provides the bdd representing each of the 2^AP
   *  possible combinations */
class APIterator {
public:
  /// the set of variables
  typedef std::vector<int> varset_t;

  /** initialize from a set of vars */
  APIterator(const varset_t & vars)
    : cur(bvec_true(vars.size())), // initialze all vars to true
      size(vars.size()),  // size
      last(true)  // initially at end
  {
    int tabvar [size];
    std::copy(vars.begin() , vars.end() , tabvar );
    // the bit vector true
    var = bvec_varvec(vars.size(), tabvar);
  }

  void first() {
    last = false;
    // cue = 0,0....0
    cur = bvec_false(size);
  }

  void next() {
    assert(!done());
    if (bvec_val(cur) == bvec_val(bvec_true(size)))
      last = true;
    else
      // add 1 to cur
      cur = cur + bvec_con(size, 1);
  }
  bool done() const {
    return last;
  }
  bdd current() const {
    assert(!done());
    // the bdd representing the current element in 2^AP
    return var == cur;
  }
private:
  // bit vectors : the vars, the current bdd
  bvec var, cur;
  // the bit vector size
  int size;
  // true iff. done
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

  static APIterator* create_new (const APIterator::varset_t & vars) {
    return new APIterator(vars);
  }

private :

  static APIterator::varset_t vars_ ;
};

}

#endif
