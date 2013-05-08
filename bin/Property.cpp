#include "Property.hh"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>

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
			
			const char * line = sline.c_str();
			char name [1024] ;
			char type;
			int n;
			if ( 2 == sscanf(line, "reach %s : %c %n",name, &type, &n) ) {
				props.push_back(Property(name, line+n , type=='I'));	
				std::cerr << "Read property : " << name << " with value :" << line+n << std::endl;
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
