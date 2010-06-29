#ifndef __PROD__LOADER__HH__
#define __PROD__LOADER__HH__

#include "petri/PNet.hh"

class ProdLoader
{
public :
  static its::PNet * loadProd (const std::string & filename);
};


#endif

