#include <iostream>
#include <fstream>
#include <cstring>

#include "ddd/MemoryManager.h"

#include "gal/parser/GALParser.hh"
#include "gal/GAL2C.hh"



int main_noex (int argc, char **argv) {
 // echo options of run
 std::cout << "gal2c command run as :\n" << std::endl;
 for (int i=0;i < argc; i++) {
   std::cout << argv[i] << "  ";
 }
 std::cout << std::endl;

 if (argc < 2) {
   std::cerr << "Please provide a GAL file as input argument."<< std::endl;
   return 1;
 }

 vLabel pathinputff = argv[1];

 its::GAL* gal = its::GALParser::loadGAL(pathinputff);
 its::GAL2C gal2c(*gal);

 vLabel cName = pathinputff;
 size_t pos = cName.find_last_of("/\\");
 if (pos != std::string::npos)
   cName = "./" + cName.substr(pos+1);

 // 4 = ".gal" string length
 cName.replace(cName.find(".gal"),4,".cc");
 std::ofstream myfile;
 myfile.open (cName.c_str());
 gal2c.dump(myfile);
 myfile.close();
 delete gal;

 // compile the produced file
 std::string flags;
 if (argc < 3)
   flags = "-O3";
 else
   flags = argv[2];

 std::string cmd = "g++ -shared -fPIC -std=c++11 " + flags + " " + cName;
 cName.replace(cName.find(".cc"),3,".gal2C");
 cmd += " -o " + cName;
 // run the command
 int res = system(cmd.c_str());
 if (res)
   throw std::runtime_error("Execution of '" + cmd + "' returned exit code " + std::to_string(WEXITSTATUS(res)));

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
  } catch (std::string err) {
    std::cerr << "An unexpected exception occurred : " << err << std::endl;
    return 1;
  } catch(std::bad_alloc ex) {
    // Delete the reserved memory so we can print an error message before exiting
    delete[] _emergencyMemory;

    MemoryManager::pstats();
    std::cerr << "Out of memory error !";
    std::cin.get();
    exit(1);
  }

}
