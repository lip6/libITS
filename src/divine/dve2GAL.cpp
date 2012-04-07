#include <iostream>

#include <divine/dve2GAL.hh>
#include <divine/common.h>



using wibble::str::fmt;
using namespace its;
using std::map;
using std::vector;
using std::pair;
using std::string;

using namespace divine;

namespace dve2GAL {



  static std::map< int, const char * > op = std::map< int, const char * >();
  static bool isOpInit = false;

  static std::map< int, const char * > & getOp() {
    if (! isOpInit) {
      isOpInit=true;
      op[ T_LT ] = "LT"; op[ T_LEQ ] = "LEQ";
      op[ T_EQ ] = "EQ"; op[ T_NEQ ] = "NEQ";
      op[ T_GT ] = "GT"; op[ T_GEQ ] = "GEQ";
      
      op[ T_PLUS ] = "PLUS"; op[ T_MINUS ] = "MINUS";
      op[ T_MULT ] = "MULT"; op[ T_DIV ] = "DIV"; op[ T_MOD ] = "MOD";
      
      op[ T_AND ] = "&"; op[ T_OR ] = "|"; op[ T_XOR ] = "^";
      op[ T_LSHIFT ] = "<<"; op[ T_RSHIFT ] = ">>";
      
      op[ T_BOOL_AND ] = "&&"; op[ T_BOOL_OR ] = "||";
      
      op[ T_ASSIGNMENT ] = "=";
    }
    return op;
  }

  Assignment dve2GAL::convertAssign (dve_expression_t & expr) {
    dve_symbol_table_t * parent_table = expr.get_symbol_table();
    if (!parent_table) gerr << "Writing expression: Symbol table not set" << thr();

    switch (expr.get_operator())
      {
      case T_ASSIGNMENT:
	return assign (*expr.left(), *expr.right() );
      default : 
	std::cerr << "Parse error from divine file, unexpected node \""<<  getOp()[ expr.get_operator() ] << "\" where Assignement  expected"<< std::endl;
	throw "Parse error in divine file.";
      }
  }

  IntExpression dve2GAL::convertInt (dve_expression_t & expr) {
    dve_symbol_table_t * parent_table = expr.get_symbol_table();
    if (!parent_table) gerr << "Writing expression: Symbol table not set" << thr();

    // variable name if resolving a variable
    std::stringstream vname;
    switch (expr.get_operator())
      {
      case T_ID:

	// ostr << "Variable(\"";
	if(!parent_table->get_variable(expr.get_ident_gid())->is_const())
	  {
	    if(parent_table->get_variable(expr.get_ident_gid())->get_process_gid() != NO_ID)
	      {
		vname << parent_table->get_process(parent_table->get_variable(expr.get_ident_gid())->
						  get_process_gid())->get_name(); //name of process
		vname <<".";
	      }
	  }
	vname << parent_table->get_variable(expr.get_ident_gid())->get_name();
	// ostr << "\")";
	return IntExpressionFactory::createVariable(Variable(vname.str()));
							  
      case T_FOREIGN_ID:
	vname << parent_table->get_process(parent_table->get_variable(expr.get_ident_gid())->
					  get_process_gid())->get_name(); //name of process
	vname<<"->";
	vname << parent_table->get_variable(expr.get_ident_gid())->get_name();
	return IntExpressionFactory::createVariable(Variable(vname.str()));

      case T_NAT:

	return IntExpressionFactory::createConstant(expr.get_value());

      case T_PARENTHESIS:
	return convertInt(*expr.left());

      case T_SQUARE_BRACKETS:
	// ostr << "IntExpressionFactory::createArrayAccess(Variable(\"";
	
	if(parent_table->get_variable(expr.get_ident_gid())->get_process_gid() != NO_ID)
	  {
	    vname << parent_table->get_process(parent_table->get_variable(expr.get_ident_gid())->
					      get_process_gid())->get_name(); //name of process
	    vname <<".";
	  }
	vname << parent_table->get_variable(expr.get_ident_gid())->get_name(); 

	return IntExpressionFactory::createArrayAccess(Variable(vname.str()), convertInt(*expr.left()));
      case T_FOREIGN_SQUARE_BRACKETS:
	vname << parent_table->get_process(parent_table->get_variable(expr.get_ident_gid())->
					  get_process_gid())->get_name(); //name of preocess
	vname<<"->";
	vname << parent_table->get_variable(expr.get_ident_gid())->get_name();
	
	return IntExpressionFactory::createArrayAccess(Variable(vname.str()), convertInt(*expr.left()));

      case T_PLUS:
    	  return IntExpressionFactory::createBinary ( PLUS , convertInt( *expr.left()), convertInt( *expr.right() ) );
      case T_MINUS:
          	  return IntExpressionFactory::createBinary ( MINUS , convertInt( *expr.left()), convertInt( *expr.right() ) );
      case T_MULT:
          	  return IntExpressionFactory::createBinary ( MULT , convertInt( *expr.left()), convertInt( *expr.right() ) );
      case T_DIV:
          	  return IntExpressionFactory::createBinary ( DIV , convertInt( *expr.left()), convertInt( *expr.right() ) );
      case T_MOD:
          	  return IntExpressionFactory::createBinary ( MOD , convertInt( *expr.left()), convertInt( *expr.right() ) );
      case T_UNARY_MINUS:
      	  return IntExpressionFactory::createBinary ( MINUS , IntExpressionFactory::createConstant(0), convertInt( *expr.right() ) );
      case T_LSHIFT: case T_RSHIFT:
      case T_TILDE:
    	  std::cerr << "Bit shift <<, >> and bitwise ~ are not supported. " << std::endl;
    	  // fall thru deliberately
      case T_DOT:
      default : 
    	  std::cerr << "Parse error from divine file, unexpected node \""<<  getOp()[ expr.get_operator() ] << "\" where IntExpression expected"<< std::endl;
    	  throw "Parse error in divine file.";
      }


  }

