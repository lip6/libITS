#ifndef __JSON_2_ITS__HH__
#define __JSON_2_ITS__HH__


#include "its/ITSModel.hh"


class JSONLoader
{
public :
  /** Parse a cami file and an order configuration file (JSON format) and produce an equivalent ITS system. */
  static vLabel loadJsonCami (its::ITSModel & model, const std::string & pathcamiff, const std::string & confff) ;
  /** Parse a cami file and produce a flat Petri net equivalent ITS model. */
  static vLabel loadCami (its::ITSModel & model, const std::string & pathcamiff) ;
   /** Parse a JSON file and apply the provided hierarchy/order on the provided Petri net to build an ITS model. */
  static vLabel loadJsonRdPE (its::ITSModel & model, class RdPE & R, const std::string & confff);
};



#endif
