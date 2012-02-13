#include <divine/dvecompile.h>
#include <divine/common.h>

using namespace std;
using wibble::str::fmt;

namespace divine {



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


void dve_compiler::write_C(dve_expression_t & expr, std::ostream & ostr, std::string state_name)
{

    dve_symbol_table_t * parent_table = expr.get_symbol_table();
    if (!parent_table) gerr << "Writing expression: Symbol table not set" << thr();
    switch (expr.get_operator())
    {
        case T_ID:
	  ostr << "Variable(\"";
            if(!parent_table->get_variable(expr.get_ident_gid())->is_const())
            {
	      if(parent_table->get_variable(expr.get_ident_gid())->get_process_gid() != NO_ID)
                {
		  ostr << parent_table->get_process(parent_table->get_variable(expr.get_ident_gid())->
						    get_process_gid())->get_name(); //name of process
		  ostr<<".";
                }
            }
            ostr << parent_table->get_variable(expr.get_ident_gid())->get_name();
	  ostr << "\")";
            break;
        case T_FOREIGN_ID:
            ostr << parent_table->get_process(parent_table->get_variable(expr.get_ident_gid())->
                                              get_process_gid())->get_name(); //name of process
            ostr<<"->";
            ostr << parent_table->get_variable(expr.get_ident_gid())->get_name();
            break;
        case T_NAT:
	  ostr << "IntExpressionFactory::createConstant(" << expr.get_value() << ")";
	  break;
        case T_PARENTHESIS:
            ostr << "(";
            write_C(*expr.left(), ostr, state_name);
            ostr << ")";
            break;
        case T_SQUARE_BRACKETS:
	  ostr << "IntExpressionFactory::createArrayAccess(Variable(\"";
	  if(parent_table->get_variable(expr.get_ident_gid())->get_process_gid() != NO_ID)
            {
	      ostr << parent_table->get_process(parent_table->get_variable(expr.get_ident_gid())->
						get_process_gid())->get_name(); //name of process
	      ostr<<".";
            }
	  ostr << parent_table->get_variable(expr.get_ident_gid())->get_name(); 
	  ostr << "\")," ;
	  write_C(*expr.left(), ostr, state_name);
//	  ostr<<expr.left()->get_value() ;
	  ostr << ")";
	  break;
        case T_FOREIGN_SQUARE_BRACKETS:
            ostr << parent_table->get_process(parent_table->get_variable(expr.get_ident_gid())->
                                              get_process_gid())->get_name(); //name of preocess
            ostr<<"->";
            ostr << parent_table->get_variable(expr.get_ident_gid())->get_name();
            ostr<<"["; write_C(*expr.left(), ostr, state_name); ostr<<"]";
            break;

        case T_LT: case T_LEQ: case T_EQ: case T_NEQ: case T_GT: case T_GEQ:
	  ostr << "BoolExpressionFactory::createComparison ( " <<  getOp()[ expr.get_operator() ] <<", ";
	  write_C( *expr.left(), ostr, state_name );
	  ostr << " , " ;
	  write_C( *expr.right(), ostr, state_name );
	  ostr << ")";
	  break;
        case T_PLUS: case T_MINUS: case T_MULT: case T_DIV: case T_MOD:
	  ostr << "IntExpressionFactory::createBinary ( " <<  getOp()[ expr.get_operator() ] <<", ";
	  write_C( *expr.left(), ostr, state_name );
	  ostr << " , " ;
	  write_C( *expr.right(), ostr, state_name );
	  ostr << ")";
	  break;
        case T_AND: case T_OR: case T_XOR: case T_LSHIFT: case T_RSHIFT:
        case T_BOOL_AND: case T_BOOL_OR: 
            write_C(*expr.left(), ostr, state_name);
            ostr<< " " << getOp()[ expr.get_operator() ] << "  ";
            write_C(*expr.right(), ostr, state_name);
            break;
	  
    case T_ASSIGNMENT:

            ostr << "ga.addAction(Assignment(" ;
// 	    if (false && expr.left()->get_operator() == T_SQUARE_BRACKETS) {
// 	      ostr << "Variable(\"";
// 	      if(parent_table->get_variable(expr.get_ident_gid())->get_process_gid() != NO_ID)
// 		{
// 		  ostr << parent_table->get_process(parent_table->get_variable(expr.get_ident_gid())->
// 						    get_process_gid())->get_name(); //name of process
// 		  ostr<<".";
// 		}
// 	      ostr << parent_table->get_variable(expr.left()->get_ident_gid())->get_name(); 
// //	      ostr<<"[" ; write_C(*expr.left()->left(), ostr, state_name); ostr<<"]";
// 	      ostr<<"[" << expr.left()->left()->get_value() << "]";
// 	      ostr << "\")," ;
// 	    } else {
	      write_C( *expr.left(), ostr, state_name );
	      ostr << ", " ;
//	    }
	    write_C( *expr.right(), ostr, state_name );
	    ostr << "))";
	    break;

        case T_DOT:
            ostr<<state_name<<".";
            ostr<<parent_table->get_process(parent_table->get_state(expr.get_ident_gid())->
                                            get_process_gid())->get_name(); ostr<<".state"<<(ltsmin?".var":"")<<" == ";
            ostr<<parent_table->get_state(expr.get_ident_gid())->get_lid();
            break;

        case T_IMPLY:
            write_C(*expr.left(), ostr, state_name);
            ostr<<" -> "; // FIXME this looks wrong, -> in C is dereference
            write_C(*expr.right(), ostr, state_name);
            break;
        case T_UNARY_MINUS:
            ostr<<"-";
            write_C(*expr.right(), ostr, state_name);
            break;
        case T_TILDE:
            ostr<<"~";
            write_C(*expr.right(), ostr, state_name);
            break;
        case T_BOOL_NOT:
            ostr<<" ! (";
            write_C(*expr.right(), ostr, state_name);
            ostr<< " )";
            break;
        default:
            gerr << "Problem in expression - unknown operator"
                 << " number " << expr.get_operator() << psh();
    }
}

std::string dve_compiler::cexpr( dve_expression_t & expr, std::string state )
{
    std::stringstream str;
    str << "(";
    write_C( expr, str, state.c_str() );
    str << ")";
    return str.str();
}

namespace divine {
extern const char *pool_h_str;
extern const char *circular_h_str;
extern const char *blob_h_str;
}

void dve_compiler::gen_header()
{
    line( "#include <stdio.h>" );
    line( "#include <string.h>" );
    line( "#include <stdint.h>" );
    line();
    line();
    line( "#include \"gal/GAL.hh\"");
    line();

    line("namespace its");
    block_begin();

    line("class Divine : public GAL ");
    block_begin();
    line("public:");
    line("Divine():GAL(\"" +modelName  + "\") ");
    block_begin();

}

void dve_compiler::gen_tail () {
  block_end(); // constructor body

  block_end(); // class body
  line(";");

  block_end(); // namespace its

  line("extern \"C\" its::GAL * createGAL()");
  block_begin();
  line("return new its::Divine();");
  block_end();
}




void dve_compiler::gen_state_struct()
{
}


static std::vector<std::string> varnames = std::vector<std::string> ();


void dve_compiler::gen_initial_state()
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
		for(size_int_t j=0; j < state_creators[i].array_size; ++j)
		  {
		    value = (initial_values_counts[state_creators[i].gid]?
			     initial_values[state_creators[i].gid].all_values[j]:0);
		    sprintf(buf, "    addVariable(Variable(\"%s[%zu]\"),%d);", name.c_str(), j, value);
		    line(buf);
		    sprintf(buf, "%s[%zu]",name.c_str(), j);
		    varnames.push_back(buf);
		  }
		continue;
	      }
	    else 
	      {
		value = (initial_values_counts[state_creators[i].gid]?
			 initial_values[state_creators[i].gid].all_value:0) ;
		sprintf(buf, "    addVariable(Variable(\"%s\"),%d);", name.c_str(), value);
		varnames.push_back(name);
		line(buf);
	      }
	    break;
	  }
	case state_creator_t::PROCESS_STATE:
	  {
	    global = false;
	    name = get_symbol_table()->get_process(state_creators[i].gid)->get_name();
	    process_name = name;
	    
	    value = initial_states[state_creators[i].gid];
	    sprintf(buf,"    addVariable(Variable(\"%s.state\"),%d);", name.c_str(), value);
	    line(buf);
	    varnames.push_back(name+".state");
	  }
	  break;
	case state_creator_t::CHANNEL_BUFFER:
	  {
	    name = get_symbol_table()->get_channel(state_creators[i].gid)->get_name();
	    dve_symbol_t * symbol = get_symbol_table()->get_channel(state_creators[i].gid);
	    size_int_t item_count = symbol->get_channel_type_list_size();
	    size_int_t chan_size = symbol->get_channel_buffer_size();
	    sprintf(buf, "addVariable(Variable(\"%s.number_of_items\"),%d);", name.c_str(), 0);	    
	    line(buf);
	    varnames.push_back(name+".number_of_items");
	    for(size_int_t l=0; l < chan_size; ++l)
	      {
		for (size_int_t j=0; j<item_count; ++j)
		  {
		    sprintf(buf, "addVariable(Variable(\"%s[%zu].x%zu\"),%d);", name.c_str(), l,j,0);
		    line(buf);
		    sprintf(buf, "%s[%zu].x%zu", name.c_str(), l,j);
		    varnames.push_back(buf);
		  }
	      }
	  }
	  break;
	default: gerr << "Unexpected error generating initial state" << thr();
	  break;
	};
    }
  line();

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

