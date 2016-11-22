#include "GAL2C.hh"

namespace its
{

GAL2C::GAL2C(const GAL& g)
  : gal_(g)
  , next_(0)
{
  // register variables
  for (auto vit = gal_.vars_begin(); vit != gal_.vars_end(); ++vit)
  {
    vars2index[vit->getName()] = next_;
    ++next_;
  }
  for (auto ait = gal_.arrays_begin(); ait != gal_.arrays_end(); ++ait)
  {
    arrays2index[ait->getName()] = next_;
    for (size_t i = 0; i != ait->size(); ++i)
    {
      vars2index[IntExpressionFactory::createArrayAccess(ait->getName(), i, ait->size()).getName()] = next_;
      ++next_;
    }
  }
}

std::string
GAL2C::new_label(const std::string& s) const
{
  auto it = vars2index.find(s);
  if (it != vars2index.end())
    return "s[" + std::to_string(it->second) + "]";

  it = arrays2index.find(s);
  if (it != arrays2index.end())
    return "(s + " + std::to_string(it->second) + ")";

  throw std::runtime_error("variable " + s + " not found while compiling gal model");
}

// to print the intexpression, it suffices to change the variable labels
// use the createIntExpression method, only the environment changes
env_t
GAL2C::remap_env(const env_t& e) const
{
  // find the new environment
  env_t res(e.size());
  for (size_t i = 0; i != e.size(); ++i)
  {
    std::string v = IntExpressionFactory::getVar(e[i]);
    res[i] = IntExpressionFactory::getVarIndex(new_label(v));
  }
  return res;
}

IntExpression
GAL2C::remap_expr(const IntExpression& expr) const
{
  return IntExpressionFactory::createIntExpression(expr.getExpr(), remap_env(expr.getEnv()));
}
BoolExpression
GAL2C::remap_expr(const BoolExpression& expr) const
{
  return BoolExpressionFactory::createBoolExpression(expr.getExpr(), remap_env(expr.getEnv()));
}

class ActionPrinter : public StatementVisitor
{
public:
  ActionPrinter(const GAL2C& g, std::ostream& o): g_(g), os(o) {}
  ~ActionPrinter() = default;

  void
  visitAssign(const Assignment& a) override
  {
    os << "\t" << g_.remap_expr(a.getVariable()) << " = " << g_.remap_expr(a.getExpression()) << ";\n";
  }

  void
  visitIncrAssign(const IncrAssignment& a) override
  {
    os << "\t" << g_.remap_expr(a.getVariable()) << " += " << g_.remap_expr(a.getExpression()) << ";\n";
  }

  void
  visitSequence(const Sequence& s) override
  {
    for (auto it = s.begin(); it != s.end(); ++it)
    {
      (*it)->acceptVisitor(*this);
    }
  }

  void
  visitIte(const Ite&)
  {
    throw std::runtime_error("ITE not handled");
  }

  void
  visitFix(const FixStatement&)
  {
    throw std::runtime_error("fixpoint not handled");
  }

  void
  visitCall(const Call&)
  {
    throw std::runtime_error("call not handled");
  }

  void
  visitAbort()
  {
    throw std::runtime_error("abort not handled");
  }

  void
  visitSyncAssign(const SyncAssignment&)
  {
    throw std::runtime_error("synchronous assignment not handled");
  }

private:
  const GAL2C& g_;
  std::ostream& os;
};

void
GAL2C::dump(std::ostream& os) const
{
  // headers
  os << "#include <algorithm>\n\n\n";

  // export transitions
  ActionPrinter ap(*this, os);
  for (auto tit = gal_.trans_begin(); tit != gal_.trans_end(); ++tit)
  {
    os << "bool enabled_" << tit->getName() << "(const int* s)\n";
    os << "{\n";
    os << "\treturn " << remap_expr(tit->getGuard()) << ";\n";
    os << "}\n";

    os << "void " << tit->getName() << "(int* s)\n";
    os << "{\n";
    ap.visitSequence(tit->getAction());
    os << "}\n\n";
  }

  os << "extern \"C\" void get_initial_state(void* ss)\n";
  os << "{\n";
  os << "\tint* s = (int*)ss;\n";
  for (auto vit = gal_.state_begin(); vit != gal_.state_end(); ++vit)
  {
    os << "\t" << new_label(vit->first) << " = " << vit->second << ";\n";
  }
  os << "}\n\n";

  os << "extern \"C\" int get_state_variable_count()\n";
  os << "{\n";
  os << "\treturn " << next_ << ";\n";
  os << "}\n\n";

  os << "const char* varnames[" << next_ << "] = { ";
  for (int i = 0; i != next_; ++i)
  {
    auto it = std::find_if(vars2index.begin(), vars2index.end(),
        [i](const std::pair<std::string, int>& p) { return p.second == i; });
    os << "\"" << it->first << "\"";
    if (i != next_-1)
      os << ", ";
  }
  os << " };\n";
  os << "extern \"C\" const char* get_state_variable_name(int i)\n";
  os << "{\n";
  os << "\treturn varnames[i];\n";
  os << "}\n\n";

  os << "extern \"C\" int get_state_variable_type(int)\n";
  os << "{\n";
  os << "\treturn 0;\n";
  os << "}\n\n";

  os << "extern \"C\" int get_state_variable_type_count()\n";
  os << "{\n";
  os << "\treturn 1;\n";
  os << "}\n\n";

  os << "extern \"C\" const char* get_state_variable_type_name(int)\n";
  os << "{\n";
  os << "\treturn \"int\";\n";
  os << "}\n\n";

  os << "extern \"C\" int get_state_variable_type_value_count(int)\n";
  os << "{\n";
  os << "\treturn 0;\n";
  os << "}\n\n";

  os << "extern \"C\" const char* get_state_variable_type_value(int, int)\n";
  os << "{\n";
  os << "\treturn \"\";\n";
  os << "}\n\n";

  // spins interface
  os << "class transition_info_t;\n";
  os << "typedef void (*TransitionCB)(void* ctx, transition_info_t* transition_info, int* dst);\n\n";

  // call all transitions
  os << "extern \"C\" int get_successors(void* , int* in, TransitionCB cb, void* ctx)\n";
  os << "{\n";
  os << "\tint res = 0;\n";
  os << "\tint dst[" << next_ << "];\n";
  for (auto tit = gal_.trans_begin(); tit != gal_.trans_end(); ++tit)
  {
    os << "\tif (enabled_" << tit->getName() << "(in))\n";
    os << "\t{\n";
    os << "\t\t++res;\n";
    os << "\t\tstd::copy(in, in + " << next_ << ", dst);\n";
    os << "\t\t" << tit->getName() << "(dst);\n";
    os << "\t\tcb(ctx, nullptr, dst);\n";
    os << "\t}\n";
  }
  os << "\treturn res;\n";
  os << "}\n";

  os << "\n";
}

} // namespace its

