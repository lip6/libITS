#ifndef __SMT__EXPORTER_DDD__
#define __SMT__EXPORTER_DDD__


#include "ITSModel.hh"

class SMTExporter {
private :
  std::ofstream  out;
public:
  SMTExporter(Label path);

  bool exportDD (const its::ITSModel & model, its::State reachable) ;
  void exportPredicate(Label pname, const its::ITSModel & model, Label pred);

  void close();
};

#endif
