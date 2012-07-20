#include "GAL.hh"


using std::endl;

namespace its {

  bool Assignment::operator==(const Assignment &other) const {
    return var_.equals(other.var_) && expr_.equals(other.expr_);
  }
  
  void Assignment::print (std::ostream & os) const {
    var_.print(os);
    os <<  " = " ;
    expr_.print(os);    
  }
  
  Assignment Assignment::operator&(const Assertion &a) const {
    IntExpression new_var = var_ & a;
    IntExpression new_expr = expr_ & a;
    return Assignment( new_var, new_expr );
  }
  
  std::set<Variable> Assignment::getSupport() const {
    std::set<Variable> result = var_.getSupport();
    std::set<Variable> tmp = expr_.getSupport();
    result.insert( tmp.begin(), tmp.end() );
    return result;
  }

  std::set<Variable> GuardedAction::getSupport() const {
    std::set<Variable> result = guard_.getSupport();
    for (actions_it it = begin() ; it != end() ; ++it) {
      std::set<Variable> tmp = it->getSupport();
      result.insert( tmp.begin(), tmp.end() );
    }
    return result;
  }
  
  bool GuardedAction::operator==(const GuardedAction &other) const {
    bool result = guard_ == other.guard_;
    actions_it ait, bit;
    for (ait = begin(), bit = other.begin() ; result && ait != end() && bit!= other.end() ; ++ait, ++bit) {
      result = (*ait) == (*bit);
    }
    if (ait == end() && bit != other.end()) {
      return false;
    }
    if (bit == other.end() && ait != end()) {
      return false;
    }
    return  result;
  }
  
  void  GuardedAction::print (std::ostream & os) const {
    os << "  transition " << getName() ;
    os << "  [ " ;
    guard_.print(os);
    os << " ] \n";
    if (label_ != "") 
      os << "label \"" << label_ << "\"" ;
    os << "     { " ;
    for (actions_it it = begin(); it != end() ; /*done in loop */ ) {
      it->print(os) ;
      os << ";";
      if (++it != end() ) {
	os << "\n       ";
      }
    }
    os << "    }" << endl;
  }

  ArrayDeclaration::ArrayDeclaration (Label name, size_t size) : name_(name), size_(size) {
      for (size_t i=0; i < size; ++i){
	vars_.push_back( IntExpressionFactory::createArrayAccess( name, i).eval().getName() );
      }
  }


  std::ostream & operator<< ( std::ostream & os, const GAL & gal) {
    os << "GAL " << gal.getName() << endl;
    os << "{" << endl;

    os << "  //arrays"<<endl;
    for (GAL::arrays_it it = gal.arrays_begin() ; it != gal.arrays_end() ; ++it ) {
      os  << "  int[" << it->size() << "] " << it->getName() << "= (" ;
      for (ArrayDeclaration::vars_it jt = it->vars_begin() ; jt != it->vars_end() ; /* in loop */ ) {
	os << gal.getVarValue(jt->getName()) ;
	++jt;
	if (jt != it->vars_end()) {
	  os << ", ";
	}
      }
      os << ") ;" << std::endl;
    }
    

    os << "  //variables"<<endl;
    for (GAL::vars_it it = gal.vars_begin() ; it != gal.vars_end() ; ++it ) {
      os  << "  int " << it->getName() << "=" << gal.getVarValue(it->getName()) << ";" << endl;
    }

    os << "  //transitions "<<endl;
    for (GAL::trans_it it = gal.trans_begin() ; it != gal.trans_end() ; ++it ) {
      it->print(os);
      os << endl;
    }

    os << "  // transient predicate " << endl ;
    os << "TRANSIENT = " << gal.isTransientState() << ";"  << std::endl;

    os << "\n}\n"<<endl;;
    return os;
  }

  void GAL::addArray (Label name, const vals_t & vals) 
  {
    
    ArrayDeclaration ad = ArrayDeclaration(name,vals.size());
    arrays_.push_back(ad);
    int i = 0;
    for (ArrayDeclaration::vars_it it = ad.vars_begin() ; it != ad.vars_end() ; ++it,++i ) {
      init_.setVarValue(it->getName(), vals[i]);
    }
  } 
  

}
