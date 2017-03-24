#ifndef __MODULAR_2_ITS__HH__
#define __MODULAR_2_ITS__HH__


#include "its/ITSModel.hh"


class RdPELoader
{
public :
  static vLabel loadModularProd (its::ITSModel & model, const std::string & pathnetff) ;
  static vLabel loadModularRdPE (its::ITSModel & model, class RdPE & R) ;
  static vLabel loadJsonProd (its::ITSModel & model, const std::string & pathnetff, const std::string & confff);
};



#endif
