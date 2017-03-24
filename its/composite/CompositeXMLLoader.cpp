#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstring>

#include <expat.h>

#include "its/composite.hh"
#include "TComposite.hh"
#include "its/compositeXMLLoader.hh"

// for pretty names
#include <map>


typedef std::map<int,std::string> map_t;
static map_t tnames;

static its::ITSModel * model=NULL;

void CompositeXMLLoader::loadNodes (void * data, const XML_Char* Elt, const XML_Char** Attr)
{
  int id=0;
  std::string name, type, label;
  std::stringstream s;
  
  its::Composite *pn = (its::Composite *) data;
  
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
//    s << "i_" << id << "_" << name ;
    s << name;
    tnames[id] = s.str();

    pn->addInstance (s.str(), type, *model);
    pn->updateStateDef ("init",s.str(),"init"); 
  } else if (! strcmp(Elt,"synchronization")) {
    for(int i=0; Attr[i]; i+=2) {
      if(!strcmp(Attr[i],"name")) {
	name = Attr[i+1];
      } else if (!strcmp(Attr[i],"label")) {
	label = Attr[i+1];
      } else if (!strcmp(Attr[i],"id")) {
	id = atoi(Attr[i+1]);
      }
    }
    // I do not use the label to make it easier to reference the
    // transition when creating arcs
//    s << "T_" << id << "_" << name ;
    s << name;
    tnames[id] = s.str();

    pn->addSynchronization (s.str(), label);
  }
  
}

void CompositeXMLLoader::loadArcs(void * data, const XML_Char* Elt, const XML_Char** Attr)
{
    int instID=0, syncID =0 ;
    std::string labels;
    std::string type;

    its::Composite *comp = (its::Composite *) data;

    if(!strcmp(Elt,"arc")) {
        for(int i=0; Attr[i]; i+=2) {
            if(!strcmp(Attr[i],"instance")) {
	      instID = atoi(Attr[i+1]);
            } else if(!strcmp(Attr[i],"synchronization")) {
	      syncID = atoi(Attr[i+1]);
            } else if(!strcmp(Attr[i],"labels")) {
	      labels = Attr[i+1];
            }
        }
	std::string sname = tnames[syncID];
	std::string iname = tnames[instID];

	std::string buff;
	// parse the labels field 
	for (std::string::iterator it = labels.begin() ; it != labels.end() ; ++it ) {
	  if (*it == ';') {
	    // end of this label
	    comp->addSyncPart(sname, iname, buff);
	    buff = "";
	  } else if ( *it == '\n' || *it == '\t' || *it == ' ') {
	    // NOP
	  } else {
	    buff += *it;
	  }
	}
	if (buff != "") {
	    comp->addSyncPart(sname, iname, buff);
	}
    }
}

its::Composite * CompositeXMLLoader::loadXML(const std::string & filename, its::ITSModel & modell, bool isTimed) 
{
  char *Buffer = NULL;
  
  model = &modell;
  // remove any path info up to file name: suppose path separator is / not backslash
  std::string netname = filename.substr(filename.find_last_of("/",filename.size())+1 );
  //remove 4 last characters = .xml (hopefully)
  netname = netname.substr(0 , netname.size()-4);
  
  its::Composite *comp;
  if (isTimed) {
    comp = new its::TComposite(netname);
  } else {
    comp = new its::Composite(netname);
  }

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
  XML_SetElementHandler(p, CompositeXMLLoader::loadNodes, NULL);
  XML_Parse(p, Buffer, strlen(Buffer), 1);
  XML_ParserFree(p);
	
  // Parsing arcs
  p = XML_ParserCreate(NULL);
  XML_SetUserData(p, comp);
  XML_SetElementHandler(p, CompositeXMLLoader::loadArcs, NULL);
  XML_Parse(p, Buffer, strlen(Buffer), 1);
  XML_ParserFree(p);
	
  delete [] Buffer;
  
  return comp;
}
