#include "ctl/parser/makers.hh"

namespace ctl
{

////////////////////////////////////////////////////////////////////

formula
make_constant(bool b)
{
  if( b )
  {
    return formula( True() ); 
  }
  else
  {
    return formula( False() ); 
  }
}

////////////////////////////////////////////////////////////////////

/// Visitor to flatten operands of the same logical operator.
/// i.e. "A and B and C" => "and(A,B,C)".
template< typename LogicalOperator >
struct flatten_visitor
  : boost::static_visitor<>
{
  
  /// The set of operands we want to construct.
  /// Mutable because the current visitor will be passed
  /// as a const reference.
  mutable formulae& operands;
  
  /// Constructor.
  flatten_visitor(formulae& op)
    : operands(op)
  {}
  
  /// Applied when the encountered type is the expected logical
  /// operator: and, or , xor.
  /// In this case, we visit each of their operands to put them
  /// in the our current operands.
  void
  operator()( const LogicalOperator& lop )
  const
  {
    for( formulae::const_iterator cit = lop.operands.begin() 
       ; cit != lop.operands.end() ; ++ cit )
    {
      boost::apply_visitor( *this , *cit );
    }
  }
  
  /// When the encoutered type is a logical operator, we have
  /// to propagate the visit on the real logical operator.
  void
  operator()( const logical_operator& lop )
  const
  {
    boost::apply_visitor( *this , lop.op );
  }
  
  /// When the encoutered type os not a logical operator, we just
  /// put it in our current operands.
  template< typename T >
  void
  operator()( const T& x)
  const
  {
    operands.push_back(x);
  }
  
};

////////////////////////////////////////////////////////////////////

/// Construct an AND logical operator.
/// i.e. "A and B".
formula
make_and( const formula& lhs , const formula& rhs )
{
  formulae operands;
  boost::apply_visitor( flatten_visitor<logical_and>(operands) , lhs );
  boost::apply_visitor( flatten_visitor<logical_and>(operands) , rhs );
  return formula( logical_operator( logical_and(operands) ));
}

////////////////////////////////////////////////////////////////////

/// Construct an OR logical operator.
/// i.e. "A or B".
formula
make_or( const formula& lhs , const formula& rhs )
{
  formulae operands;
  boost::apply_visitor( flatten_visitor<logical_or>(operands) , lhs );
  boost::apply_visitor( flatten_visitor<logical_or>(operands) , rhs );
  return formula( logical_operator( logical_or(operands) ));
}

////////////////////////////////////////////////////////////////////

/// Construct an NOT logical operator.
/// i.e. "not A".
formula
make_not( const formula& f )
{
  return formula( logical_operator( logical_not(f) ));
}

////////////////////////////////////////////////////////////////////

/// Construct a XOR logical operator.
/// i.e. "A xor B".
formula
make_xor( const formula& lhs , const formula& rhs)
{
  formulae operands;
  boost::apply_visitor( flatten_visitor<logical_xor>(operands) , lhs );
  boost::apply_visitor( flatten_visitor<logical_xor>(operands) , rhs );
  return formula( logical_operator( logical_xor(operands) ));
}

////////////////////////////////////////////////////////////////////

formula
make_implies( const formula& lhs , const formula& rhs)
{
  return formula( logical_implies( lhs , rhs ) );
}

////////////////////////////////////////////////////////////////////

formula
make_eq( const formula& lhs , const formula& rhs)
{
  return formula( logical_eq( lhs , rhs ) );
}

////////////////////////////////////////////////////////////////////

/// Construct an Always Finally temporal operator.
/// i.e. "AF(P)".
formula
make_AF( const formula& f )
{
  return formula( temporal_operator( forall() , finally(f) ) );
}

////////////////////////////////////////////////////////////////////

/// Construct an Exists Finally temporal operator.
/// i.e. "EF(P)".
formula
make_EF( const formula& f )
{
  return formula( temporal_operator( exists() , finally(f) ) );
}

////////////////////////////////////////////////////////////////////

/// Construct an Always Globally temporal operator.
/// i.e. "AG(P)".
formula
make_AG( const formula& f )
{
  return formula( temporal_operator( forall() , globally(f) ) );
}

////////////////////////////////////////////////////////////////////

/// Construct an Exists Globally temporal operator.
/// i.e. "EG(P)".
formula
make_EG( const formula& f )
{
  return formula( temporal_operator( exists() , globally(f) ) );
}

////////////////////////////////////////////////////////////////////

/// Construct an Always Next temporal operator.
/// i.e. "AX(P)".
formula
make_AX( const formula& f )
{
  return formula( temporal_operator( forall() , next(f) ) );
}

////////////////////////////////////////////////////////////////////

/// Construct an Exists Next temporal operator.
/// i.e. "EX(P)".
formula
make_EX( const formula& f )
{
  return formula( temporal_operator( exists() , next(f) ) );
}

////////////////////////////////////////////////////////////////////

/// Construct an Always Until temporal operator.
/// i.e. "A(P U Q)".
formula
make_AU( const formula& lhs , const formula& rhs )
{
  return formula( temporal_operator( forall() , until(lhs,rhs) ) );
}

////////////////////////////////////////////////////////////////////

/// Construct an Exists Until temporal operator.
/// i.e. "E(P U Q)".
formula
make_EU( const formula& lhs , const formula& rhs )
{
  return formula( temporal_operator( exists() , until(lhs,rhs) ) );
}

////////////////////////////////////////////////////////////////////

} // namespace ctl
