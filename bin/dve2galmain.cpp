#include <iostream>
#include <fstream>
#include <cstring>

#include "its/Options.hh"
// The ITS model referential
#include "its/ITSModel.hh"
#include "ddd/SDD.h"
#include "ddd/MemoryManager.h"




int main_noex (int argc, char **argv) {
 // echo options of run
 std::cout << "dve2gal command run as :\n" << std::endl;
 for (int i=0;i < argc; i++) {
   std::cout << argv[i] << "  ";
 }
 std::cout << std::endl;

 if (argc < 2) {
   std::cerr << "Please provide a DVE file as input argument."<< std::endl;
   return 1;
 }

 vLabel pathinputff = argv[1];

 // instanciate a Model
 its::ITSModel model;

 Label modelName = model.declareDVEType(pathinputff);
 if (modelName != "") {
   model.setInstance(modelName, "main");
   model.setInstanceState("init");
 }

 vLabel galName = pathinputff;
 // 4 = ".dve" string length
 galName.replace(galName.find(".dve"),4,".gal");
 std::ofstream myfile;
 myfile.open (galName.c_str());
 myfile << * model.findType(modelName) << std::endl ;
 myfile.close();
 return 0;
}


int main (int argc, char **argv) { 

  // Reserve 16K of memory that can be deleted just in case we run out of memory
  char* _emergencyMemory = new char[16384];
  try {
    return main_noex (argc, argv);
  } catch (const char * ex) {
    std::cerr << "An unexpected exception occurred : " << ex << std::endl;
    return 1;
  } catch (std::string & err) {
    std::cerr << "An unexpected exception occurred : " << err << std::endl;
    return 1;
  } catch(std::bad_alloc & ex) {
    // Delete the reserved memory so we can print an error message before exiting
    delete[] _emergencyMemory;

    MemoryManager::pstats();
    std::cerr << "Out of memory error !";
    std::cin.get();
    exit(1);
  }

}
