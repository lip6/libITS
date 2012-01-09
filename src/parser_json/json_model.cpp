#include "ITSModel.hh"
#include "petri/JSONPNetType.hh"
	
	
namespace its {
	  // Create a type to hold a Petri net, with hierarchical representation based on JSON description.
  bool ITSModel::declareType (const class PNet & net, const json::Hierarchie * hier) {
    return addType(new JsonPNetType(net,hier));
  }
}

