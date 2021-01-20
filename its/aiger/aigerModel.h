#ifndef __AIGER_MODEL_H_
#define __AIGER_MODEL_H_

#include <map>
#include <string>

/// forward declaration
struct aiger;

/// forward declaration
namespace its {
  class BoolExpression;
  class GAL;
  class IntExpression;
}

class aiger2GAL {
  /// the underlying aiger model
  aiger * _aiger;
  /// map aiger latches to GAL variables
  std::map<unsigned, its::IntExpression> _latch2var;
  std::map<unsigned, its::IntExpression> _input2var;

  void collect_variables (its::GAL &);
  void gen_transitions (its::GAL &);

  its::BoolExpression get_value (unsigned) const;

public:
  /// constructor (does not take ownership of the passed aiger)
  explicit aiger2GAL(aiger *);
  /// destructor
  ~aiger2GAL ();

  its::GAL * toGAL ();
};

// the public interface
its::GAL *
load_aiger (const std::string &);

#endif  // __AIGER_MODEL_H_
