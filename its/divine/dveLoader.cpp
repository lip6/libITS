#include "its/divine/dveLoader.hh"
#include "its/divine/dve2GAL.hh"
#include "its/divine/wibble/string.h"

// old  dlopen() style stuff
// #include <dlfcn.h>

using std::cerr;
using std::endl;

namespace its {


typedef GAL* (*createGAL_t) (void);

GAL * loadDve2Gal (Label inputff) {
  dve2GAL::dve2GAL loader;
  std::string modelName = wibble::str::basename(inputff); 
  loader.setModelName(modelName);
  loader.read( inputff.c_str() );
  loader.analyse();

  return loader.convertFromDve();
}



  /** Old g++ compiler reliant code was before :**/

//   compile(inputff);
//   vLabel objname = "./" + wibble::str::basename( inputff ) + ".so";

//   void *hndl = dlopen(objname.c_str(), RTLD_NOW);
//   if(hndl == NULL){
//     cerr << "Error during dlopen call: " << dlerror() << endl;
//     exit(-1);
//   }  
//   createGAL_t mkr = (createGAL_t) (dlsym(hndl, "createGAL") );
//   GAL * toret = (*mkr) ();
//   std::cout << "Model successfully loaded from binary object file " << objname << std::endl;
//   return toret;
  


  
// static void die( std::string bla ) 
// {
//   std::cerr << bla << std::endl;
//   exit( 1 );
// }


// static void run( std::string command ) {
//   int status = system( command.c_str() );
//   if ( status != -1 && WEXITSTATUS( status ) != 0 )
//     die( "Error running external command: " + command );

// }

// static void gplusplus( std::string in, std::string out, std::string flags = "" ) {
//   std::stringstream cmd;
//   std::string multiarch =
// #if defined(USE_GCC_M32)
//     "-m32 "
// #elif defined(USE_GCC_M64)
//     "-m64 "
// #else
//     ""
// #endif
//     ;
//   std::string os_dependant_flags =
// #if defined(__APPLE__)
//   "-bundle -flat_namespace -undefined suppress "
// #else
//   "-shared "
// #endif
//   ;
//   cmd << "g++ -O0 -fPIC " << os_dependant_flags << multiarch << flags << " -o " << out << " " << in;
//   std::cout << "Running compilation step :" << cmd.str() << std::endl;
//   run( cmd.str() );
// }
  
// void compile (Label inputff) {
//   divine::dve_compiler compiler(true);
//   std::string modelName = wibble::str::basename(inputff); 
//   compiler.setModelName(modelName);
//   compiler.read( inputff.c_str() );
//   compiler.analyse();
  
//   std::string outfile = wibble::str::basename( inputff ) + ".cpp";
//   std::ofstream out( outfile.c_str() );
//   compiler.setOutput( out );
//   compiler.print_generator();

//   std::cout << "Generated GAL model file :" << outfile << std::endl;
//   gplusplus( outfile, wibble::str::basename( inputff ) + ".so" , std::string("-g -I")+LIBDDD+" -I"+LIBITS+" ");
// }

} // namespace its
