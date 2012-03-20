// -*- C++ -*- (c) 2009, 2010 Milan Ceska & Petr Rockai
#include <divine/legacy/system/dve/dve_explicit_system.hh>
#include <string>
#include <math.h>
#include <map>
#include <vector>
#include <divine/wibble/string.h>

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

  /** These functions inherited from LTSmin analysis of divine specifications. */
    int  count_state_variables();
    void analyse_expression( divine::dve_expression_t & expr, ext_transition_t &ext_transition, std::vector<int> &dep );
    void output_dependency_comment( ext_transition_t &ext_transition );
    void mark_dependency ( divine::size_int_t gid, int type, int idx, std::vector<int> &dep);
    void analyse_transition_dependencies( ext_transition_t &ext_transition );
    void analyse_transition( divine::dve_transition_t * transition,
                             std::vector<ext_transition_t> &ext_transition_vector );
    void analyse();


    // a string for process name from index
    std::string process_name( int i ) ;

    // a variable corresponding to process state
    Variable process_state( int i, std::string state ) ;

    // Test whether the given process is in the given state
    BoolExpression in_state( int process, int state) ;

    // The variable representing the current number of itmes in a channel
    Variable channel_items( int i ) ;


    std::string channel_item_at( int i, std::string pos, int x, std::string state ) {
        return state + "." + channel_name( i ) + ".content[" + pos + "].x" + wibble::str::fmt( x ) + (ltsmin?".var":"");
    }

    int channel_capacity( int i ) {
        return get_symbol_table()->get_channel( i )->get_channel_buffer_size();
    }

    void transition_guard( ext_transition_t *, std::string );
    void transition_effect( ext_transition_t *, std::string, std::string );

    bool is_property( size_t i ) {
        return get_with_property() && i == get_property_gid();
    }

    std::string cexpr( dve_expression_t &expr, std::string state );
    void print_cexpr( dve_expression_t &expr, std::string state )
    {
        line( cexpr( expr, state ) + ";" );
    }

    void new_label() {
        if (many)
            return;
        append( std::string( "l" ) + wibble::str::fmt( current_label ) + ": " );
        current_label ++;
    }

    void block_begin() { line( "{" ); indent(); }
    void block_end() { deindent(); line( "}" ); }

    std::string in_state( int process, int state, std::string from_state ) {
      return "BoolExpressionFactory::createComparison ( EQ," + process_state( process, from_state ) + " , IntExpressionFactory::createConstant(" + wibble::str::fmt( state ) +"))";
    }

    void setOutput( std::ostream &o ) {
        m_output = &o;
    }

    void yield_state();
    void new_output_state();

    void gen_successors();
    void gen_ltsmin_successors();
    void gen_is_accepting();
    void gen_header();
  void gen_transient();
  void gen_tail();
    void gen_state_struct();
    void gen_initial_state();
    void gen_state_info();
    void gen_transition_info();

    void print_generator();
};

}
