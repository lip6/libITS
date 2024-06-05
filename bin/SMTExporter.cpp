#include <iostream>
#include <fstream>

#include "SMTExporter.hh"
#include "its/gal/GALType.hh"
#include "its/gal/PIntExprVisitor.hh"
#include "its/gal/parser/GALParser.hh"

using namespace its;


typedef ext_hash_map<GDDD, vLabel> cache_t;
cache_t cache;

static int nextIndex = 0;


void declareIntVar (Label vname, std::ostream & out) {
  out << "(declare-const |"<< vname << "| Int)" << std::endl;
}

Label exportDDrec (const VarOrder * vo, const GDDD & states, std::ostream & out, cache_t & cache) {
  cache_t::accessor access;  
  cache.find(access, states);

  if( access.empty() ) {
    vLabel res;
    // miss
    if (states == GDDD::one) {
      res = "true";
    } else {
      res = "n" + to_string(nextIndex++) ;
      std::stringstream tmpout ;
      tmpout << "(define-fun " 
	  << res  // name
	  << " () "  // empty param list
	  << " Bool "  //return type 
	;
      Label vname = "|" + vo->getLabel (states.variable() ) + "|";

      std::stringstream clauses;
      for (GDDD::const_iterator it = states.begin() ; it  != states.end() ; ++it) {
	clauses << "(and " 
		<< "(= " << vname << " " << it->first << ") "
	        << exportDDrec(vo, it->second, out, cache).c_str()
	        << ") ";
      }
      if (states.nbsons() == 1) {
	tmpout << clauses.str() ;
      } else {
	tmpout << "(or " << clauses.str() << ")" ;
      }
      tmpout << ")";
      out << tmpout.str() <<  std::endl;
    }
    cache.insert(access,states);
    access->second = res;
    return access->second;
  } else {
    // hit
    return access->second;
  }
}

void exportDD (const VarOrder * vo, const GDDD & states, std::ostream & out, cache_t & cache) {
//  for (size_t i = 0 ; i < vo->size() ; ++i ) {
//    Label var = vo->getLabel(i);
//    declareIntVar(var, out);
//  }

  exportDDrec(vo, states, out, cache);

  out << "(define-fun "
      << "unreachable"  // name
      << " () "  // empty param list
      << " Bool "  //return type
      << " (not n0))" << std::endl;

 // out << "(assert n0)" << std::endl;
 // out << "(check-sat)" << std::endl;

}

SMTExporter::SMTExporter (Label path) : out(path.c_str())  {
}


class CollectType : public BasicTypeVisitor  {
public :
  const class GAL * gal;
  void visitGAL( const class GAL & gall) { 
    gal = &gall;
  }

};

class SMTOutputter : public PIntExprVisitor, public PBoolExprVisitor {
  std::ostream & out;
  const env_t & env;
public :
  SMTOutputter (std::ostream & out,const env_t & env) : out(out), env(env) {};
  void visitVarExpr (int index) {
    out <<  "|" << IntExpressionFactory::getVar (env[ index ]) << "|";
  }
  void visitConstExpr (int val) {
    out << to_string(val);
  }
  void visitNDefExpr () {
    std:: cout << "NDEF in expression to translate to SMT !";
  }
  void visitWrapBoolExpr (const class PBoolExpression & be) {
    std:: cout << "WrapBool in expression to translate to SMT !";
  }
  void visitArrayVarExpr (int, const class PIntExpression &) {
    std:: cout << "ArrayVar in expression to translate to SMT !";
  }
  void visitArrayConstExpr (int arr, const class PIntExpression & index) {
    out <<  "|" << IntExpressionFactory::getVar (env[ arr ]) << "[";
    index.accept(this);
    out << "]|";
  }
  void visitNaryIntExpr (IntExprType type, const NaryPParamType & args) {
    if (type == PLUS ) {
      out << "(+ ";
    } else {
      out << "(* ";
    }
    for (NaryPParamType::const_iterator it = args.begin() ; it != args.end() ; ++it) {
      it->accept(this);
      out << " ";
    }
    out << ")";
  }
  void visitBinaryIntExpr (IntExprType, const class PIntExpression &, const class PIntExpression &) {

  }

  void visitUnaryIntExpr (IntExprType, const class PIntExpression &) {

  }

  void visitNaryBoolExpr (BoolExprType type, const std::vector<class PBoolExpression> & args) {
    if (type == AND) {
      out << "(and ";
    } else {
      out << "(or ";
    }
    for (std::vector<class PBoolExpression>::const_iterator it = args.begin() ; it != args.end(); ++it) {
      it->accept(this);
      out << " ";
    }
    out << ")";
  }
  void visitBinaryBoolComp (BoolExprType type, const class PIntExpression & l, const class PIntExpression & r) {
    switch (type) {
    case EQ :
      out << "(= ";
      break;
    case NEQ :
      out << "(not (= ";
      break;
    case LT :
      out << "(< ";
      break;
    case GT :
      out << "(> ";
      break;
    case LEQ :
      out << "(<= ";
      break;
    case GEQ :
      out << "(>= ";
      break;
    default :
      throw "Incorrect expression structure !";
    }
    l.accept(this);
    out << " ";
    r.accept(this);
    out << ")";
    if (type == NEQ) {
      out << ")";
    }
  }
  void visitNotBoolExpr (const class PBoolExpression &e) {
    out << "(not " ;
    e.accept(this);
    out << ")";
  }
  void visitBoolConstExpr (bool b) {
    if (b) {
      out << "true";
    } else {
      out << "false";
    }
  }
  void visitBoolNDefExpr () {
    std:: cout << "ArrayVar in expression to translate to SMT !";
  }


};

void SMTExporter::exportPredicate (Label pname, const its::ITSModel & model, Label pred) {
  CollectType ct;
  model.getInstance()->getType()->visit(&ct);
  BoolExpression be = GALParser::parsePredicate(pred, ct.gal);
  be = be.eval().pushNegations();
  SMTOutputter smto (out, be.getEnv());
  out << "(define-fun " << pname << " () Bool "<< std::endl;
  be.getExpr().accept(&smto);
  out << ")" << std::endl;  
  out << "(push)" << std::endl;
  out << "(assert "<< pname << ")";
  out << "(check-sat)" << std::endl;
  out << "(pop)" << std::endl;
}

void SMTExporter::close() {
  out.close();
}

bool SMTExporter::exportDD (const ITSModel & model, its::State reachable)  {
  
  
  if (reachable.nbsons() == 1) {
    State::const_iterator it = reachable.begin();
    if (it-> second == State::one) {
       if (typeid(* it->first) == typeid(DDD)) {
	 cache_t cache;
	 // add header, set logic...
	 ::exportDD( model.getInstance()->getType()->getVarOrder(), GDDD ( (DDD&) *it->first ), out, cache);
	 return true;
       }
    }
  }

  std::cout << "could not export SDD states yet." << std::endl;
  return false;
}
