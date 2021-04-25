#include "Property.hh"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>

#DEFINE TRACE 0
namespace its {


  void loadProps(Label reachFile, std::vector<Property> & props) {


	std::string sline;
	std::ifstream myfile (reachFile.c_str());

	if (myfile.is_open())
	{
		if (myfile.good()) {
			// kill first line 
			std::getline (myfile,sline);
		}
		while ( myfile.good() )
		{
			std::getline (myfile,sline);
			size_t sz = sline.length();
			if (sz > 0 && sline[sz-1] == '\n') {
				sline.erase(sz-1);
			}
			if (sline == "")
			  continue;
			
			const char * line = sline.c_str();
			char name [1024] ;
			char type [1024];
			int n;
			if ( 2 == sscanf(line, "property %s %s : %n",name, type, &n) ) {
			  PropType ptype;
			  if (! strcmp(type,"[never]")) {
			    ptype = NEVER;
			  } else if (! strcmp(type,"[reachable]")) {
			    ptype = REACH;
			  } else if (! strcmp(type,"[invariant]")) {
			    ptype = INVARIANT;
			  } else if (! strcmp(type,"[bounds]")) {
			    ptype = BOUNDS;
			  } else {
			    std::cerr << "Unable to read property type " << type << " (skipping this line): " << line << std::endl;
			    continue;
			  }

			  props.push_back(Property(name, line+n , ptype));
	
			  if (TRACE) std::cerr << "Read "<<type<< " property : " << name << " with value :" << line+n << std::endl;
			} else {
			  std::cerr << "Unable to read property (skipping this line): " << line << std::endl;
			}			
		}
		myfile.close();
	} else {
      std::cerr << "Could not access property file : " << reachFile << std::endl;
      exit(1);
	}
  }

}