void dve_compiler::output_dependency_comment( ext_transition_t &ext_transition )
{
  size_t count = count_state_variables();
    char buf[1024];

    int nbEdges = 0;

    append("// read : " );
    for(size_int_t i = 0; i < count; i++)
    {
      if (ext_transition.sv_read[i])
	nbEdges++;
      sprintf(buf, "%s%d", ((i==0)?"":","), ext_transition.sv_read[i]);
      append(buf);
    }
    line();
    fflush(nets_out);

    append("// write: " );
    for(size_int_t i = 0; i < count; i++)
    {
      if (ext_transition.sv_write[i])
	nbEdges++;
      sprintf(buf, "%s%d", ((i==0)?"":","), ext_transition.sv_write[i]);
      append(buf);
    }
    line();

    totalpins += nbEdges;
    if (nets_out == NULL) {
      nets_out = fopen("out.nets","w");
      fprintf(nets_out,"UCLA nets   1.0\n# Generated by dvecompile\n# NumNets : %zu\n  NumPins : ", get_trans_count());
      pinspos= ftell(nets_out);
      // allow some space to write the number of pins into
      fprintf(nets_out,"         \n");
    }    
    fprintf(nets_out, "NetDegree : %d\n",nbEdges);
    // read
    for(size_int_t i = 0; i < count; i++)
    {
      if (ext_transition.sv_read[i]) {
	fprintf(nets_out, "v%zu I\n", i);
//	fprintf(nets_out, "%s I\n", varnames[i].c_str());
      }
    }
    line();
    fflush(nets_out);

    // write
    for(size_int_t i = 0; i < count; i++)
    {
      if (ext_transition.sv_write[i]) {
	fprintf(nets_out, "v%zu O\n", i);
//	fprintf(nets_out, "%s O\n", varnames[i].c_str());
      }
    }
    line();
    
    
}

