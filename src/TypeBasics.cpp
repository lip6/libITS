#include "TypeBasics.hh"
#include "Hom_Basic.hh"
#include <algorithm>
#include <cstring>
#include <cstdio>

using std::find;



namespace its {

  // dtor
  TypeBasics::~TypeBasics() {
    if (vorder_ != NULL)
      delete vorder_;
  }

/** one initial state can be designated as initial state */
Label TypeBasics::getDefaultState() const {
	return default_;
}

// also sets to default if currently unset.
VarOrder * TypeBasics::getVarOrder () const {
  if (vorder_ == NULL) {
    // obtain variable set thru virtual call
    labels_t vnames = getVarSet ();
    vorder_ = new VarOrder(vnames);
  }
  return vorder_;
}

   /** set variable order in representation.
    * Vars should contain a permutation of all DD variables currently defined (i.e. as queriable through getVarSet()).    */
  bool TypeBasics::setVarOrder (labels_t vars) const {
    getVarOrder();
    return vorder_->updateOrder(vars);
  }

  /** the setter returns false if the label provided is not in InitStates */
bool TypeBasics::setDefaultState (Label def) {
  labels_t states = getInitStates ();
  labels_it it = find (states.begin(),states.end(),def);
  if (it != states.end()) {
    default_ = def;
    return true;
  } else {
    return false;
  }
}

  Transition TypeBasics::getPredicate (char * pred) const {
    // char  pred [predicate.size()+1];
    // strcpy(pred,predicate.c_str());

    if (! strcmp(pred,"true") ) {
      return Transition::id;
    } else if (! strcmp(pred,"false")) {
      return Transition::null;
    }

    if (*pred == '!') {
      if ( * (pred+1) == '(') {
	return ! getPredicate(pred+1);
      } else {
	std::cerr << "Syntax Error : we require not (!) to be followed by a parenthesized expression, e.g. write !(a>=1) instead of !a>=1.\nParsing :" << pred << std::endl;
	exit(1);
      }
    }

    // Test for nesting
    if (* pred == '(') {
      // scan ahead for closing paren.
      char * end = pred+1 ;
      for ( int open =1 ; *end ; ++end) {
	if ( * end == '(') {
	  ++open;
	} else if ( *end == ')' ) {
	  --open;
	  if (open == 0) break;
	}
      }
      if (*end =='\0') {
	std::cerr << "Syntax Error : Mismatched paren problem in predicate : " << pred << std::endl;
	exit(1);
      } else {
	*end='\0';
	Transition nested = getPredicate(pred+1);
	if (*(end+1)!='\0') {
	  if (! strncmp(end+1,"&&",2)) {
	    // std::cerr << "parsing predicate :" << predicate << std::endl;
	    // std::cerr << "Returning  :" << nested  <<  " combine with & getPred:" << end+3 << std::endl;	    
	    return nested & getPredicate(end+3);
	  } else if (! strncmp(end+1,"||",2)) {
	    // std::cerr << "parsing predicate :" << predicate << std::endl;
	    // std::cerr << "Returning  :" << nested  <<  " combine with + getPred:" << end+3 << std::endl;	    
	    return nested + getPredicate(end+3);
	  } else {
	    std::cerr << "Syntax Error : Expected operator && or || after predicate : (" << pred+1 << ") but encountered unexpected:" << end+1 << std::endl;
	    exit(1);
	  }
	} else {
	  return nested;
	}
      } 
    } else {
      // No nesting case
      char * and_t = strstr(pred,"&&");
      char * or_t = strstr(pred,"||");
      if (! and_t && ! or_t) {
	// std::cerr << "parsing predicate :" << predicate << std::endl;
	// std::cerr << "Returning  atomic predicate." << std::endl;	    

	return getAPredicate(pred);
      } 
      if ( and_t && (! or_t || and_t < or_t)) {
	*and_t = '\0';
	Transition left = getPredicate(pred);
	// std::cerr << "parsing predicate :" << predicate << std::endl;
	// std::cerr << "Returning  :" << left  <<  " combine with & getPred:" << and_t+2 << std::endl;	    

	return left & getPredicate(and_t+2);
      } else {
	// so we are sure that :
	// ( or_t && (! and_t || or_t < and_t)) 
	*or_t = '\0';
	Transition left = getPredicate(pred);

	// std::cerr << "parsing predicate :" << predicate << std::endl;
	// std::cerr << "Returning  :" << left  <<  " combine with + getPred:" << or_t+2 << std::endl;	    

	return left + getPredicate(or_t+2);
      }	
    }
    
  }

  
  static void recPrintDDD (const GDDD & d, std::ostream & os, const VarOrder & vo, vLabel str) {
    if (d == DDD::one)
      os << "[ " << str << "]"<<std::endl;
    else if(d == DDD::top)
      os << "[ " << str << "T ]"<<std::endl;
    else if(d == DDD::null)
      os << "[ " << str << "0 ]"<<std::endl;
    else{
      
		for(GDDD::const_iterator vi=d.begin();vi!=d.end();++vi){
		  if (vi->first == 0) {
			recPrintDDD(vi->second,os,vo,str);
		  } else {
			std::stringstream tmp;
			tmp << vo.getLabel(d.variable())<<'='<<vi->first<<" ";
			recPrintDDD(vi->second,os,vo,str+tmp.str());
		  }
		}
    }
  }


