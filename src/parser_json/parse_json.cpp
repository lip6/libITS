
/**
 * \author Yann TM, HONG Silien
 * \version 1.2
 * \date 09/01/2012
 * \brief Module de définition du parseur d'un fichier JSON.
 * Le format JSON permet de rendre configurable l'état initial d'un SDD selon un fichier de configuration au format JSON
 * ou l'utilisateur va décrire l'ordre de la hiérarchie de chaque place.
 * Ici nous utilisons un parseur BOOST JSON cJSON de Dave Gamble (see readme)
 */

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>

#include "cJSON.h"
#include "parse_json.hh"


#if !defined(__MINGW32__) 
    #include <sys/mman.h> 
    #include <sys/types.h>
#else 
    #include <windows.h> 
#endif 



namespace json {


static void * myMMap (void* addr, size_t len, int fd, off_t off) {
#ifdef __MINGW32__ 
    HANDLE handle; 
    if(addr != NULL ) 
        return NULL; 
   handle = CreateFileMapping( 
       (HANDLE)_get_osfhandle(fd),NULL, 
       PAGE_WRITECOPY,0,0,NULL 
   ); 
   if(handle != NULL) 
   { 
       addr = MapViewOfFile(handle,FILE_MAP_COPY,0,off,len); 
       CloseHandle(handle); 
   } 
   return addr; 
#else 
   void *toret = mmap(addr,len, PROT_READ, MAP_PRIVATE,fd,off); 
   if (toret == MAP_FAILED) return NULL;
   return toret;
#endif 
}

int MyMUnmap(void* addr, size_t size) 
{ 
#ifdef __MINGW32__ 
    UnmapViewOfFile(addr); 
    return 0; 
#else 
    return munmap(addr,size); 
#endif 
} 




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
 

    int fd;
    char *map;  

    fd = open(filename.c_str(), O_RDONLY);
    if (fd == -1) {
		perror(("Error opening JSON file "+ filename + "\n Quitting, sorry.").c_str());    
		exit(1);
    }
	size_t filesize = lseek(fd, 0, SEEK_END); 
    map = (char*) myMMap(0, filesize,  fd, 0);
    if (map == NULL) {
		close(fd);
		perror(("mmap error opening JSON file "+ filename + "\n Quitting, sorry.").c_str());		
		exit(1);
    }
	
	cJSON *root = cJSON_Parse(map);

	if (MyMUnmap(map, filesize) == -1) {
		perror("Error un-mmapping the file");
    }
    close(fd);
  // Debug stuff
//   char * out = cJSON_Print(root);
//   printf("%s\n",out);
//   free(out);

  readJSONintoHierarchy(root,h1);
  
  cJSON_Delete(root);
  
}


}
