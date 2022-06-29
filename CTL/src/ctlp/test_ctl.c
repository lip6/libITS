
#include "visfd.h"
#include "ctlp.h"
#include "ctlpInt.h"


int main (int argc, char ** argv) {  
  vis_init();
  return CommandCtlpTest(argc,argv);
}
