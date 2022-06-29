/**CHeaderFile*****************************************************************

  FileName    [ctlpInt.h]

  PackageName [ctlp]

  Synopsis    [Declarations for internal use.]

  Author      [Gary York, Ramin Hojati, Tom Shiple, Adnan Aziz, Yuji Kukimoto,
               Jae-Young Jang, In-Ho Moon]

  Copyright   [Copyright (c) 1994-1996 The Regents of the Univ. of California.
  All rights reserved.

  Permission is hereby granted, without written agreement and without license
  or royalty fees, to use, copy, modify, and distribute this software and its
  documentation for any purpose, provided that the above copyright notice and
  the following two paragraphs appear in all copies of this software.

  IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
  DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
  OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
  CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
  FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN
  "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO PROVIDE
  MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.]

  Revision    [$Id: ctlpInt.h,v 1.26 2005/05/13 05:51:05 fabio Exp $]

******************************************************************************/

#ifndef _CTLPINT
#define _CTLPINT

#define MAX_LENGTH_OF_VAR_NAME 512 

#include <string.h>
#include "ctlp.h"
#include "st.h"


/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

/**Struct**********************************************************************

  Synopsis    [Tree representing a CTL formula.]

  Description [Data structure for a CTL formula.  A formula is a tree
  or a one-rooted DAG.  An internal vertex has one or two children,
  depending on the operator type: if the operator takes just one
  argument, then the left child holds the subformula, and the right
  child is NULL; if the operator takes two arguments, then both
  children are non-NULL.
  
  <p>A leaf is either a pair of a variable name in the network and a
  value, TRUE or FALSE.  The left and right fields are used in leaf
  vertices only in the first case above. (The left child holds a
  variable name while the right child holds a value. Note that these
  two fields are used differently in internal vertices and leaves.)

  <p>DbgInfo is an uninterpreted pointer used by the CTL debugger to store
  information needed to debug a formula.

  <p>States holds the exact satisfying set of a formula in some fsm (the fsm is
  not stored along with the set).  The Underapprox and overapprox fields are
  approximations of the states field. Either states is NIL (no exact result is
  known), or both underapprox and overapprox are NIL (the exact result is
  known, or nothing is known at all).

  Note that the convertedFlag being NIL does not imply that originalFormula is
  NIL: a conversion could have been done that was trivial for the particular
  node.]

******************************************************************************/
struct CtlpFormulaStruct {
  Ctlp_FormulaType type;
  Ctlp_Formula_t *left;   /* left child in formula */
  Ctlp_Formula_t *right;  /* right child, or null if not used */
  int refCount;           /* number of formulas referring to this
			     formula as a left or right child */
  Ctlp_Formula_t *forward;  /* store the forward version in translation */
  array_t *leaves;
  array_t *matchfound_top;    /*array of booleans*/
  array_t *matchelement_top;  /* array of match numbers*/
  array_t *matchfound_bot;    /*array of booleans*/
  array_t *matchelement_bot;  /* array of match numbers*/
  Ctlp_Parity_t negation_parity; /* negation parity */
  int share;
  struct {
    void *data;           /* used  to store information used by debugger */
    Ctlp_DbgInfoFreeFn freeFn;         /* free function for data */
    int convertedFlag;  /* was converted to existential form
                                           by non-trivial transformation */
    Ctlp_Formula_t *originalFormula; /* pointer to formula from which this
                                           was converted */
  } dbgInfo;
  /*
  ** The following fields are added for forward model checking.
  */
  short top; /* 0 : , 1 : forward */
  short compareValue; /* 0 : = false, 1 : != false */
  Ctlp_Approx_t latest;	  /* which of the three results is most recent */
  array_t *BotOnionRings;
  array_t *TopOnionRings;
};


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/**Variable********************************************************************

  Synopsis    [Global flag for parsing CTL file.]

  Description [This flag is set to 0 before parsing a CTL file commences.  If
  an error is found while parsing the file, this flag is set to 1.  After the
  parser returns, if this flag is set, then the global CTL formula array is
  freed.]

******************************************************************************/
extern int CtlpGlobalError;

/**Variable********************************************************************

  Synopsis    [Global pointer to the CTL formulas being created.]

  Description [This pointer is always set to the current formula
  so that the parser can free a partially constructed CTL formula
  when an error is detected. Every time the parser starts reasing
  a new formula, this variable is set to NIL(Ctlp_Fromula_t).]

******************************************************************************/
extern Ctlp_Formula_t *CtlpGlobalFormula;

/**Variable********************************************************************

  Synopsis    [Global pointer to the macro table of formulae]

  Description [This pointer is always set to the macro table so that 
  the parser can substitute macro string with defined formula]

******************************************************************************/
extern st_table *CtlpMacroTable;

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN void CtlpFormulaSetStatesToNULL(Ctlp_Formula_t *formula);
EXTERN void CtlpFormulaIncrementRefCount(Ctlp_Formula_t *formula);
EXTERN void CtlpFormulaDecrementRefCount(Ctlp_Formula_t *formula);
EXTERN void CtlpFormulaAddToGlobalArray(Ctlp_Formula_t * formula);
EXTERN void CtlpFormulaFree(Ctlp_Formula_t * formula);
EXTERN int CtlpStringChangeValueStrToBinString(char *value, char *binValueStr, int interval);
EXTERN void CtlpChangeBracket(char *inStr);
EXTERN char * CtlpGetVectorInfoFromStr(char *str, int *start, int *end, int *interval, int *inc);
EXTERN int CtlpFormulaAddToTable(char *name, Ctlp_Formula_t *formula, st_table *macroTable);
EXTERN Ctlp_Formula_t * CtlpFormulaFindInTable(char *name, st_table *macroTable);
EXTERN Ctlp_FormulaClass CtlpResolveClass(Ctlp_FormulaClass class1, Ctlp_FormulaClass class2);
EXTERN Ctlp_FormulaClass CtlpNegateFormulaClass(Ctlp_FormulaClass class_);

EXTERN int CommandCtlpTest(int  argc, char ** argv);

/**AutomaticEnd***************************************************************/

EXTERN void CtlpFileSetup(FILE *fp);  /* lives in a .l file, so not
                                                automatically generated */
EXTERN int CtlpYyparse(void);
#endif /* _CTLPINT */
