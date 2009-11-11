#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstring>

#include <expat.h>

#include "TPNet.hh"
#include "PTransition.hh"
#include "XMLLoader.hh"

// for pretty names
#include <map>


typedef std::map<int,std::string> map_t;
static map_t tnames;
static map_t pnames;

void XMLLoader::loadTransitions(void * data, const XML_Char* Elt, const XML_Char** Attr)
{
    int id=0, eft=0, lft=0;
    std::string label;
    std::stringstream s;
    bool isPublic = false;

    its::TPNet *pn = (its::TPNet *) data;

    if(!strcmp(Elt,"transition")) {
        for(int i=0; Attr[i]; i+=2) {
            if(!strcmp(Attr[i],"id")) {
                id = atoi(Attr[i+1]);
            }
            else if(!strcmp(Attr[i],"label")) {
                label = Attr[i+1];
            }
            else if(!strcmp(Attr[i],"eft")) {
                eft = atoi(Attr[i+1]);
            }
            else if(!strcmp(Attr[i],"public")) {
                isPublic = atoi(Attr[i+1]);
            }
            else if(!strcmp(Attr[i],"lft")){
	      if (! strcmp(Attr[i+1],"infini") ){
		lft = -1;
	      } else {
		lft = atoi(Attr[i+1]);
	      }
            }
        }
        
        // I do not use the label to make it easier to reference the
        // transition when creating arcs
	if (isPublic)
	  s << label ;
	else
	  s << "T_" << id << label ;
	tnames[id] = s.str();
	if (isPublic)
	  pn->addTransition(s.str(),its::PUBLIC);
	else
	  pn->addTransition(s.str(),its::PRIVATE);

	pn->addClock(s.str(),eft,lft);
    }
}

void XMLLoader::loadPlaces(void * data, const XML_Char* Elt, const XML_Char** Attr)
{
    int initialMarking=-1,id=0;
    std::string label;
    std::stringstream s;

    its::TPNet *pn = (its::TPNet *) data;

    
    if(!strcmp(Elt,"place")) {
        for(int i=0; Attr[i]; i+=2) {
            if(!strcmp(Attr[i],"id")) {
                id = atoi(Attr[i+1]);
            }
            else if(!strcmp(Attr[i],"label")) {
                label = Attr[i+1];
            }
            else if(!strcmp(Attr[i],"initialMarking")) {
                initialMarking = atoi(Attr[i+1]);
            }
        }
        
        // I do not use the label to make it easier to reference the place
        // when creating arcs
        s << "P_" << id << label;
      	pnames[id] = s.str() ;

        pn->addPlace(s.str());
	// Add place marking to "init", ignore empty markings	
	if (initialMarking > 0) 
	  pn->setMarking("init",s.str(),initialMarking);
	}
}

void XMLLoader::loadArcs(void * data, const XML_Char* Elt, const XML_Char** Attr)
{
    int place=0, transition=0, weight=0;
    std::string type;
    std::stringstream sp, st;
    its::PNet::ArcVal av;
    its::ArcType at= its::INPUT;

    its::TPNet *pn = (its::TPNet *) data;

    if(!strcmp(Elt,"arc")) {
        for(int i=0; Attr[i]; i+=2) {
            if(!strcmp(Attr[i],"place")) {
                place = atoi(Attr[i+1]);
            }
            else if(!strcmp(Attr[i],"transition")) {
                transition = atoi(Attr[i+1]);
            }
            else if(!strcmp(Attr[i],"weight")) {
                weight = atoi(Attr[i+1]);
            }
            else if(!strcmp(Attr[i],"type")) {
                type = Attr[i+1];
            }
        }

        
        av.first = pnames[place];
        av.second = weight;

    	if (type == "PlaceTransition" ) {
    		at = its::INPUT;
    	} else if (type == "TransitionPlace" ) {
    		at = its::OUTPUT;
    	} else if (type == "logicalInhibitor" ) {
            // Was it intended to handle stopwatches? If so, then it is
            // "timedInhibitor" and not "logicalInhibitor" 
    		at = its::INHIBITOR;
    	} else if (type == "flush" ) {
	  at = its::RESET;
	}

        pn->addArc(av,tnames[transition],at);
    }
}

its::TPNet * XMLLoader::loadXML(std::string filename) 
{
	char *Buffer = NULL;

	
	// remove any path info up to file name: suppose path separator is / not backslash
	std::string netname = filename.substr(filename.find_last_of("/",filename.size())+1 );
	//remove 4 last characters = .xml (hopefully)
	netname = netname.substr(0 , netname.size()-4);
	
    its::TPNet *pn = new its::TPNet(netname);

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
    Buffer[length] = '\0';
    file.read(Buffer, length);
    file.close();
	
	// Parsing places
	XML_Parser p = XML_ParserCreate(NULL);
	XML_SetUserData(p, pn);
    XML_SetElementHandler(p, XMLLoader::loadPlaces,NULL);
	XML_Parse(p, Buffer, strlen(Buffer), 1);
	XML_ParserFree(p);
	
	// Parsing transitions
	p = XML_ParserCreate(NULL);
	XML_SetUserData(p, pn);
	XML_SetElementHandler(p, XMLLoader::loadTransitions, NULL);
	XML_Parse(p, Buffer, strlen(Buffer), 1);
	XML_ParserFree(p);
	
	// Parsing arcs
	p = XML_ParserCreate(NULL);
	XML_SetUserData(p, pn);
	XML_SetElementHandler(p, XMLLoader::loadArcs, NULL);
	XML_Parse(p, Buffer, strlen(Buffer), 1);
	XML_ParserFree(p);
    
	delete [] Buffer;
    
    return pn;
}