  BoolExpression dve2GAL::convertBool (dve_expression_t & expr) {
      dve_symbol_table_t * parent_table = expr.get_symbol_table();
      if (!parent_table) gerr << "Writing expression: Symbol table not set" << thr();

      // variable name if resolving a variable
      std::stringstream vname;
      switch (expr.get_operator())
      {
      case T_PARENTHESIS:
          return convertBool(*expr.left());
      case T_LT:
    	  return BoolExpressionFactory::createComparison ( LT, convertInt( *expr.left()), convertInt( *expr.right()));
      case T_LEQ:
    	  return BoolExpressionFactory::createComparison ( LEQ, convertInt( *expr.left()), convertInt( *expr.right()));
      case T_EQ:
    	  return BoolExpressionFactory::createComparison ( EQ, convertInt( *expr.left()), convertInt( *expr.right()));
      case T_NEQ:
    	  return BoolExpressionFactory::createComparison ( NEQ, convertInt( *expr.left()), convertInt( *expr.right()));
      case T_GT:
    	  return BoolExpressionFactory::createComparison ( GT, convertInt( *expr.left()), convertInt( *expr.right()));
      case T_GEQ:
    	  return BoolExpressionFactory::createComparison ( GEQ, convertInt( *expr.left()), convertInt( *expr.right()));
      case T_AND:
      case T_BOOL_AND:
          return BoolExpressionFactory::createBinary(AND,convertBool(*expr.left()), convertBool(*expr.right()));
      case T_OR:
      case T_BOOL_OR:
          return BoolExpressionFactory::createBinary(OR,convertBool(*expr.left()), convertBool(*expr.right()));
      case T_XOR:
      {
    	  // a ^ b = a&!b || !a&b
    	  BoolExpression l = convertBool(*expr.left());
    	  BoolExpression r = convertBool(*expr.right());
    	  return BoolExpressionFactory::createBinary( OR , l && (! r), (!l) && r );
      }
      case T_IMPLY:
      {
    	  // a->b = !a || b
    	  BoolExpression l = convertBool(*expr.left());
    	  BoolExpression r = convertBool(*expr.right());
    	  return BoolExpressionFactory::createBinary( OR , r , !l );
      }
      case T_BOOL_NOT:
    	  return BoolExpressionFactory::createNot( convertBool(*expr.right() ));

   	  default :
   		  std::cerr << "Parse error from divine file, unexpected node \""<<  getOp()[ expr.get_operator() ] << "\" where BoolExpression expected"<< std::endl;
   		  throw "Parse error in divine file.";

      }
  }


static std::vector<std::string> varnames = std::vector<std::string> ();


void dve2GAL::gen_initial_state(GAL & system)
{
  char buf[2048];
  //  append( "DDD initial_state =  " );
  string name = "UNINITIALIZED";
  string process_name = "UNINITIALIZED";
  bool global = true;
  int value=-1;
  for (size_int_t i=0; i!=state_creators_count; ++i)
    {
      switch (state_creators[i].type)
	{
	case state_creator_t::VARIABLE:
	  {
	    name = (global?"":process_name + ".") + 
	      get_symbol_table()->get_variable(state_creators[i].gid)->get_name();
	    if (state_creators[i].array_size)
	      {
		GAL::vals_t init;
		for(size_int_t j=0; j < state_creators[i].array_size; ++j)
		  {
		    value = (initial_values_counts[state_creators[i].gid]?
			     initial_values[state_creators[i].gid].all_values[j]:0);
		    init.push_back(value);

		    // TODO : this varnames code is not used anymore I think
		    sprintf(buf, "%s[%zu]", name.c_str(), j);
		    varnames.push_back(buf);

		  }
		system.addArray(name,init);
		continue;
	      }
	    else 
	      {
		value = (initial_values_counts[state_creators[i].gid]?
			 initial_values[state_creators[i].gid].all_value:0) ;
	    system.addVariable(Variable(name), value);
	    varnames.push_back(name);
		  }
	    break;
	  }
	case state_creator_t::PROCESS_STATE:
	  {
	    if (! procHasSingleState(state_creators[i].gid)) {
	      global = false;
	      name = get_symbol_table()->get_process(state_creators[i].gid)->get_name();
	      process_name = name;
	    
	      value = initial_states[state_creators[i].gid];
	      sprintf(buf, "%s.state", name.c_str());
	      system.addVariable(Variable(buf), value);
	      varnames.push_back(buf);
	    }
	  }
	  break;
	case state_creator_t::CHANNEL_BUFFER:
	  {
	    name = get_symbol_table()->get_channel(state_creators[i].gid)->get_name();
	    dve_symbol_t * symbol = get_symbol_table()->get_channel(state_creators[i].gid);
	    size_int_t item_count = symbol->get_channel_type_list_size();
	    size_int_t chan_size = symbol->get_channel_buffer_size();
	    sprintf(buf, "%s.number_of_items", name.c_str());
	    system.addVariable(Variable(buf), 0);
	    varnames.push_back(buf);
	    for(size_int_t l=0; l < chan_size; ++l)
	      {
		for (size_int_t j=0; j<item_count; ++j)
		  {
		    sprintf(buf, "%s[%zu].x%zu", name.c_str(), l,j);
		    system.addVariable(Variable(buf), 0);
		    varnames.push_back(buf);
		  }
	      }
	  }
	  break;
	default: gerr << "Unexpected error generating initial state" << thr();
	  break;
	};
    }
}





std::string dve2GAL::process_name( int i ) {
    return get_symbol_table()->get_process( i )->get_name();
}

Variable dve2GAL::process_state( int i ) {
  return  Variable( process_name( i ) + ".state") ;
}


BoolExpression dve2GAL::in_state( int process, int state) {
  if ( ! procHasSingleState(process) )
    return BoolExpressionFactory::createComparison ( EQ, process_state( process ) , IntExpressionFactory::createConstant( state ) );
  return BoolExpressionFactory::createConstant(true);
}


Variable dve2GAL::channel_items( int i ) {
    return Variable( channel_name( i ) + ".number_of_items" );
}



BoolExpression dve2GAL::transition_guard( ext_transition_t *et )
{
  BoolExpression guard = BoolExpressionFactory::createConstant(true);


  if (et->first->get_guard()) {
	  guard = guard && convertBool(*et->first->get_guard());
  }


  if( et->synchronized )
    {
	  guard = guard && in_state( et->second->get_process_gid(), et->second->get_state1_lid());
	  if (et->second->get_guard()) {
		  guard = guard && convertBool(*et->second->get_guard());
	  }
    }
  else
    {
      int chan = et->first->get_channel_gid();
      if(et->first->get_sync_mode() == SYNC_EXCLAIM_BUFFER)
    	  // check channel not full
    	  guard = guard &&
    	  	  ( IntExpressionFactory::createVariable(channel_items( chan )) != IntExpressionFactory::createConstant(channel_capacity( chan ) ));
      if(et->first->get_sync_mode() == SYNC_ASK_BUFFER)
    	  // check channel not empty
    	  guard = guard && ( IntExpressionFactory::createVariable(channel_items( chan )) != IntExpressionFactory::createConstant(0) );
    }
  return guard.eval();
}

its::Assignment dve2GAL::assign( dve_expression_t &  left, dve_expression_t & right ) 
{
  return Assignment(convertInt(left), convertInt(right));
}

its::Variable dve2GAL::channel_item ( int i , int x) {
  return Variable ( channel_name( i ) + ".content" + ".x" + wibble::str::fmt( x ));
}

its::IntExpression  dve2GAL::channel_item_at( int i, const IntExpression & pos, int x ) 
{
  return IntExpressionFactory::createArrayAccess(  channel_item( i,x ),   pos );
}

void dve2GAL::transition_effect( ext_transition_t *et, its::GuardedAction & ga )
{
  if(et->synchronized)
    {
      for(size_int_t s = 0;s < et->first->get_sync_expr_list_size();s++)
	ga.addAction( assign( *et->first->get_sync_expr_list_item(s), *et->second->get_sync_expr_list_item(s) ));
    }
  else
    {
      int chan = et->first->get_channel_gid();
      if(et->first->get_sync_mode() == SYNC_EXCLAIM_BUFFER)
        {
	  // When sending message, the set expr_list assigns to each field of the struct in channel, in order
	  for(size_int_t s = 0;s < et->first->get_sync_expr_list_size();s++)
            {
	      ga.addAction( Assignment ( channel_item_at( chan, channel_items( chan ), s ),
				    convertInt( *et->first->get_sync_expr_list_item( s ) ) ));
            }
	  // Add 1 to channel size
	  ga.addAction( Assignment ( channel_items( chan ), IntExpressionFactory::createBinary (PLUS, channel_items( chan ), 1) )); 
        }
      if(et->first->get_sync_mode() == SYNC_ASK_BUFFER)
        {
	  // When reading message, the set expr_list assigns to local variables the value of each field of the struct in channel[0], in order
	  for(size_int_t s = 0;s < et->first->get_sync_expr_list_size();s++)
	    ga.addAction( Assignment( convertInt( *et->first->get_sync_expr_list_item(s)), channel_item_at( chan, 0, s) ));
	  // Subtract 1 from channel size
	  ga.addAction( Assignment ( channel_items( chan ), IntExpressionFactory::createBinary (MINUS, channel_items( chan ), 1) )); 
	  
	  // Shift entries in channel down one index
	  // For every position in channel up to n-2
	  for(size_int_t i = 0 ; i < channel_capacity( chan ) - 1 ; i++) {
	    // For every field in the channel message struct
            for(size_int_t s = 0;s < et->first->get_sync_expr_list_size();s++)
	      {
		// copy value in message i+1 to i
		ga.addAction( Assignment( channel_item_at( chan, i, s), channel_item_at( chan, i+1, s) ));
	      }
	  }
	  // clear content of last channel cell
	  for(size_int_t s = 0;s < et->first->get_sync_expr_list_size();s++)
	    {
	      ga.addAction( Assignment( channel_item_at( chan, channel_capacity( chan ) - 1, s), 0 ));
	    }
	}
      
      //first transition effect
      // Update process state variable
      if (! procHasSingleState ( et->first->get_process_gid() ))
	  ga.addAction ( Assignment( process_state( et->first->get_process_gid()), et->first->get_state2_lid() ));
      
      // Effects on variables of main process
      for(size_int_t e = 0;e < et->first->get_effect_count();e++) {
	ga.addAction( convertAssign ( *et->first->get_effect(e) ));
      }

      // process Rendez-Vous
      if(et->synchronized) //second transiton effect
	{
	  // Update second process state
	  if (! procHasSingleState ( et->second->get_process_gid() ))
	      ga.addAction(Assignment( process_state( et->second->get_process_gid()),
				       et->second->get_state2_lid() )) ;
	  // actions of second process
	  for(size_int_t e = 0;e < et->second->get_effect_count();e++)
	    ga.addAction( convertAssign ( *et->second->get_effect(e) ));
	}

      if(have_property) //change of the property process state
        ga.addAction( Assignment ( process_state( et->property->get_process_gid()), et->property->get_state2_lid() ) );

    }
//     // show dependency information in the source
//     output_dependency_comment(*et);

}

void dve2GAL::gen_transient(GAL & gal)
{
  BoolExpression transient = BoolExpressionFactory::createConstant(false) ;
  int nbtransient = 0;
  // for every process
  for(size_int_t pid = 0; pid < get_process_count(); pid++) {
    dve_process_t* proc =  dynamic_cast<dve_process_t*>(get_process(pid));
    size_t nbstates = proc->get_state_count();
    // for each process state
    for(size_int_t pstate = 0; pstate < nbstates ; pstate++) {
      std::string stname =  get_symbol_table()->get_state(proc->get_state_gid(pstate))->get_name() ;
      // if it matches the regexp : trans_\w*
      if ( stname.substr(0,6) == "trans_" ) {
	// or this condition into the full predicate
	transient = transient ||  in_state(pid,pstate);
	nbtransient++;
      }
    }
  }
  if (nbtransient) {
    gal.setTransientPredicate (transient);
    // set the transient predicate for the full type.
    // line("// found "+ fmt(nbtransient) + " transient states");
  }
}

