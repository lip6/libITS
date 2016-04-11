#include "gal/Statements.hh"

namespace its {

bool
Assignment::operator== (const Assignment &other) const
{
  return var_.equals (other.var_) && expr_.equals (other.expr_);
}

bool
Assignment::operator< (const Assignment &other) const
{
  return var_.equals (other.var_) ? expr_.less (other.expr_) : var_.less (other.var_);
}

std::set<Variable>
Assignment::getSupport () const
{
  std::set<Variable> result = var_.getSupport ();
  std::set<Variable> tmp = expr_.getSupport ();
  result.insert (tmp.begin (), tmp.end ());
  return result;
}

bool
IncrAssignment::operator== (const IncrAssignment &other) const
{
  return var_.equals (other.var_) && expr_.equals (other.expr_);
}

bool
IncrAssignment::operator< (const IncrAssignment &other) const
{
  return var_.equals (other.var_) ? expr_.less (other.expr_) : var_.less (other.var_);
}

std::set<Variable>
IncrAssignment::getSupport () const
{
  std::set<Variable> result = var_.getSupport ();
  std::set<Variable> tmp = expr_.getSupport ();
  result.insert (tmp.begin (), tmp.end ());
  return result;
}



}  // namespace its