#ifndef __XMLLOADER_HH__
#define __XMLLOADER_HH__

#include <string>

#include <expat.h>

#include "TPNet.hh"

class XMLLoader 
{
	public:	
		static void loadTransitions(void * data, const XML_Char* Elt, const XML_Char** Attr);
		static void loadPlaces(void * data, const XML_Char* Elt, const XML_Char** Attr);
		static void loadArcs(void * data, const XML_Char* Elt, const XML_Char** Attr);
		static its::TPNet * loadXML(std::string filename);
			
};

#endif
