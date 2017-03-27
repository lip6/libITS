#include "its/gal/GAL2C.hh"

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
GAL2C::new_label(const std::string& olds, const std::string& news) const
{
  auto it = vars2index.find(olds);
  if (it != vars2index.end())
    return news + "[" + std::to_string(it->second) + "]";

  it = arrays2index.find(olds);
  if (it != arrays2index.end())
    return "(" + news + "+" + std::to_string(it->second) + ")";

  throw std::runtime_error("variable " + olds + " not found while compiling gal model");
}

// to print the intexpression, it suffices to change the variable labels
// use the createIntExpression method, only the environment changes
env_t
GAL2C::remap_env(const env_t& e, const std::string& vname) const
{
  // find the new environment
  env_t res(e.size());
  for (size_t i = 0; i != e.size(); ++i)
  {
    std::string v = IntExpressionFactory::getVar(e[i]);
    res[i] = IntExpressionFactory::getVarIndex(new_label(v, vname));
  }
  return res;
}

IntExpression
GAL2C::remap_expr(const IntExpression& expr, const std::string& vname) const
{
  return IntExpressionFactory::createIntExpression(expr.getExpr(), remap_env(expr.getEnv(), vname));
}
BoolExpression
GAL2C::remap_expr(const BoolExpression& expr, const std::string& vname) const
{
  return BoolExpressionFactory::createBoolExpression(expr.getExpr(), remap_env(expr.getEnv(), vname));
}

class ActionPrinter : public StatementVisitor
{
public:
  ActionPrinter(const GAL2C& g, std::ostream& o): g_(g), os(o), next_(0) {}
  ~ActionPrinter() = default;

  void
  visitAssign(const Assignment& a) override
  {
    os << "\t\t" << g_.remap_expr(a.getVariable(), "s") << " = " << g_.remap_expr(a.getExpression(), "s") << ";\n";
  }

  void
  visitIncrAssign(const IncrAssignment& a) override
  {
    os << "\t\t" << g_.remap_expr(a.getVariable(), "s") << " += " << g_.remap_expr(a.getExpression(), "s") << ";\n";
  }

  void
  visitSequence(const Sequence& s) override
  {
    // split the sequence into subsequences
    // each subsequence contains at most one Call, and if so it is its last statement
    // furthermore, subsequences should be maximal (at most one that contains no Call)
    // TODO in full generality, Call above should be replaced by any non-deterministic Statement
    std::vector<std::vector<const Statement*>> subsequences;
    subsequences.push_back({});
    for (const auto& stat : s)
    {
      subsequences.rbegin()->push_back(stat);
      if (stat->getType() == its::CALL)
        subsequences.push_back({});
    }
    // if the last statement is a call, the last subsequence is empty: remove it.
    if (subsequences.back().empty())
      subsequences.pop_back();

    // generate a callback for each subsequence
    for (unsigned i = 0; i != subsequences.size(); ++i)
    {
      os << "\tauto call_" << i << " = [](int* s, TransitionCB callback, void* context)\n";
      os << "\t{\n";
      for (const auto& s : subsequences[i])
        s->acceptVisitor(*this);
      if (subsequences[i].back()->getType() != its::CALL)
        os << "\t\tcallback(context, nullptr, s);\n";
      os << "\t};\n";
    }

    os << "\tcount_context c = { 0, ctx };\n";
    for (unsigned i = 0; i <= subsequences.size(); ++i)
    {
      os << "\tmycontext c" << i << " = { ";
      if (i == 0)
      {
        os << "wrapper_count, cb, &c";
      }
      else
      {
        os << "call_" << subsequences.size() - i << ", wrapper, &c" << (i-1);
      }
      os << "};\n";
    }

    os << "\tint dst[" << g_.next_ << "];\n";
    os << "\tstd::copy(in, in+" << g_.next_ << ", dst);\n";
    os << "\twrapper(&c" << subsequences.size() << ", nullptr, dst);\n";
    os << "\treturn c.nb;\n";
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
  visitCall(const Call& c)
  {
    for (auto tit = g_.gal_.trans_begin(); tit != g_.gal_.trans_end(); ++tit)
    {
      if (tit->getLabel() == c.getLabel())
        os << "\t\tdo_" << tit->getName() << "(s, callback, context);\n";
    }
  }

  void
  visitAbort()
  {
    /* would not be hard to do nothing... */
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
  int next_;
  std::map<const its::Statement*, std::string> names_;
};

void
GAL2C::dump(std::ostream& os) const
{
  // headers
  os << "#include <algorithm>\n\n\n";

  // spins interface
  os << "class transition_info_t;\n";
  os << "typedef void (*TransitionCB)(void* ctx, transition_info_t* transition_info, int* dst);\n";
  os << "typedef void (*run_trans)(int*, TransitionCB, void*);\n";
  os << "struct count_context { int nb; void* ctx; };\n";
  os << "struct mycontext { run_trans f; TransitionCB cb; void* ctx; };\n\n";

  os << "void wrapper(void* c, transition_info_t*, int* s)\n";
  os << "{\n";
  os << "\tmycontext* cc = (mycontext*)c;\n";
  os << "\tcc->f(s, cc->cb, cc->ctx);\n";
  os << "};\n\n";

  os << "void wrapper_count(int* s, TransitionCB cb, void* c)\n";
  os << "{\n";
  os << "\tcount_context* cc = (count_context*)c;\n";
  os << "\tcb(cc->ctx, nullptr, s);\n";
  os << "\t++cc->nb;\n";
  os << "}\n\n";

  // export transitions
  ActionPrinter ap(*this, os);
  for (auto tit = gal_.trans_begin(); tit != gal_.trans_end(); ++tit)
  {
    os << "bool enabled_" << tit->getName() << "(const int* s)\n";
    os << "{\n";
    os << "\treturn " << remap_expr(tit->getGuard(), "s") << ";\n";
    os << "}\n";

    os << "int do_" << tit->getName() << "(const int* in, TransitionCB cb, void* ctx)\n";
    os << "{\n";
    os << "\tif (! enabled_" << tit->getName() << "(in)) return 0;\n\n";
    ap.visitSequence(tit->getAction());
    os << "}\n\n";
  }

  os << "extern \"C\" void get_initial_state(void* ss)\n";
  os << "{\n";
  os << "\tint* s = (int*)ss;\n";
  for (auto vit = gal_.state_begin(); vit != gal_.state_end(); ++vit)
  {
    os << "\t" << new_label(vit->first, "s") << " = " << vit->second << ";\n";
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


  // call all transitions
  os << "extern \"C\" int get_successors(void* , int* in, TransitionCB cb, void* ctx)\n";
  os << "{\n";
  os << "\tint res = 0;\n";
  for (auto tit = gal_.trans_begin(); tit != gal_.trans_end(); ++tit)
  {
    if (tit->getLabel().empty())
      os << "\tres += do_" << tit->getName() << "(in, cb, ctx);\n";
  }
  os << "\treturn res;\n";
  os << "}\n";

  os << "\n";
}

} // namespace its