  static void recPrintSDD (State s, std::ostream & os, const VarOrder & vo, vLabel str) {
    if (s == State::one)
      os << "[ " << str << "]";
    else if(s ==  State::top)
      os << "[ " << str << "T ]";
    else if(s == State::null)
      os << "[ " << str << "0 ]";
    else{
      for(State::const_iterator vi=s.begin(); vi!=s.end(); ++vi){
	
		
	// grab the DDD on the arc	
	DDD val = (const DDD &) * vi->first;
	
	if (val.nbsons() == 1 && val.begin()->first == 0) {
	  // skip {0} values
	  recPrintSDD(vi->second, os, vo, str);
	} else {
	  std::stringstream tmp;
	  // pretty print variable names
	  Label varname = vo.getLabel(s.variable());
	  tmp << varname << "={";
	
	  for (DDD::const_iterator it = val.begin(); it != val.end() ; /**increment in loop */) {
	    tmp << to_string(it->first) ;
	    ++it;
	    if (it != val.end()) tmp << ",";
	  }
	  tmp << "} ";
	
	  recPrintSDD(vi->second, os, vo, str + tmp.str());
	}
      }
    }
  }

  void TypeBasics::printSDDState (State s, std::ostream & os, const VarOrder & vo) {
    recPrintSDD(s, os, vo, "");
  }
  
  void TypeBasics::printDDDState (State s, std::ostream & os, const VarOrder & vo) {
    // should have a single variable, hence a single arc with a DDD label
    if (s==State::null) {
      os << "EmptySet";
      return;
    }
    assert(s.begin() != s.end());
    assert(s.begin()->second == State::one);
    DDD state = (const DDD &) * s.begin()->first;
    // for now just invoke DDD print
    recPrintDDD(state, os, vo, "");
  }

  // helper function
  AtomicPredicate TypeBasics::parseAtomicPredicate (Label predicate) const {
        // The predicate should respect the grammar : varName (=|>|<|>=|<=|<|!=) value
    // Where varName is a place or a transition (hence designating its clock) such as found in getVariableSet(), getVarOrder()
    // and value is an integer
    
    // step 1 : parse the predicate
    const char * pred = predicate.c_str();
    int mode = 0;
    vLabel var,comp,val;
    for (const char * cp = pred ; *cp ; ++cp) {
      // skip ws
      if (*cp == ' ')
	continue;
      switch (mode) {
	// parsing variable
      case 0:
	if (*cp == '!' || *cp == '>' || *cp == '=' || *cp == '<') {
	  mode = 1;
	  comp += *cp;
	} else {
	  var += *cp;
	}
	break;
      case 1:
	if (*cp == '!' || *cp == '>' || *cp == '=' || *cp == '<') {
	  comp += *cp;
	} else {
	  val += *cp;
	}
	mode = 2;
	break;
      case 2:
	val += *cp;
      }
    }
    // Check parse
    int index = getVarOrder()->getIndex(var);
    if ( index == -1 ) {
      std::cerr << "Error variable " + var + " cannot be resolved when trying to parse predicate : "  + predicate << std::endl;
      std::cerr << "Failing with error code 2"<< std::endl;
      exit (2);
    }
    GHom (* foo) (int,int)  = NULL;
    if ( comp == "=" ) {
      foo = & varEqState;
    } else if (comp == "!=") {
      foo = & varNeqState;
    } else if (comp == ">") {
      foo = & varGtState;
    } else if (comp == "<") {
      foo = & varLtState;
    } else if (comp == "<=") {
      foo = & varLeqState;
    } else if (comp == ">=") {
      foo = & varGeqState;
    } else {
      std::cerr << "Unrecognized comparison operator : " << comp << " when parsing predicate " << predicate << std::endl;
      std::cerr << "Error is fatal, failing with error code 2" << std::endl;
      exit (2);
    }
    int value = 0;
    if ( sscanf (val.c_str(), "%d" , &value) == 0 ) {
      std::cerr << "Unable to parse right hand side of comparison as an integer : " << comp << " when parsing predicate " << predicate << std::endl;
      std::cerr << "Error is fatal, failing with error code 2" << std::endl;
      exit (2);      
    }
    return AtomicPredicate(index,foo,value);
  }


} // namespace