void dve_compiler::analyse_expression( dve_expression_t & expr, ext_transition_t &ext_transition, std::vector<int> &dep)
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

void dve_compiler::analyse_transition_dependencies( ext_transition_t &ext_transition )
{
    // only for ltsmin
    if (!ltsmin)
        return;

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

void dve_compiler::analyse_transition(
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

void dve_compiler::analyse()
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
                vector<dve_transition_t*> transition_vector;
                transition_vector.push_back(transition);
                channel_map.insert(pair<size_int_t,vector<dve_transition_t*> >(
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

void dve_compiler::transition_guard( ext_transition_t *et, std::string in )
{
  line("BoolExpression guard = BoolExpressionFactory::createConstant(true) ");

  // For correct if_clause behavior
  m_if_disjoint = false; // AND the guard
  m_if_empty = false; // we've added "true" already

  if_cexpr_clause( et->first->get_guard(), in );

  if( et->synchronized )
    {
      if_clause( in_state( et->second->get_process_gid(),
			   et->second->get_state1_lid(), in ) );
      if_cexpr_clause( et->second->get_guard(), in );
    }
  else
    {
      int chan = et->first->get_channel_gid();
      if(et->first->get_sync_mode() == SYNC_EXCLAIM_BUFFER)
	if_clause( relate( channel_items( chan, in ), "!=",
			   fmt( channel_capacity( chan ) ) ) );
      
      if(et->first->get_sync_mode() == SYNC_ASK_BUFFER)
	if_clause( relate( channel_items( chan, in ), "!=", "0" ) );
    }
  line(";");
}

void dve_compiler::transition_effect( ext_transition_t *et, std::string in, std::string out )
{
    if(et->synchronized)
    {
        for(size_int_t s = 0;s < et->first->get_sync_expr_list_size();s++)
            assign( cexpr( *et->first->get_sync_expr_list_item(s), out ),
                    cexpr( *et->second->get_sync_expr_list_item(s), in ) );
    }
    else
    {
        int chan = et->first->get_channel_gid();
        if(et->first->get_sync_mode() == SYNC_EXCLAIM_BUFFER)
        {
            for(size_int_t s = 0;s < et->first->get_sync_expr_list_size();s++)
            {
                assign( channel_item_at( chan, channel_items( chan, in ), s, out ),
                        cexpr( *et->first->get_sync_expr_list_item( s ), in ) );
            }
            line( channel_items( chan, out ) + "++;" );
        }
        if(et->first->get_sync_mode() == SYNC_ASK_BUFFER)
        {
            for(size_int_t s = 0;s < et->first->get_sync_expr_list_size();s++)
                assign( cexpr( *et->first->get_sync_expr_list_item(s), out ),
                        channel_item_at( chan, "0", s, in ) );
            line( channel_items( chan, out ) + "--;" );

            line( "for(size_int_t i = 1 ; i <= " + channel_items( chan, out ) + "; i++)" );
            block_begin();
            for(size_int_t s = 0;s < et->first->get_sync_expr_list_size();s++)
            {
                assign( channel_item_at( chan, "i-1", s, out ), channel_item_at( chan, "i", s, in ) );
                assign( channel_item_at( chan, "i", s, out ), "0" );
            }
            block_end();
        }
    }

    //first transition effect
    assign( process_state( et->first->get_process_gid(), out ),
            fmt( et->first->get_state2_lid() ) );

    for(size_int_t e = 0;e < et->first->get_effect_count();e++) {
      print_cexpr( *et->first->get_effect(e), out );
    }

    if(et->synchronized) //second transiton effect
    {
        assign( process_state( et->second->get_process_gid(), out ),
                fmt( et->second->get_state2_lid() ) );
        for(size_int_t e = 0;e < et->second->get_effect_count();e++)
            print_cexpr( *et->second->get_effect(e), out );
    }

    if(have_property) //change of the property process state
        assign( process_state( et->property->get_process_gid(), out ),
                fmt( et->property->get_state2_lid() ) );

    // show dependency information in the source
    output_dependency_comment(*et);
}

void dve_compiler::new_output_state() {
    if (ltsmin) {
            line( "*out = *in;" );
    } else {
        if ( many ) {
            line( "divine::Blob blob_out( *pool, slack + state_size );" );
            line( "state_struct_t *out = (state_struct_t *)(blob_out.data() + slack);" );
            line( "blob_out.clear( 0, slack );" );
            line( "*out = *in;" );
        }
    }
}

void dve_compiler::yield_state() {
    if (ltsmin) {
        if (many) {
            line ("transition_info.group = " + fmt( current_label++) + ";");
        }
        line( "callback(arg, &transition_info, out);" );
        line( "++states_emitted;" );
    } else {
        if ( many ) {
            line( "if (buf_out->space() < 2) {" );
            line( "    buf_out->unadd( states_emitted );" );
            line( "    return;" );
            line( "}");
            line( "buf_out->add( (*buf_in)[ 0 ] );" );
            line( "buf_out->add( blob_out );" );
            line( "++states_emitted;" );
        } else {
            line( "return " + fmt( current_label ) + ";" );
        }
    }
}

void dve_compiler::gen_ltsmin_successors()
{
    string in = "(*in)", out = "(*out)", space = "";
    bool some_commited_state = false;

    // find some commited state
    for(size_int_t i = 0; i < get_process_count(); i++)
        for(size_int_t j = 0; j < dynamic_cast<dve_process_t*>(get_process(i))->get_state_count(); j++)
            if(dynamic_cast<dve_process_t*>(get_process(i))->get_commited(j))
                some_commited_state = true;

    if (some_commited_state)
    {
        for(size_int_t i = 0; i < this->get_process_count(); i++)
        {
            if( transition_map.find(i) != transition_map.end() && !is_property( i ) )
                for(iter_process_transition_map = transition_map.find(i)->second.begin();
                    iter_process_transition_map != transition_map.find(i)->second.end();
                    iter_process_transition_map++)
                {
                    if(dynamic_cast<dve_process_t*>(get_process(i))->get_commited(
                           iter_process_transition_map->first))
                    {
                        new_label();

                        // committed state
                        if_begin( true );

                        for(size_int_t p = 0; p < get_process_count(); p++)
                            for(size_int_t c = 0; c < dynamic_cast<dve_process_t*>(get_process(p))->get_state_count(); c++)
                                if(dynamic_cast<dve_process_t*>(get_process(p))->get_commited(c))
                                    if_clause( in_state( p, c, in ) );

                        if_end(); // otherwise this condition is disjoint with the new condition

                        if_begin(true);
                        if_clause( in_state( i, iter_process_transition_map->first, in ) );
                        if_end(); block_begin();

                        new_output_state();

                        for(iter_ext_transition_vector = iter_process_transition_map->second.begin();
                            iter_ext_transition_vector != iter_process_transition_map->second.end();
                            iter_ext_transition_vector++)
                        {
                            // !! jak je to s property synchronizaci v comitted stavech !!
                            if( !iter_ext_transition_vector->synchronized ||
                                dynamic_cast<dve_process_t*>(
                                    get_process(iter_ext_transition_vector->second->get_process_gid()))->
                                get_commited(iter_ext_transition_vector->second->get_state1_lid()) )
                            {
                                transition_guard( &*iter_ext_transition_vector, in );
                                block_begin();
                                transition_effect( &*iter_ext_transition_vector, in, out );
                                block_end();
                            }
                        }

                        yield_state();
                        block_end();
                        line("return states_emitted;");
                    }
                }
        }
    }

    for(size_int_t i = 0; i < get_process_count(); i++)
    {
        if(transition_map.find(i) != transition_map.end() && !is_property( i ))
            for(iter_process_transition_map = transition_map.find(i)->second.begin();
                iter_process_transition_map != transition_map.find(i)->second.end();
                iter_process_transition_map++)
            {
                for(iter_ext_transition_vector = iter_process_transition_map->second.begin();
                    iter_ext_transition_vector != iter_process_transition_map->second.end();
                    iter_ext_transition_vector++)
                {
                    // make sure this transition is not a committed one
                    if (!
                        dynamic_cast<dve_process_t*>(
                            get_process(iter_ext_transition_vector->first->get_process_gid()))->
                        get_commited(iter_ext_transition_vector->first->get_state1_lid()) )
                    {

                        new_label();

                        transition_guard( &*iter_ext_transition_vector, in );
                        block_begin();
                        if (some_commited_state)
                        {
                            // committed state
                            if_begin( true );

                            for(size_int_t p = 0; p < get_process_count(); p++)
                                for(size_int_t c = 0; c < dynamic_cast<dve_process_t*>(get_process(p))->get_state_count(); c++)
                                    if(dynamic_cast<dve_process_t*>(get_process(p))->get_commited(c))
                                        if_clause( in_state( p, c, in ) );

                            if_end();
                            line("    return 0;"); // bail out early
                        }


                        new_output_state();
                        transition_effect( &*iter_ext_transition_vector, in, out );
                        yield_state();
                        block_end();
                        line("return states_emitted;");
                    }
                }
            }
    }
}

void dve_compiler::gen_transient()
{
  std::stringstream sstream;
  sstream << "BoolExpressionFactory::createConstant(false) ";
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
	sstream << " || " << in_state(pid,pstate,"");
	nbtransient++;
      }
    }
  }
  if (nbtransient) {
    line ("setTransientPredicate (" + sstream.str() + ");");
    // set the transient predicate for the full type.
    line("// found "+ fmt(nbtransient) + " transient states");
  }
}

void dve_compiler::gen_successors()
{
    string in = "(*in)", out = "(*out)", space = "";


    new_label();
    if_begin( true );

    for(size_int_t i = 0; i < get_process_count(); i++)
        for(size_int_t j = 0; j < dynamic_cast<dve_process_t*>(get_process(i))->get_state_count(); j++)
            if(dynamic_cast<dve_process_t*>(get_process(i))->get_commited(j))
                if_clause( in_state( i, j, in ) );

    if_end(); block_begin();

    for(size_int_t i = 0; i < this->get_process_count(); i++)
    {
        if( transition_map.find(i) != transition_map.end() && !is_property( i ) )
            for(iter_process_transition_map = transition_map.find(i)->second.begin();
                iter_process_transition_map != transition_map.find(i)->second.end();
                iter_process_transition_map++)
            {
                if(dynamic_cast<dve_process_t*>(get_process(i))->get_commited(
                       iter_process_transition_map->first))
                {
                    new_label();

                    if_begin( true );
                    if_clause( in_state( i, iter_process_transition_map->first, in ) );
                    if_end(); block_begin();

                    new_output_state();

                    for(iter_ext_transition_vector = iter_process_transition_map->second.begin();
                        iter_ext_transition_vector != iter_process_transition_map->second.end();
                        iter_ext_transition_vector++)
                    {
                        // !! jak je to s property synchronizaci v comitted stavech !!
                        if( !iter_ext_transition_vector->synchronized ||
                            dynamic_cast<dve_process_t*>(
                                get_process(iter_ext_transition_vector->second->get_process_gid()))->
                            get_commited(iter_ext_transition_vector->second->get_state1_lid()) )
                        {
                            transition_guard( &*iter_ext_transition_vector, in );
                            block_begin();
                            transition_effect( &*iter_ext_transition_vector, in, out );
                            block_end();
                        }
                    }

                    yield_state();
                    block_end();
                }
            }
    }

    block_end();
    line( "else" );
    block_begin();

    // counter to label transitions
    int tnum = 0;

    for(size_int_t i = 0; i < get_process_count(); i++)
    {
        if(transition_map.find(i) != transition_map.end() && !is_property( i ))
            for(iter_process_transition_map = transition_map.find(i)->second.begin();
                iter_process_transition_map != transition_map.find(i)->second.end();
                iter_process_transition_map++)
            {
                // make sure this transition is not a committed one
                if (!
                    dynamic_cast<dve_process_t*>(
                        get_process(i))->get_commited(iter_process_transition_map->first) )
                {
		  
		  block_begin();		    

		  line("BoolExpression procState =" + in_state( i, iter_process_transition_map->first, in ) + ";");


		  for(iter_ext_transition_vector = iter_process_transition_map->second.begin();
		      iter_ext_transition_vector != iter_process_transition_map->second.end();
		      iter_ext_transition_vector++)
                    {
		      block_begin();		    
		      transition_guard( &*iter_ext_transition_vector, in );

		      std::string name = "t" + wibble::str::fmt(tnum++);
		      std::string label = 
			process_name(i) + "." + get_symbol_table()->get_state(iter_process_transition_map->first)->get_name() // + fmt(iter_process_transition_map->first) +"_" + process_name(i) 
			+ "-" +  get_symbol_table()->get_state(iter_ext_transition_vector->first->get_state2_gid())->get_name() ;
		      
		      line("GuardedAction ga(\"t" + wibble::str::fmt(tnum++)  + "\");");
//		      line("ga.setLabel(\""+label+"\");");
		      line("ga.setGuard(guard && procState);");

		      
		      transition_effect( &*iter_ext_transition_vector, in, out );
		      
		      line("addTransition(ga);");
		      block_end();
                    }
		  block_end();
                }
            }
    }
    block_end();
    
    fseek(nets_out,pinspos,SEEK_SET);
    fprintf(nets_out, "%d",totalpins);
    fclose(nets_out);
    nets_out = NULL;
}

void dve_compiler::gen_is_accepting()
{
}

void dve_compiler::print_generator()
{
    gen_header();
    gen_state_struct();
    gen_initial_state();

    gen_successors();

    gen_transient();

    gen_tail();
}

void dve_compiler::gen_state_info()
{

}

void dve_compiler::gen_transition_info()
{

}


void dve_compiler::mark_dependency( size_int_t gid, int type, int idx, std::vector<int> &dep )
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

int dve_compiler::count_state_variables()
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


}
