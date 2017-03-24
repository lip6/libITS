#ifndef __OPTIONS_HH__
#define __OPTIONS_HH__

#include "its/ITSModel.hh"

namespace its {

  /** Consumes the options that are recognized in args, and treats them to build the Model.
   *  Options recognized by this options parser: 
   MANDATORY : -i Inputfile -t {CAMI|PROD|ROMEO|ITSXML|ETF|DVE} 
   OPTIONAL : --load-order {$f.json|$f.ord}      NB:extension of file is used to determine its type
   CONFIGURATION : --ddd|--sdd : privilege ddd or sdd in encoding. sdd is default
   For Scalar and circular : -ssD2, -ssDR, -ssDS   (default -ssD2 1)
  */
  bool handleInputOptions (std::vector<const char *> & argv, ITSModel & model);

  /** Prints a message on how to use these options on stderr */
  void usageInputOptions ();

  /** Consumes the options that are recognized in args, and treats them to configure libDDD
   *  Options recognized by this options parser: --no-garbage, --gc-threshold XXX (in kb), --fixpoint {BFS,DFS}
   */
  bool handleSDDOptions (std::vector<const char *> & argv, bool & with_garbage) ;

  /** Prints a message on how to use these options on stderr */
  void usageSDDOptions ();

  /** Set a new usage() function for use in case of options misuse. 
   *  Expected behavior is to print a bunch of help to stderr.
   */
  void setUsage( void (*usage) (void));
}

#endif