  bool dve2GAL::has_committed_states () {
    for(size_int_t i = 0; i < get_process_count(); i++)
      for(size_int_t j = 0; j < dynamic_cast<dve_process_t*>(get_process(i))->get_state_count(); j++)
	if(dynamic_cast<dve_process_t*>(get_process(i))->get_commited(j))
	  return true; 
    return false;
  }

bool dve2GAL::procHasSingleState (int i) 
{
  return dynamic_cast<dve_process_t*>(get_process(i))->get_state_count() == 1;
}

void dve2GAL::gen_transitions(GAL & gal)
{
  // Build a bool expression for testing the condition : "at least a process is in a commited state"
  BoolExpression committed_state = BoolExpressionFactory::createConstant(false);

  if (has_committed_states()) {

    // From Divine documentation, on the semantics of "commit" states:
    // Committed state of a system = state of a system, where at least one process is in a committed state. 
    // If the system is in a committed state, then only processes in committed states can transit to another state.
    // It means that the sequence of transitions beginning with a transition leading to a committed state 
    // and ending with a transition leading to a non-committed state cannot be interlaced with other
    // transitions leading from non-committed states.


    // foreach process
    for(size_int_t i = 0; i < get_process_count(); i++)
      // for each process state
      for(size_int_t j = 0; j < dynamic_cast<dve_process_t*>(get_process(i))->get_state_count(); j++)
	// if state is "commit"
	if(dynamic_cast<dve_process_t*>(get_process(i))->get_commited(j))
	  // Or into the condition
	  committed_state = committed_state ||  in_state( i, j ) ;
    // simplify
    committed_state = committed_state.eval();
  }

  int tnum = 0;
  // Foreach process
  for(size_int_t i = 0; i < this->get_process_count(); i++)
    {
      // Foreach source state of transitions
      if( transition_map.find(i) != transition_map.end() && !is_property( i ) )
	for(iter_process_transition_map = transition_map.find(i)->second.begin();
	    iter_process_transition_map != transition_map.find(i)->second.end();
	    iter_process_transition_map++)
	  {
	    
	    // Test location = source of transitions
	    BoolExpression stateGuard = in_state( i, iter_process_transition_map->first ) ;
	    
	    
	    // foreach transition with this state as source
	    for(iter_ext_transition_vector = iter_process_transition_map->second.begin();
		iter_ext_transition_vector != iter_process_transition_map->second.end();
		iter_ext_transition_vector++)
	      {
		// First compute additional enabling conditions
		BoolExpression guard = stateGuard;
		
		// If this state is NOT commited
		if(! dynamic_cast<dve_process_t*>(get_process(i))->get_commited(iter_process_transition_map->first)) 
		  {
		    // Source is not committed, due to priority, we can only fire 
		    // if we ensure no urgent transitions are available.
		    // Thus, and the "Not In commited state" to guard of transition
		    guard = guard && (! committed_state);
		  } else if (
		    // From Divine documentation :
		    // Synchronization between processes in committed states and non-committed states is ignored. 
		    // This test implements this condition. 
		    // The remark in czech translates to "what about sync with commit states of the property ???"
		    // I don't really care in our setting where the property will be fed separately to libits tools.
		  
		    // !! jak je to s property synchronizaci v comitted stavech !!
		     // it is a synchronization of process
		       iter_ext_transition_vector->synchronized 
		       &&
			! dynamic_cast<dve_process_t*> (
				get_process
				(
				 // Source state for transition of second process of the synchronization
				 iter_ext_transition_vector->second->get_process_gid()
				 )
				// is not "commited"
				)->get_commited(iter_ext_transition_vector->second->get_state1_lid()) 
		       ) {
		      // Synchronization between processes in committed states and non-committed states is ignored. 
		      // NOTE : for me this should be a syntax error. Print a warning ?
		      continue;
		}
		

		std::string name = "t" + wibble::str::fmt(tnum++);
		std::string label = 
		  process_name(i) + "." + get_symbol_table()->get_state(iter_process_transition_map->first)->get_name() // + fmt(iter_process_transition_map->first) +"_" + process_name(i) 
		  + "-" +  get_symbol_table()->get_state(iter_ext_transition_vector->first->get_state2_gid())->get_name() ;
		
		GuardedAction ga(name);
		//	    ga.setLabel(label);
		
		// Now add final transition guard
		guard = guard && transition_guard( &*iter_ext_transition_vector );
		
		ga.setGuard(guard.eval());

		// set transition effect
		transition_effect( &*iter_ext_transition_vector, ga );
		    
		gal.addTransition(ga);

	      } // foreach transition
	  } // foreach process state
    } // foreach process

}

GAL * dve2GAL::convertFromDve()
{
  GAL * gal = new GAL (modelName);
  gen_initial_state(*gal);
  gen_transitions(*gal);

  gen_transient(*gal);

  return gal;
}







/*********** Code below is courtesy of Twente team of LTSmin, and builds some structures that facilitate the definition of the semantics above. */


void dve2GAL::mark_dependency( size_int_t gid, int type, int idx, std::vector<int> &dep )
{
    size_int_t size = 0;
    bool mark = false;
    for (size_int_t i=0; i!=state_creators_count; ++i)
    {
        mark = state_creators[i].gid == gid && type == state_creators[i].type;
        switch (state_creators[i].type)
        {
            case state_creator_t::VARIABLE:
            {
                if (state_creators[i].array_size)
                {
                     for(size_t j=0; j<state_creators[i].array_size; j++)
                     {
		       if (mark && (idx == -1 || (size_t)idx == j)) dep[size]=1;
                        size++;
                     }
                }
                else
                {
                    if (mark) { dep[size]=1; }
                    size++;
                }
            }
            break;
            case state_creator_t::PROCESS_STATE:
            {
                if (mark) { dep[size]=1; }
                size++;
            }
            break;
            case state_creator_t::CHANNEL_BUFFER:
            {
                // mark number of items
                if (mark) dep[size]=1;
                size++;

                // mark channel
                dve_symbol_t * symbol =
                  get_symbol_table()->get_channel(state_creators[i].gid);
                size_int_t item_count = symbol->get_channel_type_list_size();
                size_int_t chan_size = symbol->get_channel_buffer_size();
                for(size_int_t i=0; i < chan_size; ++i) {
                    for (size_int_t j=0; j<item_count; ++j) {
                        if (mark) dep[size]=1;
                        size++;
                    }
                }
            }
            break;
            default: gerr << "Unexpected error while marking dependency" << thr();
                break;
        };
    }
}

int dve2GAL::count_state_variables() const
{
    size_int_t size = 0;
    for (size_int_t i=0; i!=state_creators_count; ++i)
    {
        switch (state_creators[i].type)
        {
            case state_creator_t::VARIABLE:
                size += (state_creators[i].array_size)?state_creators[i].array_size:1;
                break;
            case state_creator_t::PROCESS_STATE:
                size++;
                break;
            case state_creator_t::CHANNEL_BUFFER:
            {
                dve_symbol_t * symbol =
                  get_symbol_table()->get_channel(state_creators[i].gid);
                size_int_t item_count = symbol->get_channel_type_list_size();
                size_int_t chan_size = symbol->get_channel_buffer_size();
                size += (chan_size * item_count) + 1;
                break;
            }
            default: gerr << "Unexpected error while counting length of state" << thr();
                break;
        };
    }
    return size;
}


void dve2GAL::analyse_expression( dve_expression_t & expr, ext_transition_t &ext_transition, std::vector<int> &dep)
{
    dve_symbol_table_t * parent_table = expr.get_symbol_table();
    if (!parent_table) gerr << "Writing expression: Symbol table not set" << thr();
    switch (expr.get_operator())
    {
        case T_ID:
            //if (!(parent_table->get_variable(expr.get_ident_gid())->is_const())) // should this be here?
            mark_dependency(expr.get_ident_gid(), state_creator_t::VARIABLE, -1, dep);
            break;
        case T_FOREIGN_ID:
            mark_dependency(expr.get_ident_gid(), state_creator_t::VARIABLE, -1, dep);
            break;
        case T_NAT:
            break;
        case T_PARENTHESIS:
            analyse_expression(*expr.left(), ext_transition, dep);
            break;
        case T_FOREIGN_SQUARE_BRACKETS:
        case T_SQUARE_BRACKETS:
            if ((*expr.left()).get_operator() == T_NAT)
            {
                mark_dependency(expr.get_ident_gid(), state_creator_t::VARIABLE, (*expr.left()).get_value(), dep);
            } else {
                // some expression, mark all & continue analysis
                mark_dependency(expr.get_ident_gid(), state_creator_t::VARIABLE, -1, dep);
                analyse_expression(*expr.left(), ext_transition, dep);
            }
            break;
        case T_LT: case T_LEQ: case T_EQ: case T_NEQ: case T_GT: case T_GEQ:
        case T_PLUS: case T_MINUS: case T_MULT: case T_DIV: case T_MOD:
        case T_AND: case T_OR: case T_XOR: case T_LSHIFT: case T_RSHIFT:
        case T_BOOL_AND: case T_BOOL_OR:
            analyse_expression( *expr.left(), ext_transition, ext_transition.sv_read );
            analyse_expression( *expr.right(), ext_transition, ext_transition.sv_read );
            break;
        case T_ASSIGNMENT:
            analyse_expression( *expr.left(), ext_transition, ext_transition.sv_write );
            analyse_expression( *expr.right(), ext_transition, ext_transition.sv_read );
            break;
        case T_DOT:
            // dot addes an explicit == (see code), thus must be read
            mark_dependency(parent_table->get_state(expr.get_ident_gid())->get_process_gid(),
                            state_creator_t::PROCESS_STATE, -1, ext_transition.sv_read);
            break;
        case T_IMPLY:
            analyse_expression( *expr.left(), ext_transition, dep );
            analyse_expression( *expr.right(), ext_transition, dep );
            break;
        case T_UNARY_MINUS:
            analyse_expression( *expr.right(), ext_transition, dep);
            break;
        case T_TILDE:
            analyse_expression( *expr.right(), ext_transition, dep );
            break;
        case T_BOOL_NOT:
            analyse_expression( *expr.right(), ext_transition, dep );
            break;
        default:
            gerr << "Problem in expression - unknown operator"
                 << " number " << expr.get_operator() << psh();
    }
}

void dve2GAL::analyse_transition_dependencies( ext_transition_t &ext_transition )
{

    // initialize read/write dependency vector
    int count = count_state_variables();
    ext_transition.sv_read.resize(count);
    ext_transition.sv_write.resize(count);

    // guard

    // mark process as read
    mark_dependency(ext_transition.first->get_process_gid(),
                    state_creator_t::PROCESS_STATE, -1, ext_transition.sv_read);

    if (ext_transition.first->get_guard())
    analyse_expression( *(ext_transition.first->get_guard()), ext_transition,
                        ext_transition.sv_read);

    if (ext_transition.synchronized)
    {
        // mark process as read
        mark_dependency(ext_transition.second->get_process_gid(),
                        state_creator_t::PROCESS_STATE, -1, ext_transition.sv_read);

        // analyse ext_transition->second->get_guard
        if (ext_transition.second->get_guard())
            analyse_expression( *(ext_transition.second->get_guard()), ext_transition,
            ext_transition.sv_read);
    } else {
        int sm = ext_transition.first->get_sync_mode();
        if (sm == SYNC_EXCLAIM_BUFFER || sm == SYNC_ASK_BUFFER)
        {
            mark_dependency(ext_transition.first->get_channel_gid(),
                            state_creator_t::CHANNEL_BUFFER, -1, ext_transition.sv_read);
        }
    }

    if (have_property) // doesn't work for ltsmin, but mark anyway
    {
        // mark process as read/write?
        mark_dependency(ext_transition.property->get_process_gid(),
                        state_creator_t::PROCESS_STATE, -1, ext_transition.sv_write);

        // analyse ext_transition->property->get_guard
        if (ext_transition.property->get_guard())
            analyse_expression( *(ext_transition.property->get_guard()), ext_transition,
            ext_transition.sv_read);
    }

    // effect
    // todo: synchronized & channel effects...
    if (ext_transition.synchronized)
    {
        for(size_int_t s = 0;s < ext_transition.first->get_sync_expr_list_size();s++)
        {
            // todo: test  :)
            analyse_expression( *(ext_transition.first->get_sync_expr_list_item(s)), ext_transition,
                                ext_transition.sv_write);
            analyse_expression( *(ext_transition.second->get_sync_expr_list_item(s)), ext_transition,
                                ext_transition.sv_read);
        }
    } else {
        int sm = ext_transition.first->get_sync_mode();
        if (sm == SYNC_EXCLAIM_BUFFER)
        {
            // mark entire channel
            mark_dependency(ext_transition.first->get_channel_gid(),
                            state_creator_t::CHANNEL_BUFFER, -1, ext_transition.sv_write);
            // mark sync expressions
            for(size_int_t s = 0;s < ext_transition.first->get_sync_expr_list_size();s++)
            {
                analyse_expression( *(ext_transition.first->get_sync_expr_list_item(s)), ext_transition,
                                    ext_transition.sv_read);
            }
        }
        if (sm == SYNC_ASK_BUFFER)
        {
            // mark entire channel
            mark_dependency(ext_transition.first->get_channel_gid(),
                            state_creator_t::CHANNEL_BUFFER, -1, ext_transition.sv_read);
            mark_dependency(ext_transition.first->get_channel_gid(),
                            state_creator_t::CHANNEL_BUFFER, -1, ext_transition.sv_write);
            // mark sync expressions
            for(size_int_t s = 0;s < ext_transition.first->get_sync_expr_list_size();s++)
            {
                analyse_expression( *(ext_transition.first->get_sync_expr_list_item(s)), ext_transition,
                                    ext_transition.sv_write);
            }

        }

    }

    // mark process as read (write is probably in transition effect)
    mark_dependency(ext_transition.first->get_process_gid(),
                    state_creator_t::PROCESS_STATE, -1, ext_transition.sv_write);

    // analyse ext_transition->first
    for(size_int_t e = 0;e < ext_transition.first->get_effect_count();e++)
        analyse_expression( *(ext_transition.first->get_effect(e)), ext_transition,
        ext_transition.sv_read);

    // analyse ext_transition->second?
    if (ext_transition.synchronized)
    {
        // mark process as read (write is probably in transition effect)
        mark_dependency(ext_transition.second->get_process_gid(),
                        state_creator_t::PROCESS_STATE, -1, ext_transition.sv_write);

        // analyse ext_transition->second
        for(size_int_t e = 0;e < ext_transition.second->get_effect_count();e++)
            analyse_expression( *(ext_transition.second->get_effect(e)), ext_transition,
            ext_transition.sv_read);
    }

    if (have_property) // doesn't work for ltsmin, but mark anyway
    {
        // mark process as read/write?
        mark_dependency(ext_transition.property->get_process_gid(),
                        state_creator_t::PROCESS_STATE, -1, ext_transition.sv_write);
    }
}

void dve2GAL::analyse_transition(
    dve_transition_t * transition,
    vector<ext_transition_t> &ext_transition_vector )
{
    if(!transition->is_sync_ask())
    {
        // transition not of type SYNC_ASK
        if(!have_property)
        {
            // no properties, just add to ext_transition vector
            ext_transition_t ext_transition;
            ext_transition.synchronized = false;
            ext_transition.first = transition;
            analyse_transition_dependencies(ext_transition);
            ext_transition_vector.push_back(ext_transition);
        }
        else
        {
            // this transition is not a property, but there are properties
            // forall properties, add this transition to ext_transition_vector
            for(iter_property_transitions = property_transitions.begin();
                iter_property_transitions != property_transitions.end();
                iter_property_transitions++)
            {
                ext_transition_t ext_transition;
                ext_transition.synchronized = false;
                ext_transition.first = transition;
                ext_transition.property = (*iter_property_transitions);
                // ltsmin doesn't work with properties, but analyse anyway
                analyse_transition_dependencies(ext_transition);
                ext_transition_vector.push_back(ext_transition);
            }
        }
    }
    else
    {
        // transition of type SYNC_ASK
        iter_channel_map = channel_map.find(transition->get_channel_gid());
        if(iter_channel_map != channel_map.end())
        {
            // channel of this transition is found
            // (strange test, no else part for if statement)
            // assume: channel should always be present
            // forall transitions that also use this channel, add to ext_transitions
            for(iter_transition_vector  = iter_channel_map->second.begin();
                iter_transition_vector != iter_channel_map->second.end();
                iter_transition_vector++)
            {
                if (transition->get_process_gid() != (*iter_transition_vector)->get_process_gid() ) //not synchronize with yourself
                {
                    if(!have_property)
                    {
                        // system has no properties, so add only once without property
                        ext_transition_t ext_transition;
                        ext_transition.synchronized = true;
                        ext_transition.first = transition;
                        ext_transition.second = (*iter_transition_vector);
                        analyse_transition_dependencies(ext_transition);
                        ext_transition_vector.push_back(ext_transition);
                    }
                    else
                    {
                        // system has properties, so forall properties, add the combination if this transition,
                        // the transition that also uses this channel and the property
                        for(iter_property_transitions = property_transitions.begin();
                            iter_property_transitions != property_transitions.end();
                            iter_property_transitions++)
                        {
                            ext_transition_t ext_transition;
                            ext_transition.synchronized = true;
                            ext_transition.first = transition;
                            ext_transition.second = (*iter_transition_vector);
                            ext_transition.property = (*iter_property_transitions);
                            // ltsmin does't work with properties, but analyse anyway
                            analyse_transition_dependencies(ext_transition);
                            ext_transition_vector.push_back(ext_transition);
                        }
                    }
                }
            }
        }
    }
}

void dve2GAL::analyse()
{
    dve_transition_t * transition;
    have_property = get_with_property();

    // obtain transition with synchronization of the type SYNC_EXCLAIM and property transitions
    for(size_int_t i = 0; i < get_trans_count(); i++)
    {
        transition = dynamic_cast<dve_transition_t*>(get_transition(i));
        if(transition->is_sync_exclaim())
        {
            iter_channel_map = channel_map.find(transition->get_channel_gid());
            if(iter_channel_map == channel_map.end()) //new channel
            {
                std::vector<dve_transition_t*> transition_vector;
                transition_vector.push_back(transition);
                channel_map.insert(std::pair<size_int_t,std::vector<dve_transition_t*> >(
                                       transition->get_channel_gid(),transition_vector));
            }
            else{
                iter_channel_map->second.push_back(transition);
            }
        }

        if( is_property( transition->get_process_gid() ) )
            property_transitions.push_back(transition);
    }

    // obtain map of transitions
    for(size_int_t i = 0; i < get_trans_count(); i++)
    {
        transition = dynamic_cast<dve_transition_t*>(get_transition(i));
        if(!transition->is_sync_exclaim() && !is_property( transition->get_process_gid() ) )
        {
            // not syncronized sender without buffer and not a property transition
            iter_transition_map = transition_map.find(transition->get_process_gid());

            //new process it means that new state in process is also new
            if( iter_transition_map == transition_map.end())
            {
                // new process, add to transition map
	        map<size_int_t,vector<ext_transition_t> >  process_transition_map;
                vector<ext_transition_t> ext_transition_vector;

                analyse_transition( transition, ext_transition_vector );

                // for this process state, add the ext transitions
                process_transition_map.insert(pair<size_int_t,vector<ext_transition_t> >(
                                                  transition->get_state1_lid(),ext_transition_vector));
                // then add this vector to the transition map for this process
                transition_map.insert(pair<size_int_t,map<size_int_t,vector<ext_transition_t> > >(
                                          transition->get_process_gid(),process_transition_map));
            } else {
                // existing process, find process_transition_map
                iter_process_transition_map =
                    iter_transition_map->second.find(transition->get_state1_lid());

                //new state in current process
                if( iter_process_transition_map == iter_transition_map->second.end())
                {
                    vector<ext_transition_t> ext_transition_vector;
                    analyse_transition( transition, ext_transition_vector );

                    // and reinsert result
                    iter_transition_map->second.insert(
                        pair<size_int_t,vector<ext_transition_t> >(
                            transition->get_state1_lid(),ext_transition_vector) );
                } else analyse_transition( transition, iter_process_transition_map->second );
            }
        }
    }
}


/************** This code used to handle FORCE compatible hypergraph output */

void dve2GAL::buildHGraphNodes () const {
  FILE * vertex_out = fopen("out.nodes","w");
  FILE * vnames_out = fopen("out.vars","w");
  fprintf(vertex_out, "UCLA nodes   1.0\n# Generated by dvecompile libDDD\nNumNodes : %zu\nNumTerminals : 0\n",varnames.size());
  int count = 0;
  for (std::vector<string>::const_iterator it = varnames.begin() ; it != varnames.end() ; ++it ) {
    fprintf(vnames_out,"%s\n",it->c_str());
    fprintf(vertex_out,"     v%d 1 1\n",++count);
  }
  fclose(vertex_out);
  fclose(vnames_out);
  FILE * aux_out = fopen("out.aux","w");
  fprintf (aux_out, "HGraph : out.nodes out.nets\n");
  fclose(aux_out);
}


static FILE * nets_out = NULL;
long int pinspos=0;
int totalpins= 0;

void dve2GAL::buildHGraphEdge ( ext_transition_t &ext_transition ) const
{
  size_t count = count_state_variables();

  int nbEdges = 0;

  // Count variables impacted (read or write)
  for(size_int_t i = 0; i < count; i++)
    {
      if (ext_transition.sv_read[i])
	nbEdges++;
      if (ext_transition.sv_write[i])
	nbEdges++;
    }
  
  totalpins += nbEdges;

  // print header of file for first transition
  if (nets_out == NULL) {
    nets_out = fopen("out.nets","w");
    fprintf(nets_out,"UCLA nets   1.0\n# Generated by dvecompile\n# NumNets : %zu\n  NumPins : ", get_trans_count());
    pinspos= ftell(nets_out);
    // allow some space to write the number of pins into
    fprintf(nets_out,"         \n");
  }

  /** Print current edge into HGraph output file */
  fprintf(nets_out, "NetDegree : %d\n",nbEdges);

  for(size_int_t i = 0; i < count; i++)
    {
      // read
      if (ext_transition.sv_read[i]) {
	fprintf(nets_out, "v%zu I\n", i);
      }
      // write
      if (ext_transition.sv_write[i]) {
	fprintf(nets_out, "v%zu O\n", i);
      }
    }


// Aftyer computing nbtransitions   
    fseek(nets_out,pinspos,SEEK_SET);
    fprintf(nets_out, "%d",totalpins);
    fclose(nets_out);
    nets_out = NULL;
    
}

/*************FORCE OUTPUT END *******/

}
