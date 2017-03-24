#include "aigerModel.h"

extern "C" {
#include "aiger.h"
}

#include "its/gal/GAL.hh"

aiger2GAL::aiger2GAL (aiger * p)
: _aiger (p)
{}

// nothing to do: _aiger is not owned
aiger2GAL::~aiger2GAL ()
{}

its::GAL *
aiger2GAL::toGAL ()
{
  // \todo use another name for the model
  its::GAL * gal = new its::GAL ("aiger model");
  collect_variables (*gal);
  gen_transitions (*gal);
  return gal;
}

void
aiger2GAL::collect_variables (its::GAL &gal)
{
  std::set<its::Variable> variables;

  // each latch is a state variable
  // latches are initialized to 0 (aiger < 1.9)
  // \todo allow arbitrary initial value (aiger >= 1.9)
  for (unsigned l = 0; l != _aiger->num_latches; ++l)
  {
    std::string latch_name;
    if (_aiger->latches[l].name)
    {
      latch_name = _aiger->latches[l].name;
    }
    else
    {
      std::stringstream tmp;
      tmp << "_var_" << l;
      latch_name = tmp.str ();
    }

    its::Variable latch_var (latch_name);
    // the proper way to detect whether a Variable is an array cell or not is through its::IntExpressionFactory::createVariable
    auto ins_res = _latch2var.insert (std::make_pair (l, its::IntExpressionFactory::createVariable (latch_var)));
    assert (ins_res.second);

    variables.insert (latch_var);
  }

  // also register inputs
  for (unsigned l = 0; l != _aiger->num_inputs; ++l)
  {
    std::string input_name;
    if (_aiger->inputs[l].name)
    {
      input_name = _aiger->inputs[l].name;
    }
    else
    {
      std::stringstream tmp;
      tmp << "_input_" << l;
      input_name = tmp.str ();
    }

    // ensure that the input is not constant
    if (_aiger->inputs[l].lit < 2)
    {
      throw "an input should not be constant...";
    }

    its::Variable input_var (input_name);
    // the proper way to detect whether a Variable is an array cell or not is through its::IntExpressionFactory::createVariable
    auto ins_res = _input2var.insert (std::make_pair (l, its::IntExpressionFactory::createVariable (input_var)));
    assert (ins_res.second);

//    variables.insert (input_var);
  }

  // all input and latch variables have been collected
  // try to detect arrays in the collected variables
  std::map<std::string, unsigned int> arrays;
  for (auto const & var : variables)
  {
    if (var.getArrayName () == var.getName ())
    {
      // regular variable, add it to the GAL directly
      gal.addVariable (var, 0);
    }
    else
    {
      // array variable, update the size of the array
      unsigned int ind = var.getIndex ();
      if (arrays[var.getArrayName ()] < ind+1)
      {
        arrays[var.getArrayName ()] = ind+1;
      }
    }
  }
  // now add the arrays
  for (auto const& arr : arrays)
  {
    gal.addArray (arr.first, its::GAL::vals_t (arr.second, 0));
  }
}

its::BoolExpression
aiger2GAL::get_value (unsigned lit) const
{
  // if the litteral is negated, recursive call
  if (aiger_sign (lit))
  {
    return ! get_value (aiger_not (lit));
  }

  switch (aiger_lit2tag (_aiger, lit))
  {
    case 0: // constant
      return its::BoolExpressionFactory::createConstant (lit == aiger_true);
    case 1: // input
    {
      aiger_symbol * input = aiger_is_input (_aiger, lit);
      for (unsigned l = 0; l != _aiger->num_inputs; ++l)
      {
        if (_aiger->inputs + l == input)
        {
          return _input2var.find (l)->second == its::IntExpressionFactory::createConstant (1);
        }
      }
      // reaching this point means that the input was not registered in _input2var
      throw "imposhiburu";
    }
    case 2: // latch
    {
      aiger_symbol * latch = aiger_is_latch (_aiger, lit);
      for (unsigned l = 0; l != _aiger->num_latches; ++l)
      {
        if (_aiger->latches + l == latch)
        {
          return _latch2var.find (l)->second == its::IntExpressionFactory::createConstant (1);
        }
      }
      // reaching this point means that the latch was not registered in _latch2var
      throw "imposhiburu";
    }
    case 3: // and
    {
      aiger_and * aand = aiger_is_and (_aiger, lit);
      return get_value (aand->rhs0) && get_value (aand->rhs1);
    }

    default: // cannot happen
      throw "unknown tag for aiger litteral";
  }
}

