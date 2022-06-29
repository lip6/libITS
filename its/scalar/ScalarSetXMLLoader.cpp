#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstring>

#include <expat.h>

#include "its/scalar/ScalarSet.hh"
#include "its/scalar/ScalarSetXMLLoader.hh"

// for pretty names
#include <map>


typedef std::map<int,std::string> map_t;
static map_t tnames;

static its::ITSModel * model=NULL;

void ScalarSetXMLLoader::loadNodes (void * data, const XML_Char* Elt, const XML_Char** Attr)
{
  int id=0;
  std::string name, type, label;
  std::stringstream s;
  
  its::ScalarSet *pn = (its::ScalarSet *) data;
  
  // switch on element type :
  if (! strcmp(Elt,"instance")) {
    for(int i=0; Attr[i]; i+=2) {
      if(!strcmp(Attr[i],"name")) {
	name = Attr[i+1];
      } else if (!strcmp(Attr[i],"type")) {
	type = Attr[i+1];
      } else if (!strcmp(Attr[i],"id")) {
	id = atoi(Attr[i+1]);
      }
    }
    // I do not use the label to make it easier to reference the
    // transition when creating arcs
    s << "i_" << id << "_" << name ;
    tnames[id] = s.str();

    pn->setInstance (s.str(), type, *model);
    pn->createStateDef ("init","init"); 
  } else if (! strcmp(Elt,"delegator")) {
    bool isPublic = false;
    bool isALL = true;
    for(int i=0; Attr[i]; i+=2) {
      if(!strcmp(Attr[i],"visibility")) {
	if (! strcmp("public",Attr[i+1])) {
	  isPublic = true;
	}
      } else if(!strcmp(Attr[i],"kind")) {
	if (! strcmp("ANY",Attr[i+1])) {
	  isALL = false;
	}
      } else if (!strcmp(Attr[i],"label")) {
	label = Attr[i+1];
      } else if (!strcmp(Attr[i],"id")) {
	id = atoi(Attr[i+1]);
      }
    }
    tnames[id] = label;

    pn->addDelegator (label, isPublic ? label : "", isALL);
  } else if (! strcmp(Elt,"size")) {
    int size = 0;
    for(int i=0; Attr[i]; i+=2) {
      if (!strcmp(Attr[i],"size")) {
	size = atoi(Attr[i+1]);
      }
    }
    pn->setSize(size);
  }

}

its::ScalarSet * ScalarSetXMLLoader::loadXML(const std::string & filename, its::ITSModel & modell, bool isTimed) 
{
  char *Buffer = NULL;
  
  model = &modell;
  // remove any path info up to file name: suppose path separator is / not backslash
  std::string netname = filename.substr(filename.find_last_of("/",filename.size())+1 );
  //remove 4 last characters = .xml (hopefully)
  netname = netname.substr(0 , netname.size()-4);
  
  its::ScalarSet *comp;
  comp = new its::ScalarSet(netname);
  
  // Load the file in a buffer    
  int length;
  std::ifstream file;
  file.open(filename.c_str(),std::ios::binary);

  if (!file.is_open()) {
    std::cerr << "Error while opening: " << filename << std::endl;
    exit(1);
  }
  file.seekg(0,std::ios::end);
  length = file.tellg();
  file.seekg(0, std::ios::beg);
  Buffer = new char[length+1];
  file.read(Buffer, length);
  file.close();
	
  // Parsing nodes
  XML_Parser p = XML_ParserCreate(NULL);
  XML_SetUserData(p, comp);
  XML_SetElementHandler(p, ScalarSetXMLLoader::loadNodes, NULL);
  XML_Parse(p, Buffer, strlen(Buffer), 1);
  XML_ParserFree(p);
	
  if (isTimed) {
    comp->addDelegator("elapse","elapse",true);
  }

	
  delete [] Buffer;
  
  return comp;
}
