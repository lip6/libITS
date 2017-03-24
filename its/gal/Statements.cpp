#include "its/gal/Statements.hh"

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

SyncAssignment::SyncAssignment (const std::vector<assign_t> &a)
: _assignments (a)
{}

std::set<Variable>
SyncAssignment::getSupport () const
{
  std::set<Variable> result;
  for (auto const & p : _assignments)
  {
    std::set<Variable> tmp;
    tmp = p.first.getSupport ();
    result.insert (tmp.begin (), tmp.end ());
    tmp = p.second.getSupport ();
    result.insert (tmp.begin (), tmp.end ());
  }
  return result;
}

SyncAssignment
SyncAssignment::operator& (const Assertion &ass) const
{
  std::vector<std::pair<IntExpression, IntExpression>> res;
  for (auto const & assign : _assignments)
  {
    res.push_back (std::make_pair (assign.first, (assign.second & ass).eval ()));
  }
  return SyncAssignment (res);
}

void
SyncAssignment::print (std::ostream &os, int ind) const
{
  indent (os, ind);
  os << "Sync Assignment { " << std::endl;
  ++ind;
  for (auto const & ass : _assignments)
  {
    indent (os, ind);
    os << ass.first << " := " << ass.second << std::endl;
  }
  --ind;
  indent (os, ind);
  os << "}" << std::endl;
}

}  // namespace its