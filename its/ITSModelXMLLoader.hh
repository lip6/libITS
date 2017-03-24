#ifndef __ITSMODEL_XMLLOADER_HH__
#define __ITSMODEL_XMLLOADER_HH__

#include <string>

#include <expat.h>

#include "its/ITSModel.hh"

class ITSModelXMLLoader 
{
public:	
  static void loadTypes(void * data, const XML_Char* Elt, const XML_Char** Attr);
  static void loadScenario(void * data, const XML_Char* Elt, const XML_Char** Attr);
  static void loadXML(const std::string & filename, its::ITSModel & model);
			
};

#endif
