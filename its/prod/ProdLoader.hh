#ifndef __PROD__LOADER__HH__
#define __PROD__LOADER__HH__

#include "its/petri/PNet.hh"

class ProdLoader
{
public :
  static its::PNet * loadProd (const std::string & filename);
};


#endif

