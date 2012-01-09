
/**
 * \author Yann TM, HONG Silien
 * \version 1.2
 * \date 09/01/2012
 * \brief Module de définition du parseur d'un fichier JSON.
 * Le format JSON permet de rendre configurable l'état initial d'un SDD selon un fichier de configuration au format JSON
 * ou l'utilisateur va décrire l'ordre de la hiérarchie de chaque place.
 * Ici nous utilisons un parseur BOOST JSON de John W. Wilkinson (http://www.codeproject.com/KB/recipes/JSON_Spirit.aspx)
 */

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include "cJSON.h"
#include "parse_json.hh"

namespace json {


void readJSONintoHierarchy(cJSON * current, Hierarchie & hh) 
{
  if (current->type == cJSON_String) {
    hh.addPlace(current->valuestring);
  } else {
    Hierarchie* h = new Hierarchie();
    hh.addHierarchie((*h));
    cJSON *subitem=current->child;
    while (subitem)
      {
	// handle subitem
	readJSONintoHierarchy(subitem, *h);
	subitem=subitem->next;
      }

  }
}


/**
 * \param filename : Nom du fichier de configuration JSON à parser
 * \param h1 : Renvoie la configuration de la hiérarchie
 */
void json_parse(const std::string& filename,Hierarchie& h1)
{
  FILE *fp;
  long len;
  char *buf;
  fp=fopen(filename.c_str(),"rb");
  if (fp == NULL) {
    perror(("Error opening JSON file "+ filename + "\n Quitting, sorry.").c_str());    
  }
  fseek(fp,0,SEEK_END); //go to end
  len=ftell(fp); //get position at end (length)
  fseek(fp,0,SEEK_SET); //go to beg.
  buf=(char *)malloc(len); //malloc buffer
  fread(buf,len,1,fp); //read into buffer
  fclose(fp);

  cJSON *root = cJSON_Parse(buf);

  // Debug stuff
//   char * out = cJSON_Print(root);
//   printf("%s\n",out);
//   free(out);

  readJSONintoHierarchy(root,h1);
  
  cJSON_Delete(root);
  free(buf);
}


}
