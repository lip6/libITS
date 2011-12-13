#include "dveLoader.hh"
#include "divine/wibble/string.h"
#include <dlfcn.h>

using std::cerr;
using std::endl;

namespace its {


typedef GAL* (*createGAL_t) (void);

GAL * loadDve2Gal (Label inputff) {
  compile(inputff);
  vLabel objname = "./" + wibble::str::basename( inputff ) + ".so";

  void *hndl = dlopen(objname.c_str(), RTLD_NOW);
  if(hndl == NULL){
    cerr << "Error during dlopen call: " << dlerror() << endl;
    exit(-1);
  }  
  createGAL_t mkr = (createGAL_t) (dlsym(hndl, "createGAL") );
  return (*mkr) ();

}



  
static void die( std::string bla ) 
{
  std::cerr << bla << std::endl;
  exit( 1 );
}

static void die_help( std::string bla )
{
//  opts.outputHelp( std::cerr );
  die( bla );
}


static void run( std::string command ) {
  int status = system( command.c_str() );
#ifdef POSIX
  if ( status != -1 && WEXITSTATUS( status ) != 0 )
    die( "Error running external command: " + command );
#endif
}

static void gplusplus( std::string in, std::string out, std::string flags = "" ) {
  std::stringstream cmd;
  std::string multiarch =
#if defined(USE_GCC_M32)
    "-m32 "
#elif defined(USE_GCC_M64)
    "-m64 "
#else
    ""
#endif
    ;
  cmd << "g++ -O2 -shared -fPIC " << multiarch << flags << " -o " << out << " " << in;
  run( cmd.str() );
}
  
void compile (Label inputff) {
  divine::dve_compiler compiler(true);
  compiler.read( inputff.c_str() );
  compiler.analyse();
  
  std::string outfile = wibble::str::basename( inputff ) + ".cpp";
  std::ofstream out( outfile.c_str() );
  compiler.setOutput( out );
  compiler.print_generator();
  gplusplus( outfile, wibble::str::basename( inputff ) + ".so" , std::string("-g -I")+LIBDDD+" -I"+LIBITS);
}

} // namespace its
