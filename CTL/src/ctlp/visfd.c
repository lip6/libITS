#include "visfd.h"


FILE*  vis_stdout     ; 
FILE*  vis_stderr     ;
FILE*  vis_historyFile;

void vis_init () {
 vis_stdout      = stdout; 
 vis_stderr      = stderr;
 vis_historyFile = NIL(FILE);

}
