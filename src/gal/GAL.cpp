#include "GAL.hh"


using std::endl;

namespace its {

  void Assignment::print (std::ostream & os) const {
    os << var_.getName() << " = " ;
    expr_.print(os);    
  }

  void  GuardedAction::print (std::ostream & os) const {
    os << "  transition " << getName() ;
    os << "  [ " ;
    guard_.print(os);
    os << " ] \n";
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

  std::ostream & operator<< ( std::ostream & os, const GAL & gal) {
    os << "GAL " << gal.getName() << endl;
    os << "{" << endl;
    os << "  //variables"<<endl;
    for (GAL::vars_it it = gal.vars_begin() ; it != gal.vars_end() ; ++it ) {
      os  << "  " << it->getName() << "=" << gal.getVarValue(it->getName()) << ";" << endl;
    }

    os << "  //transitions "<<endl;
    for (GAL::trans_it it = gal.trans_begin() ; it != gal.trans_end() ; ++it ) {
      it->print(os);
      os << endl;
    }

    os << "\n}\n"<<endl;;
    return os;
  }



}
