#include "Property.hh"

#include <cstdio>

namespace its {


  void loadProps(Label reachFile, std::vector<Property> & props) {
    FILE * file = fopen(reachFile.c_str(), "r");
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    if (file == NULL) {
      std::cerr << "Could not access property file : " << reachFile << std::endl;
      exit(1);
    }

    char name [1024] ;
    char type;
    // kill first line 
    read = getline(&line, &len, file);

    while ((read = getline(&line, &len, file)) != -1) {
      int n ;
      if ( 2 == sscanf(line, "reach %s: %c %n",name, &type, &n) ) {
	props.push_back(Property(name, line+n , type=='I'));	
	std::cerr << "Read property : " << name << " with value :" << line+n << std::endl;
      }

//       printf("Retrieved line of length %zu :\n", read);
//       printf("%s", line);
    }
    
    std::cerr << "";

    if (line)
      free(line);

    fclose(file);
  }



}
