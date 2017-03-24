// -*- C++ -*- (c) 2009, 2010 Milan Ceska & Petr Rockai
#include <its/divine/legacy/system/dve/dve_explicit_system.hh>
#include <string>
#include <math.h>
#include <map>
#include <vector>
#include <its/divine/wibble/string.h>

#include "its/gal/GAL.hh"


namespace dve2GAL {


struct ext_transition_t
{
    int synchronized;
    divine::dve_transition_t *first;
    divine::dve_transition_t *second; //only when first transition is synchronized;
    divine::dve_transition_t *property; // transition of property automaton
    std::vector<int> sv_read;
    std::vector<int> sv_write;
};

using divine::size_int_t;   
using divine::dve_transition_t;
using divine::dve_expression_t;
   
struct dve2GAL: public divine::dve_explicit_system_t
{
    bool many;
    int current_label;

    bool have_property;
  std::map<size_int_t,std::map<size_int_t,std::vector<ext_transition_t> > > transition_map;
  std::map<size_int_t,std::vector<dve_transition_t*> > channel_map;

    std::vector<dve_transition_t*> property_transitions;
    std::vector<dve_transition_t*>::iterator iter_property_transitions;

    std::map<size_int_t,std::vector<dve_transition_t*> >::iterator iter_channel_map;
    std::vector<dve_transition_t*>::iterator iter_transition_vector;
    std::map<size_int_t,std::vector<ext_transition_t> >::iterator iter_process_transition_map;
    std::map<size_int_t,std::map<size_int_t,std::vector<ext_transition_t> > >::iterator iter_transition_map;
    std::vector<ext_transition_t>::iterator iter_ext_transition_vector;


  std::string modelName;
  void setModelName(const std::string & name) {
    modelName = name ;
  }

    dve2GAL(divine::error_vector_t & evect=divine::gerr)
      : explicit_system_t(evect), dve_explicit_system_t(evect), current_label(0),modelName("divine")
    {}
    virtual ~dve2GAL() {}

  // Build an assignment from a dve expression (root should be T_ASSIGN)
  its::Assignment convertAssign (dve_expression_t & expr) ;
  // convert an expression representing an integer expression to IntExpression
  its::IntExpression convertInt (dve_expression_t & expr) ;
  // convert an expression representing a Boolean expression to BoolExpression
  its::BoolExpression convertBool (dve_expression_t & expr) ;
  


  /** These functions inherited from LTSmin analysis of divine specifications. */
    int  count_state_variables() const;
    void analyse_expression( divine::dve_expression_t & expr, ext_transition_t &ext_transition, std::vector<int> &dep );
    void output_dependency_comment( ext_transition_t &ext_transition );
    void mark_dependency ( divine::size_int_t gid, int type, int idx, std::vector<int> &dep);
    void analyse_transition_dependencies( ext_transition_t &ext_transition );
    void analyse_transition( divine::dve_transition_t * transition,
                             std::vector<ext_transition_t> &ext_transition_vector );
    void analyse();
  
  // if a proc has a single state, it will be simplified away from enabling conditions/no update in firing actions/no encoding of state variable in GAL
  bool procHasSingleState (int i) ;

    // a string for process name from index
    std::string process_name( int i ) ;

    // a string for channel name from index
    std::string channel_name( int i ) {
        return get_symbol_table()->get_channel( i )->get_name();
    }

    // a variable (procname.state) corresponding to process state
  its::Variable process_state( int i ) ;

    // Test whether the given process is in the given state
  its::BoolExpression in_state( int process, int state) ;

    // The variable representing the current number of items in a channel
  its::Variable channel_items( int i ) ;

  // The variable representing the state of field x of an element of a channel i
  // Note : this is the name of an array variable, it should be indexed by message number (see channel_item_at).
  its::Variable channel_item ( int i , int x) ;

  // An access to a channel item; i is channel index, pos is position of msg in channel, x is field id of struct in channel.
  its::IntExpression  channel_item_at( int i, int pos, int x ) ;
  its::IntExpression  channel_item_at( int i, const its::Variable & v, int x , int limit) ;
// {
//         return state + "." + channel_name( i ) + ".content[" + pos + "].x" + wibble::str::fmt( x );
//     }

  /** capacity (max size) of a channel.
   *  Used for :
   *  1. block transitions from writing/reading in channel if full/empty
   *  2. dimension channel as fixed length array
   */
  size_t channel_capacity( int i ) {
    return get_symbol_table()->get_channel( i )->get_channel_buffer_size();
  }

  /** Returns true if there is at least one divine state declared "commit".*/
  bool has_committed_states () ;

  /** Return the boolean expression corresponding to the enabling of a divien action. */
  its::BoolExpression transition_guard( ext_transition_t *et );

  /** Returns an assignment corresponding to an expression */
  its::Assignment assign( dve_expression_t &  left, dve_expression_t & right ) ;

  /** Compute transition effect and add to GuardedAssignment */
  void transition_effect( ext_transition_t * et, its::GuardedAction & ga );



  bool is_property( size_t i ) {
    return get_with_property() && i == get_property_gid();
  }

  /** Build files for FORCE oredering heuristic. */
  void buildHGraphNodes () const ;
  void buildHGraphEdge (ext_transition_t &ext_transition) const ;

  /** Create the variables and their initial values */
  void gen_initial_state(its::GAL & system);
  // adds a transient predicate to the GAL, based on naming of divine states.
  // any state prefixed by "trans_" is considered transient and not kep in th final
  // representation. It is up to the user to ensure that no zeno cycles exist.
  void gen_transient(its::GAL & gal);

  // Builds a new GAL with the semantics of the DVE model.
  its::GAL * convertFromDve ();

  void gen_transitions(its::GAL & gal);

  void gen_state_struct();
  void gen_state_info();
  void gen_transition_info();

};

}
