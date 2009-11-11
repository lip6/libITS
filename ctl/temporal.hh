#ifndef TEMPORAL_OPERATORS_HH_1ZYJ9ANA
#define TEMPORAL_OPERATORS_HH_1ZYJ9ANA

namespace ctl {

////////////////////////////////////////////////////////////////////

/// The forall ("A") quantifier.
struct forall {};

////////////////////////////////////////////////////////////////////

/// The exists ("E") quantifier.
struct exists {};

////////////////////////////////////////////////////////////////////

/// The finally temporal ("F") operator.
/// It is an unary operator.
struct finally
{
  /// Constructor.
  finally( const formula& f )
    : content(f)
  {}
  
  /// The sub-formula on which the temporal
  /// operator is applied.
  formula content;

};

////////////////////////////////////////////////////////////////////

/// The finally temporal ("G") operator.
/// It is an unary operator.
struct globally
{
  /// Constructor.
  globally( const formula& f )
    : content(f)
  {}

  /// The sub-formula on which the temporal
  /// operator is applied.  
  formula content;

};

////////////////////////////////////////////////////////////////////

/// The finally temporal ("G") operator.
/// It is an unary operator.
struct next
{
  /// Constructor.
  next( const formula& f )
    : content(f)
  {}
  
  /// The sub-formula on which the temporal
  /// operator is applied.
  formula content;

};

////////////////////////////////////////////////////////////////////

/// The finally temporal ("U") operator.
/// It is a binary operator.
struct until
{
  /// Constructor.
  until( const formula& lhs , const formula& rhs )
    : left_content(lhs)
    , right_content(rhs)
  {}
  
  /// The left sub-formula on which the temporal
  /// operator is applied.
  formula left_content;

  /// The right sub-formula on which the temporal
  /// operator is applied.
  formula right_content;

};

////////////////////////////////////////////////////////////////////

/// This struct stores together a quantifier and a
/// temporal operator. As in CTL temporal operators
/// are always associated with a quantifier, we make
/// here a little shortcut by calling it also a 
/// temporal operator.
struct temporal_operator
{
  
  /// Constructor. It can take any kind of quantifier and
  /// temporal operator.
  template< typename Quantifier , typename Operator >
  temporal_operator( const Quantifier& q , const Operator& _op )
    : quantifier(q)
    , op(_op)
  {}
  
  /// Define the different quantifiers that prefix
  /// temporal operators.
  typedef boost::variant< forall 
                        , exists
                        >
          quantifier_type;
          
  /// Define all temporal operators.        
  typedef boost::variant< finally
                        , globally
                        , next
                        , until
                        >
          operator_type;


  /// Store the quantifier associated to the temporal operator.
  quantifier_type quantifier;
  
  /// The temporal operator itself.
  operator_type   op;

};

////////////////////////////////////////////////////////////////////

} // namespace ctl

#endif // end of include guard: TEMPORAL_OPERATORS_HH_1ZYJ9ANA
