#ifndef __CIRCULAR_XMLLOADER_HH__
#define __CIRCULAR_XMLLOADER_HH__

#include <string>

#include <expat.h>

#include "its/ITSModel.hh"

class CircularSetXMLLoader 
{
public:	
  static void loadNodes(void * data, const XML_Char* Elt, const XML_Char** Attr);
  static void loadArcs(void * data, const XML_Char* Elt, const XML_Char** Attr);
  static its::CircularSet * loadXML(const std::string & filename, its::ITSModel & model, bool isTimed=false);
			
};

#endif
