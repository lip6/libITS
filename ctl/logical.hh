#ifndef LOGICAL_OPERATORS_HH_F2FHKIPA
#define LOGICAL_OPERATORS_HH_F2FHKIPA

#include "ctl/formula.hh"

namespace ctl {

////////////////////////////////////////////////////////////////////

struct logical_and
{
  logical_and( const formulae& op )
    : operands(op)
  {}
  
  formulae operands;
};

////////////////////////////////////////////////////////////////////

struct logical_not
{
  logical_not( const formula& f )
    : operand(f)
  {}

  formula operand;
};

////////////////////////////////////////////////////////////////////

struct logical_or
{
  logical_or( const formulae& op )
    : operands(op)
  {}

  formulae operands;
};

////////////////////////////////////////////////////////////////////

struct logical_xor
{
  logical_xor( const formulae& op )
    : operands(op)
  {}
  
  formulae operands;
};

////////////////////////////////////////////////////////////////////

struct logical_implies
{
  logical_implies( const formula& lhs , const formula& rhs)
    : left(lhs)
    , right(rhs)
  {}
  
  formula left;
  formula right;
};

////////////////////////////////////////////////////////////////////

struct logical_eq
{
  logical_eq( const formula& lhs , const formula& rhs)
    : left(lhs)
    , right(rhs)
  {}
  
  formula left;
  formula right;
};

////////////////////////////////////////////////////////////////////

struct logical_operator
{
  template < typename LogicalOperator >
  logical_operator( const LogicalOperator& log_op )
    : op(log_op)
  {}
  
  typedef boost::variant< logical_and
                        , logical_or
                        , logical_not
                        , logical_xor
                        , logical_implies
                        , logical_eq
                        >
          logical_operator_type;
          
  logical_operator_type op;
};

////////////////////////////////////////////////////////////////////

} // namespace ctl

#endif // end of include guard: LOGICAL_OPERATORS_HH_F2FHKIPA
