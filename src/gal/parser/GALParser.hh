#ifndef __GAL_ANTLRLOADER_HH__
#define __GAL_ANTLRLOADER_HH__

#include "gal/GAL.hh"

namespace its {

class GALParser 
{
public:	
  static its::GAL * loadGAL(const std::string & filename);
			
};

}


#endif
