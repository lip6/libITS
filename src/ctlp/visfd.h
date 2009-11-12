#ifndef ___VIS__FILEDESCRIPTORS__HH__
#define ___VIS__FILEDESCRIPTORS__HH__

#include <stdio.h>
#include "util.h"

extern FILE *vis_stderr;
extern FILE *vis_stdout;
extern FILE *vis_historyFile;

EXTERN void vis_init (void);

#endif
