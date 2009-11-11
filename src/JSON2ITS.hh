#ifndef __JSON_2_ITS__HH__
#define __JSON_2_ITS__HH__


#include "ITSModel.hh"


class JSONLoader
{
public :
  /** Parse a cami file and an order configuration file (JSON format) and produce an equivalent ITS system. */
  static void loadJsonCami (its::ITSModel & model, const std::string & pathcamiff, const std::string & confff) ;

};



#endif