void
aiger2GAL::gen_transitions (its::GAL &gal)
{
  // latches are updated through GAL assignment
  // PB: concurrent update of 2 (or more) interdependent latches
  //     design a dedicated hard-sequence of updates
  // (x = e1) &&& (y = e2)  -> (x = e'1) &&& (y = e'2) -> (x = e'1) & (y = e'2)
  // in details:
  //    first resolve the e1 and e2 to remove the interdependence x <-> y
  //    query for x and y, the interdependence is now gone, and continue with a regular sequence

  // add transitions to set inputs
//  for (unsigned l = 0; l != _aiger->num_inputs; ++l)
//  {
//    its::GuardedAction tmp0 ("");
//    tmp0.getAction ().add (its::Assignment (_input2var.find (l)->second,
//                                           its::IntExpressionFactory::createConstant (0)));
//    gal.addTransition (tmp0);
//
//    its::GuardedAction tmp1 ("");
//    tmp1.getAction ().add (its::Assignment (_input2var.find (l)->second,
//                                            its::IntExpressionFactory::createConstant (1)));
//    gal.addTransition (tmp1);
//  }

  // add a single big synchronous assignment

  // build the big synchronous assignment, that depends on inputs
  std::vector<its::SyncAssignment::assign_t> assigns;
  its::BoolExpression guard;
  for (unsigned l = 0; l != _aiger->num_latches; ++l)
  {
    its::BoolExpression e = get_value (_aiger->latches[l].next);
//    if (l < _aiger->num_latches-1) {
    assigns.push_back (std::make_pair (_latch2var.find (l)->second,
                                       its::IntExpressionFactory::wrapBoolExpr(e).eval ()));
    if (l == _aiger->num_latches-1)
      guard = !e;
//   }
//   else
//   {
//     assigns.push_back (std::make_pair (_latch2var.find (l)->second,
//                                        its::IntExpressionFactory::wrapBoolExpr(! get_value (_aiger->latches[l].next)).eval ()));
//   }
  }



  // @debug
//  for (const auto & ass : assigns)
//  {
//    std::cerr << ass.first << " := " << ass.second << std::endl;
//  }
  // @debug end

  for (unsigned i = 0; i != _aiger->num_inputs; ++i)
  {
    std::vector<its::SyncAssignment::assign_t> new_assigns = assigns;
    its::BoolExpression new_guard = guard;
    for (unsigned j = 0; j != _aiger->num_inputs; ++j)
    {
      int new_value = i == j;
      its::Assertion assertion =
        its::IntExpressionFactory::createAssertion(_input2var[j],
                                              its::IntExpressionFactory::createConstant(new_value));

      for (auto & ass : new_assigns)
      {
        ass.second = ass.second & assertion;
      }
      new_guard = new_guard & assertion;
    }

    std::vector<its::SyncAssignment::assign_t> action_assigns;
    for (const auto & ass : new_assigns)
    {
      if (! ass.second.equals (its::IntExpressionFactory::wrapBoolExpr (ass.first == 1)))
      {
        action_assigns.push_back (std::make_pair (ass.first, ass.second));
      }
    }
    its::GuardedAction action (_input2var[i].getName ());
    action.setGuard (new_guard);
    action.getAction ().add (its::SyncAssignment (action_assigns));
    gal.addTransition (action);
  }

//  its::SyncAssignment sync_assignment (assigns);
//  its::GuardedAction bigsync ("");
//  bigsync.getAction ().add (sync_assignment);
//  gal.addTransition (bigsync);
}

its::GAL *
load_aiger (const std::string &filename)
{
  // initialize aiger library
  struct aiger * res = aiger_init ();
  // load aiger from file
  const char * err = aiger_open_and_read_from_file (res, filename.c_str ());
  if (err)
  {
    std::cerr << "aiger model from file " << filename << "could not be read" << std::endl;
    std::cerr << "aiger reports error " << err << std::endl;
    std::cerr << "Aborting" << std::endl;
    exit(1);
  }
  its::GAL * gal = nullptr;
  // build a GAL from the aiger
  {
    aiger2GAL a2g(res);
    gal = a2g.toGAL ();
  }
  // delete aiger
  aiger_reset (res);

  return gal;
}
