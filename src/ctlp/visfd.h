#ifndef ___VIS__FILEDESCRIPTORS__HH__
#define ___VIS__FILEDESCRIPTORS__HH__

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <unistd.h>
#include "util.h"
#include "error.h"
#include "avl.h"
#include "var_set.h"
#include "array.h"
#include "st.h"
#include "list.h"
#include "graph.h"
#include "bdd.h"
#include "mdd.h"


extern FILE *vis_stderr;
extern FILE *vis_stdout;
extern FILE *vis_historyFile;

EXTERN void vis_init (void);

#endif
