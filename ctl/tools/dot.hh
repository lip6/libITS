#ifndef DOT_HH_2CRO84RK
#define DOT_HH_2CRO84RK

#include <ostream>

#include <boost/variant.hpp>

#include "ctl/formula.hh"

namespace ctl {

////////////////////////////////////////////////////////////////////

/// A visitor to produce the DOT representation of a formula.
/// It is a good example of how a formula can be visited.
struct dot_formula_visitor
  : public boost::static_visitor<unsigned long>
{
  
  /// The stream we export the formula to.
  std::ostream& os;
  
  /// The ID of the currently visited formula node.
  /// We need an unique ID for each node, otwherwise we
  /// could not differentiate, for instance, a AND from
  /// another AND as we would only have their names.
  /// Mutable because the visitor is propagated as a
  /// const ref.
  mutable unsigned long id;
  
  /// Constructor.
  dot_formula_visitor( std::ostream& _os )
    : os(_os)
    , id(0)
  {
    os << "digraph ctl_formula {"
       << std::endl
       << std::endl
       << "node [shape=square];"
       << std::endl
       << std::endl;
  }
  
  /// Destructor.
  ~dot_formula_visitor()
  {
    os << std::endl
       << "}" 
       << std::endl;
  }
  
  ////////////////////////////////////////////////////////////////////
  
  unsigned long
  operator()( const True& )
  const
  {
    ++id;
    os << id
       << " [label=\"True\"];"
       << std::endl;
    return id;
  }

  ////////////////////////////////////////////////////////////////////
  
  unsigned long
  operator()( const False& )
  const
  {
    ++id;
    os << id
       << " [label=\"False\"];"
       << std::endl;
    return id;
  }
  
  ////////////////////////////////////////////////////////////////////
  
  /// Atomic property.
  unsigned long
  operator()( const property* p )
  const
  {
    ++id;
    os << id
       << " [label=\""
       << p->to_string()
       << "\"];"
       << std::endl;
    return id;
  }
  
  ////////////////////////////////////////////////////////////////////
  
  /// When a logical operator is encountered, this method calls
  /// once again the visitor on the real contained operator.
  unsigned long
  operator()( const logical_operator& lop )
  const
  {
    return boost::apply_visitor( *this , lop.op );
  }
  
  ////////////////////////////////////////////////////////////////////
  
  /// When a temporal operator is encountered, this method calls
  /// once again the visitor on the real contained operator, along
  /// with the quantifier.
  unsigned long
  operator()( const temporal_operator& top )
  const
  {
    return boost::apply_visitor( *this , top.quantifier , top.op );
  }
  
  ////////////////////////////////////////////////////////////////////
  
  /// AND opertor.
  unsigned long
  operator()( const logical_and& op )
  const
  {
    unsigned long current_id = ++id;
    os << current_id << " [label=\"AND\"];" << std::endl;
    for( formulae::const_iterator cit = op.operands.begin()
       ; cit != op.operands.end() ; ++cit )
    {
      os << current_id 
         << " -> " 
         << boost::apply_visitor( *this , *cit )
         << ";"
         << std::endl;
    }
    return current_id;
  }

  ////////////////////////////////////////////////////////////////////

  /// NOT operator.
  unsigned long
  operator()( const logical_not& op )
  const
  {
    unsigned long current_id = ++id;
    os << current_id 
       << " [label=\"NOT\"];"
       << std::endl
       << current_id
       << " -> " 
       << boost::apply_visitor( *this , op.operand )
       << ";"
       << std::endl;
    return current_id;
  }

  ////////////////////////////////////////////////////////////////////

  /// OR operator.
  unsigned long
  operator()( const logical_or& op )
  const
  {
    unsigned long current_id = ++id;
    os << current_id << " [label=\"OR\"];" << std::endl;
    for( formulae::const_iterator cit = op.operands.begin()
       ; cit != op.operands.end() ; ++cit )
    {
      os << current_id 
         << " -> " 
         << boost::apply_visitor( *this , *cit )
         << ";"
         << std::endl;
    }
    return current_id;    
  }

  ////////////////////////////////////////////////////////////////////

  /// XOR operator.
  unsigned long
  operator()( const logical_xor& op )
  const
  {
    unsigned long current_id = ++id;
    os << current_id 
       << " [label=\"XOR\"];" 
       << std::endl;
    for( formulae::const_iterator cit = op.operands.begin()
       ; cit != op.operands.end() ; ++cit )
    {
      os << current_id 
         << " -> " 
         << boost::apply_visitor( *this , *cit )
         << ";"
         << std::endl;
    }
    return current_id;    
  }

  ////////////////////////////////////////////////////////////////////

  /// <=> operator.
  unsigned long
  operator()( const logical_eq& op )
  const
  {
    unsigned long current_id = ++id;
    os << current_id 
       << " [label=\"<=>\"];" 
       << std::endl
       << current_id
       << " -> "
       << boost::apply_visitor( *this , op.left)
       << "  [label=\"left\"];"
       << std::endl
       << current_id
       << " -> "
       << boost::apply_visitor( *this , op.right)
       << "  [label=\"right\"];"
       << std::endl;
    return current_id;    
  }

  ////////////////////////////////////////////////////////////////////

  /// => operator.
  unsigned long
  operator()( const logical_implies& op )
  const
  {
    unsigned long current_id = ++id;
    os << current_id 
       << " [label=\"=>\"];" 
       << std::endl
       << current_id
       << " -> "
       << boost::apply_visitor( *this , op.left)
       << "  [label=\"left\"];"
       << std::endl
       << current_id
       << " -> "
       << boost::apply_visitor( *this , op.right)
       << "  [label=\"right\"];"
       << std::endl;
    return current_id;    
  }

  ////////////////////////////////////////////////////////////////////

  /// AF operator.
  unsigned long
  operator()( const forall& , const finally& f )
  const
  {
    unsigned long current_id = ++id;
    os << current_id
       << " [label=\"AF\"];"
       << std::endl
       << current_id
       << " -> "
       << boost::apply_visitor( *this , f.content )
       << ";"
       << std::endl;
    return current_id;    
  }

  ////////////////////////////////////////////////////////////////////

  /// EF operator.
  unsigned long
  operator()( const exists& , const finally& f )
  const
  {
    unsigned long current_id = ++id;
    os << current_id
       << " [label=\"EF\"];"
       << std::endl
       << current_id
       << " -> "
       << boost::apply_visitor( *this , f.content )
       << ";"
       << std::endl;
    return current_id;        
  }

  ////////////////////////////////////////////////////////////////////

  /// AG operator.
  unsigned long
  operator()( const forall& , const globally& f )
  const
  {
    unsigned long current_id = ++id;
    os << current_id
       << " [label=\"AGF\"];"
       << std::endl
       << current_id
       << " -> "
       << boost::apply_visitor( *this , f.content )
       << ";"
       << std::endl;
    return current_id;        
  }

  ////////////////////////////////////////////////////////////////////

  /// EG operator.
  unsigned long
  operator()( const exists& , const globally& f )
  const
  {
    unsigned long current_id = ++id;
    os << current_id
       << " [label=\"EG\"];"
       << std::endl
       << current_id
       << " -> "
       << boost::apply_visitor( *this , f.content )
       << ";"
       << std::endl;
    return current_id;        
  }

  ////////////////////////////////////////////////////////////////////

  /// AX operator.
  unsigned long
  operator()( const forall& , const next& f )
  const
  {
    unsigned long current_id = ++id;
    os << current_id
       << " [label=\"AX\"];"
       << std::endl
       << current_id
       << " -> "
       << boost::apply_visitor( *this , f.content )
       << ";"
       << std::endl;
    return current_id;        
  }

  ////////////////////////////////////////////////////////////////////

  /// EX operator.
  unsigned long
  operator()( const exists& , const next& f )
  const
  {
    unsigned long current_id = ++id;
    os << current_id
       << " [label=\"EX\"];"
       << std::endl
       << current_id
       << " -> "
       << boost::apply_visitor( *this , f.content )
       << ";"
       << std::endl;
    return current_id;        
  }

  ////////////////////////////////////////////////////////////////////

  /// AU operator.
  unsigned long
  operator()( const forall& , const until& f )
  const
  {
    unsigned long current_id = ++id;
    os << current_id
       << " [label=\"AU\"];"
       << std::endl
       << current_id
       << " -> "
       << boost::apply_visitor( *this , f.left_content )
       << " [label=\"left\"];"
       << std::endl
       << current_id
       << " -> "
       << boost::apply_visitor( *this , f.right_content )
       << " [label=\"right\"];"
       << std::endl;
    return current_id;        
  }

  ////////////////////////////////////////////////////////////////////

  /// EU operator.
  unsigned long
  operator()( const exists& , const until& f )
  const
  {
    unsigned long current_id = ++id;
    os << current_id
       << " [label=\"EU\"];"
       << std::endl
       << current_id
       << " -> "
       << boost::apply_visitor( *this , f.left_content )
       << " [label=\"left\"];"
       << std::endl
       << current_id
       << " -> "
       << boost::apply_visitor( *this , f.right_content )
       << " [label=\"right\"];"
       << std::endl;
    return current_id;    
  }
  
  ////////////////////////////////////////////////////////////////////
  
};

////////////////////////////////////////////////////////////////////

/// Export a formula to an ostream in the DOT format.
std::ostream&
operator<<( std::ostream& os , const formula& f )
{
  boost::apply_visitor( dot_formula_visitor(os) , f );
  return os;
}

////////////////////////////////////////////////////////////////////

}

#endif // end of include guard: DOT_HH_2CRO84RK
