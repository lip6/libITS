/**CFile***********************************************************************

  FileName    [ctlpUtil.c]

  PackageName [ctlp]

  Synopsis    [Routines for manipulating CTL formulas.]

  Description [This file contains routines for accessing the fields of the CTL
  formula data structure, for printing CTL formulas, for reading CTL formulas
  from a file, and for converting formulas to the existential form.]

  Remarks [There are two types of conversion routines.  One set, for
  mc and amc, converts a formula to existential normal form, i.e. the
  AX, AU and AG operators are converted.  The other set, used in imc,
  converts to simple existential form, a form consisting only of
  E-operators and the boolean operators AND and NOT.

  Simple existential form is incompatible with the mc debug routines,
  because these depend on the specific form of the converted formula
  to decide what the original formula was.

  In the long run, the existential form should probably go, and the
  debug routines should be adapted.  The only drawback of simple
  existential form is that the XOR gets expanded into three nontrivial
  operations, so maybe XOR (or IFF) should be retained. (RB.)]


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

******************************************************************************/
#include "visfd.h"
#include <assert.h>

#include "ctlpInt.h"
#include "errno.h"

static char rcsid[] UNUSED = "$Id: ctlpUtil.c,v 1.71 2009/04/11 18:25:53 fabio Exp $";


/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

/**Variable********************************************************************

  Synopsis    [Array of CTL formulas (Ctlp_Formula_t) read from a file.]

  SeeAlso     [Ctlp_FormulaArrayFree]

******************************************************************************/
static array_t *globalFormulaArray;
static int	changeBracket = 1;

/* see ctlpInt.h for documentation */
int CtlpGlobalError;
Ctlp_Formula_t *CtlpGlobalFormula;
st_table *CtlpMacroTable;

/*---------------------------------------------------------------------------*/
/* Macro declarations                                                        */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/

static Ctlp_Formula_t * FormulaCreateWithType(Ctlp_FormulaType type);
static int FormulaCompare(const char *key1, const char *key2);
static int FormulaHash(char *key, int modulus);
static Ctlp_Formula_t * FormulaHashIntoUniqueTable(Ctlp_Formula_t *formula, st_table *uniqueTable);
static Ctlp_Formula_t * FormulaConvertToExistentialDAG(Ctlp_Formula_t *formula);
static void FormulaConvertToForward(Ctlp_Formula_t *formula, int compareValue);
static void FormulaInsertForwardCompareNodes(Ctlp_Formula_t *formula, Ctlp_Formula_t *parent, int compareValue);
static int FormulaGetCompareValue(Ctlp_Formula_t *formula);
static int FormulaIsConvertible(Ctlp_Formula_t *formula);
static Ctlp_Formula_t * ReplaceSimpleFormula(Ctlp_Formula_t *formula);

/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [Parses a file containing a set of CTL formulas.]

  Description [Parses a file containing a set of semicolon-ending CTL
  formulas, and returns an array of Ctlp_Formula_t representing those
  formulas.  If an error is detected while parsing the file, the routine frees
  any allocated memory and returns NULL.]

  SideEffects [Manipulates the global variables globalFormulaArray,
  CtlpGlobalError and CtlpGlobalFormula.]

  SeeAlso     [Ctlp_FormulaArrayFree Ctlp_FormulaPrint]

******************************************************************************/
array_t *
Ctlp_FileParseFormulaArray(
  FILE * fp)
{
  st_generator *stGen;
  char *name;
  Ctlp_Formula_t *formula;
  char *flagValue;
  /*
   * Initialize the global variables.
   */
  globalFormulaArray = array_alloc(Ctlp_Formula_t *, 0);
  CtlpGlobalError = 0;
  CtlpGlobalFormula = NIL(Ctlp_Formula_t);
  CtlpMacroTable = st_init_table(strcmp,st_strhash);
  CtlpFileSetup(fp);
  /*
   * Check if changing "[] -> <>" is disabled.
   */
  flagValue = "yes";
  // Cmd_FlagReadByName("ctl_change_bracket");
  if (flagValue != NIL(char)) {
    if (strcmp(flagValue, "yes") == 0)
      changeBracket = 1;
    else if (strcmp(flagValue, "no") == 0)
      changeBracket = 0;
    else {
      fprintf(vis_stderr,
	"** ctl error : invalid value %s for ctl_change_bracket[yes/no]. ***\n",
	flagValue);
    }
  }

  (void)CtlpYyparse();
  st_foreach_item(CtlpMacroTable,stGen,&name,&formula){
     FREE(name);
     CtlpFormulaFree(formula);
  }
  st_free_table(CtlpMacroTable);

  /*
   * Clean up if there was an error, otherwise return the array.
   */
  if (CtlpGlobalError){
    Ctlp_FormulaArrayFree(globalFormulaArray);
    return NIL(array_t);
  }
  else {
    return globalFormulaArray;
  }
}

/**Function********************************************************************

  Synopsis    [Returns formula as a character string.]

  Description [Returns formula as a character string. All subformulas are
  delimited by parenthesis. The syntax used is the same as used by the CTL
  parser.  Does nothing if passed a NULL formula.]

  SideEffects []

******************************************************************************/
char *
Ctlp_FormulaConvertToString(
  Ctlp_Formula_t * formula)
{
  char *s1        = NIL(char);
  char *s2        = NIL(char);
  char *tmpString = NIL(char);
  char *result;


  if (formula == NIL(Ctlp_Formula_t)) {
    return NIL(char);
  }

  /* The formula is a leaf. */
  if (formula->type == Ctlp_ID_c){
    return util_strcat3((char *)(formula->left), "",(char *)(formula->right));
  }

  /* If the formula is a non-leaf, the function is called recursively */
  s1 = Ctlp_FormulaConvertToString(formula->left);
  s2 = Ctlp_FormulaConvertToString(formula->right);

  switch(formula->type) {
    /*
     * The cases are listed in rough order of their expected frequency.
     */
    case Ctlp_OR_c:
      tmpString = util_strcat3(s1, " + ",s2);
      result    = util_strcat3("(", tmpString, ")");
      break;
    case Ctlp_AND_c:
      tmpString = util_strcat3(s1, " * ", s2);
      result    = util_strcat3("(", tmpString, ")");
      break;
    case Ctlp_THEN_c:
      tmpString = util_strcat3(s1, " -> ", s2);
      result    = util_strcat3("(", tmpString, ")");
      break;
    case Ctlp_XOR_c:
      tmpString = util_strcat3(s1, " ^ ", s2);
      result    = util_strcat3("(", tmpString, ")");
      break;
    case Ctlp_EQ_c:
      tmpString = util_strcat3(s1, " <-> ", s2);
      result    = util_strcat3("(", tmpString, ")");
      break;
    case Ctlp_NOT_c:
      tmpString = util_strcat3("(", s1, ")");
      result    = util_strcat3("!", tmpString, "");
      break;
    case Ctlp_EX_c:
      result = util_strcat3("EX(", s1, ")");
      FREE(s1);
      break;
    case Ctlp_EG_c:
      result = util_strcat3("EG(", s1, ")");
      break;
    case Ctlp_EF_c:
      result = util_strcat3("EF(", s1, ")");
      break;
    case Ctlp_EU_c:
      tmpString = util_strcat3("E(", s1, " U ");
      result    = util_strcat3(tmpString, s2, ")");
      break;
    case Ctlp_AX_c:
      result = util_strcat3("AX(", s1, ")");
      break;
    case Ctlp_AG_c:
      result = util_strcat3("AG(", s1, ")");
      break;
    case Ctlp_AF_c:
      result = util_strcat3("AF(", s1, ")");
      break;
    case Ctlp_AU_c:
      tmpString = util_strcat3("A(", s1, " U ");
      result    = util_strcat3(tmpString, s2, ")");
      break;
    case Ctlp_TRUE_c:
      result = util_strsav("TRUE");
      break;
    case Ctlp_FALSE_c:
      result = util_strsav("FALSE");
      break;
    case Ctlp_Init_c:
      result = util_strsav("Init");
      break;
    case Ctlp_Cmp_c:
      if (formula->compareValue == 0)
	tmpString = util_strcat3("[", s1, "] = ");
      else
	tmpString = util_strcat3("[", s1, "] != ");
      result    = util_strcat3(tmpString, s2, "");
      break;
    case Ctlp_FwdU_c:
      tmpString = util_strcat3("FwdU(", s1, ",");
      result    = util_strcat3(tmpString, s2, ")");
      break;
    case Ctlp_FwdG_c:
      tmpString = util_strcat3("FwdG(", s1, ",");
      result    = util_strcat3(tmpString, s2, ")");
      break;
    case Ctlp_EY_c:
      result = util_strcat3("EY(", s1, ")");
      break;
    default:
      fail("Unexpected type");
  }

  if (s1 != NIL(char)) {
    FREE(s1);
  }

  if (s2 != NIL(char)) {
    FREE(s2);
  }

  if (tmpString != NIL(char)) {
    FREE(tmpString);
  }

  return result;
}


/**Function********************************************************************

  Synopsis    [Prints a formula to a file.]

  Description [Prints a formula to a file. All subformulas are delimited by
  parenthesis. The syntax used is the same as used by the CTL parser.  Does
  nothing if passed a NULL formula.]

  SideEffects []

******************************************************************************/
void
Ctlp_FormulaPrint(
  FILE * fp,
  Ctlp_Formula_t * formula)
{
  char *tmpString;
  if (formula == NIL(Ctlp_Formula_t)) {
    return;
  }
  tmpString = Ctlp_FormulaConvertToString(formula);
  (void) fprintf(fp, "%s", tmpString);
  FREE(tmpString);
}


/**Function********************************************************************

  Synopsis    [Gets the type of a formula.]

  Description [Gets the type of a formula. See ctlp.h for all the types. It is
  an error to call this function on a NULL formula.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
Ctlp_FormulaType
Ctlp_FormulaReadType(
  Ctlp_Formula_t * formula)
{
  assert( formula != NIL(Ctlp_Formula_t) );
  return (formula->type);
}

#if 0
boolean
Ctlp_FormulaReadEvenNegations(
  Ctlp_Formula_t * formula)
{
  assert(formula != NIL(Ctlp_Formula_t));
  return (formula->even_negations);
}
#endif


/**Function********************************************************************

  Synopsis    [Gets the compare value of a formula.]

  Description [Gets the compare value of a formula.  It is an error to call
  this function on a NULL formula.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
int
Ctlp_FormulaReadCompareValue(
  Ctlp_Formula_t * formula)
{
  int value;

  assert( formula != NIL(Ctlp_Formula_t));
  value = formula->compareValue;
  return (value);
}


/**Function********************************************************************

  Synopsis    [Reads the variable name of a leaf formula.]

  Description [Reads the variable name of a leaf formula.
  It is an error to call this function on a non-leaf formula.]

  SideEffects []

******************************************************************************/
char *
Ctlp_FormulaReadVariableName(
  Ctlp_Formula_t * formula)
{
  if (formula->type != Ctlp_ID_c){
    fail("Ctlp_FormulaReadVariableName() was called on a non-leaf formula.");
  }
  return ((char *)(formula->left));
}


/**Function********************************************************************

  Synopsis    [Reads the value name of a leaf formula.]

  Description [Reads the value name of a leaf formula.
  It is an error to call this function on a non-leaf formula.]

  SideEffects []

******************************************************************************/
char *
Ctlp_FormulaReadValueName(
  Ctlp_Formula_t * formula)
{
  if (formula->type != Ctlp_ID_c){
    fail("Ctlp_FormulaReadValueName() was called on a non-leaf formula.");
  }
  return ((char *)(formula->right));
}

/**Function********************************************************************

  Synopsis    [Gets the left child of a formula.]

  Description [Gets the left child of a formula.  User must not free this
  formula. If a formula is a leaf formula, NIL(Ctlp_Formula_t) is returned.]

  SideEffects []

  SeeAlso     [Ctlp_FormulaReadRightChild]

******************************************************************************/
Ctlp_Formula_t *
Ctlp_FormulaReadLeftChild(
  Ctlp_Formula_t * formula)
{
  if (formula->type != Ctlp_ID_c){
    return (formula->left);
  }
  return NIL(Ctlp_Formula_t);
}


/**Function********************************************************************

  Synopsis    [Gets the right child of a formula.]

  Description [Gets the right child of a formula.  User must not free this
  formula. If a formula is a leaf formula, NIL(Ctlp_Formula_t) is returned.]

  SideEffects []

  SeeAlso     [Ctlp_FormulaReadLeftChild]

******************************************************************************/
Ctlp_Formula_t *
Ctlp_FormulaReadRightChild(
  Ctlp_Formula_t * formula)
{
  if (formula->type != Ctlp_ID_c){
    return (formula->right);
  }
  return NIL(Ctlp_Formula_t);
}




/**Function********************************************************************

  Synopsis    [Sets the debug information of a formula.]

  Description [Sets the debug information of a CTL formula.  The data is
  uninterpreted.  FreeFn is a pointer to a function that takes a formula as
  input and returns void.  FreeFn should free all the memory associated with
  the debug data; it is called when this formula is freed.]

  SideEffects []

  SeeAlso     [Ctlp_FormulaReadDebugData]

******************************************************************************/
void
Ctlp_FormulaSetDbgInfo(
  Ctlp_Formula_t * formula,
  void *data,
  Ctlp_DbgInfoFreeFn freeFn)
{
  if (formula->dbgInfo.data != NIL(void)){
    assert(formula->dbgInfo.freeFn != NULL);

    (*formula->dbgInfo.freeFn)(formula);
  }

  formula->dbgInfo.data   = data;
  formula->dbgInfo.freeFn = freeFn;
}


/**Function********************************************************************

  Synopsis    [Returns the debug data associated with a formula.]

  Description [Returns the debug data associated with a formula.  These data
  are uninterpreted by the ctlp package.]

  SideEffects []

  SeeAlso     [Ctlp_FormulaSetDbgInfo]

******************************************************************************/
void *
Ctlp_FormulaReadDebugData(
  Ctlp_Formula_t * formula)
{
  return formula->dbgInfo.data;
}


/**Function********************************************************************

  Synopsis    [Returns TRUE if formula was converted, else FALSE.]
  from AX/AG/AU/AF]

  Description [Returns TRUE if formula was converted from a formula of type
  AG, AX, AU, AF, or EF via a call to
  Ctlp_FormulaConvertToExistentialFormTree or
  Ctlp_FormulaConvertToExistentialFormDAG. Otherwise, returns FALSE.]

  SideEffects []

******************************************************************************/
int
Ctlp_FormulaTestIsConverted(
  Ctlp_Formula_t * formula)
{
  return formula->dbgInfo.convertedFlag;
}


/**Function********************************************************************

  Synopsis    [Returns TRUE if formula contains no path quantifiers.]

  Description [Test if a CTL formula has any path quantifiers in it;
  if so return false, else true.  For a CTL formula, being quantifier-free
  and being propositional are equivalent.]

  SideEffects []

******************************************************************************/
int
Ctlp_FormulaTestIsQuantifierFree(
  Ctlp_Formula_t *formula)
{
  int lCheck;
  int rCheck;
  Ctlp_Formula_t *leftChild;
  Ctlp_Formula_t *rightChild;
  Ctlp_FormulaType type;

  if ( formula == NIL( Ctlp_Formula_t ) ) {
    return TRUE;
  }

  type = Ctlp_FormulaReadType( formula );

  if ( ( type == Ctlp_ID_c ) ||
       ( type == Ctlp_TRUE_c ) ||
       ( type == Ctlp_FALSE_c ) ) {
    return TRUE;
  }

  if ( ( type !=  Ctlp_OR_c )   &&
       ( type !=  Ctlp_AND_c )  &&
       ( type !=  Ctlp_NOT_c )  &&
       ( type !=  Ctlp_THEN_c ) &&
       ( type !=  Ctlp_XOR_c )  &&
       ( type !=  Ctlp_EQ_c ) ) {
    return FALSE;
  }

  leftChild = Ctlp_FormulaReadLeftChild( formula );
  lCheck = Ctlp_FormulaTestIsQuantifierFree( leftChild );

  if (lCheck == FALSE)
    return FALSE;

  rightChild = Ctlp_FormulaReadRightChild( formula );
  rCheck = Ctlp_FormulaTestIsQuantifierFree( rightChild );

  return rCheck;
}


/**Function********************************************************************

  Synopsis    [Annotates the each node in parse tree with its negation
  parity.]

  Description [This function must be called at the top level with
  parity = Ctlp_Even_c.  It will then annotate each node with its parity.
  A node in a formula with sharing may have both parities and a descendant
  of a XOR or EQ node will have both.  Only backward CTL formulae are
  considered.]

  SideEffects [changes the negation_parity fields of all nodes in the formula]

******************************************************************************/
void
Ctlp_FormulaNegations(
  Ctlp_Formula_t * formula,
  Ctlp_Parity_t parity)
{

  Ctlp_FormulaType formulaType;
  Ctlp_Formula_t *leftChild;
  Ctlp_Formula_t *rightChild;
  Ctlp_Parity_t newparity;

  assert(formula != NIL(Ctlp_Formula_t));
  if (formula->negation_parity == parity)
    return; /* already done */
  if (formula->negation_parity != Ctlp_NoParity_c)
    parity = Ctlp_EvenOdd_c; /* reconverging paths with different parity */
  formula->negation_parity = parity;

  formulaType = Ctlp_FormulaReadType(formula);
  leftChild = Ctlp_FormulaReadLeftChild(formula);
  rightChild = Ctlp_FormulaReadRightChild(formula);

  switch (formulaType) {
  case Ctlp_AX_c:
  case Ctlp_AG_c:
  case Ctlp_AF_c:
  case Ctlp_EX_c:
  case Ctlp_EG_c:
  case Ctlp_EF_c:
    Ctlp_FormulaNegations(leftChild,parity);
    break;
  case Ctlp_AU_c:
  case Ctlp_EU_c:
  case Ctlp_OR_c:
  case Ctlp_AND_c:
    Ctlp_FormulaNegations(leftChild,parity);
    Ctlp_FormulaNegations(rightChild,parity);
    break;
  case Ctlp_NOT_c:
    if (parity == Ctlp_Even_c)
      newparity = Ctlp_Odd_c;
    else if (parity == Ctlp_Odd_c)
      newparity = Ctlp_Even_c;
    else newparity = Ctlp_EvenOdd_c;
    Ctlp_FormulaNegations(leftChild,newparity);
    break;
  case Ctlp_THEN_c:
    if (parity == Ctlp_Even_c)
      newparity = Ctlp_Odd_c;
    else if (parity == Ctlp_Odd_c)
      newparity = Ctlp_Even_c;
    else newparity = Ctlp_EvenOdd_c;
    Ctlp_FormulaNegations(leftChild,newparity);
    Ctlp_FormulaNegations(rightChild,parity);
    break;
  case Ctlp_XOR_c:
  case Ctlp_EQ_c:
    Ctlp_FormulaNegations(leftChild,Ctlp_EvenOdd_c);
    Ctlp_FormulaNegations(rightChild,Ctlp_EvenOdd_c);
    break;
  default:
      break;
  }
}


/**Function********************************************************************

  Synopsis    [Checks whether an ACTL formula is W-ACTL.]

  Description [Test if the binary operators in an ACTL formula have at
  least one propositional formula as one of the operands.  If not return
  Ctlp_NONWACTL_c, else return either Ctlp_WACTLsimple_c for propositional
  formulae or Ctlp_WACTLstate_c for formulae containing temporal operators.
  A W-ACTL formula must contain no exitential operators and no forward
  operators.  XOR and EQ are allowed only in propositional subformulae.]

  SideEffects [none]

******************************************************************************/
Ctlp_FormulaACTLSubClass
Ctlp_CheckIfWACTL(
  Ctlp_Formula_t *formula)
{
  Ctlp_FormulaType formulaType;
  Ctlp_Formula_t *rightFormula, *leftFormula;
  Ctlp_FormulaACTLSubClass resultLeft, resultRight;

  assert(formula != NIL(Ctlp_Formula_t));
  if(formula == NIL(Ctlp_Formula_t))
    return Ctlp_WACTLsimple_c;

  formulaType = Ctlp_FormulaReadType(formula);
  leftFormula = Ctlp_FormulaReadLeftChild(formula);
  rightFormula = Ctlp_FormulaReadRightChild(formula);

  /* Depending on the formula type, create result or recur. */
  switch (formulaType) {
  case Ctlp_AX_c:
  case Ctlp_AG_c:
  case Ctlp_AF_c:
    resultLeft = Ctlp_CheckIfWACTL(leftFormula);
    if (resultLeft == Ctlp_NONWACTL_c)
      return Ctlp_NONWACTL_c;
    else
      return Ctlp_WACTLstate_c;
    break;
  case Ctlp_AU_c:
    resultLeft = Ctlp_CheckIfWACTL(leftFormula);
    if (resultLeft == Ctlp_NONWACTL_c)
      return Ctlp_NONWACTL_c;
    resultRight = Ctlp_CheckIfWACTL(rightFormula);
    if (resultRight == Ctlp_NONWACTL_c)
      return Ctlp_NONWACTL_c;
    if (resultLeft == Ctlp_WACTLsimple_c || resultRight == Ctlp_WACTLsimple_c)
      return Ctlp_WACTLstate_c;
    else
      return Ctlp_NONWACTL_c;
    break;
  case Ctlp_OR_c:
  case Ctlp_AND_c:
  case Ctlp_THEN_c:
    resultLeft = Ctlp_CheckIfWACTL(leftFormula);
    if (resultLeft == Ctlp_NONWACTL_c) {
      return Ctlp_NONWACTL_c;
    } else {
      resultRight = Ctlp_CheckIfWACTL(rightFormula);
      if (resultRight == Ctlp_WACTLsimple_c) {
	return resultLeft;
      } else if (resultLeft == Ctlp_WACTLsimple_c) {
	return resultRight;
      } else {
	return Ctlp_NONWACTL_c;
      }
    }
    break;
  case Ctlp_NOT_c:
    resultLeft = Ctlp_CheckIfWACTL(leftFormula);
    return resultLeft;
    break;
  case Ctlp_XOR_c:
  case Ctlp_EQ_c:
    resultLeft = Ctlp_CheckIfWACTL(leftFormula);
    if (resultLeft != Ctlp_WACTLsimple_c) {
      return Ctlp_NONWACTL_c;
    } else {
      resultRight = Ctlp_CheckIfWACTL(rightFormula);
      if (resultRight == Ctlp_WACTLsimple_c)
	return Ctlp_WACTLsimple_c;
      else
	return Ctlp_NONWACTL_c;
    }
    break;
  case Ctlp_ID_c:
  case Ctlp_TRUE_c:
  case Ctlp_FALSE_c:
    return Ctlp_WACTLsimple_c;
    break;
  default:
    fprintf(vis_stderr, "#Ctlp Result : The formula should be in Universal operators!\n");
    return Ctlp_NONWACTL_c;
    break;
  }
} /* End of Ctlp_CheckTypeOfExistentialFormula */


/**Function********************************************************************

  Synopsis    [Returns original formula corresponding to converted formula.]

  SideEffects []

******************************************************************************/
Ctlp_Formula_t *
Ctlp_FormulaReadOriginalFormula(
  Ctlp_Formula_t * formula)
{
  return formula->dbgInfo.originalFormula;
}


/**Function********************************************************************

  Synopsis    [Frees a formula if no other formula refers to it as a
  sub-formula.]

  Description [The function decrements the refCount of the formula. As a
  consequence, if the refCount becomes 0, the formula is freed.]

  SideEffects []

  SeeAlso     [CtlpFormulaFree, CtlpDecrementRefCount]

******************************************************************************/
void
Ctlp_FormulaFree(
  Ctlp_Formula_t *formula)
{
  CtlpFormulaDecrementRefCount(formula);
}

/**Function********************************************************************

  Synopsis [Recursively frees  the debug data]

  Description []

  SideEffects []

  SeeAlso     [Ctlp_FormulaFree]

******************************************************************************/
void
Ctlp_FlushStates(
  Ctlp_Formula_t * formula)
{
  if (formula != NIL(Ctlp_Formula_t)) {

    if (formula->type != Ctlp_ID_c){
      if (formula->left  != NIL(Ctlp_Formula_t)) {
	Ctlp_FlushStates(formula->left);
      }
      if (formula->right != NIL(Ctlp_Formula_t)) {
	Ctlp_FlushStates(formula->right);
      }
    }

    if (formula->dbgInfo.data != NIL(void)){
      (*formula->dbgInfo.freeFn)(formula);
      formula->dbgInfo.data = NIL(void);
    }

  }

}

/**Function********************************************************************


  Synopsis    [Duplicates a CTL formula.]

  Description [Recursively duplicate a formula. Does nothing if the formula
  is NIL. Does not copy mdd for states, underapprox, overapprox, dbgInfo, and
  in general all the information that is used to annotate the formula.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
Ctlp_Formula_t *
Ctlp_FormulaDup(
  Ctlp_Formula_t * formula)
{
  Ctlp_Formula_t *result = NIL(Ctlp_Formula_t);

  if ( formula == NIL(Ctlp_Formula_t)) {
	return NIL(Ctlp_Formula_t);
  }

  result = ALLOC(Ctlp_Formula_t, 1);

  result->type                    = formula->type;
  result->forward                 = NIL(Ctlp_Formula_t);
  result->negation_parity         = Ctlp_NoParity_c;
  result->leaves                  = formula->leaves == NIL(array_t) ?
    NIL(array_t) : array_dup(formula->leaves);
  result->matchfound_top          = NIL(array_t);
  result->matchelement_top        = NIL(array_t);
  result->matchfound_bot          = NIL(array_t);
  result->matchelement_bot        = NIL(array_t);
  result->refCount                = 1;
  result->dbgInfo.data            = NIL(void);
  result->dbgInfo.freeFn          = (Ctlp_DbgInfoFreeFn) NULL;
  result->dbgInfo.convertedFlag   = FALSE;
  result->dbgInfo.originalFormula = NIL(Ctlp_Formula_t);
  result->top                     = 0;
  result->compareValue            = 0;

  if ( formula->type != Ctlp_ID_c )  {
    result->left                    = Ctlp_FormulaDup( formula->left );
    result->right                   = Ctlp_FormulaDup( formula->right );
  }
  else {
    result->left  = (Ctlp_Formula_t *) util_strsav((char *)formula->left );
    result->right = (Ctlp_Formula_t *) util_strsav((char *)formula->right );
  }
  result->share		  = 1;
  result->BotOnionRings   = NIL(array_t);
  result->TopOnionRings   = NIL(array_t);
  return result;
}

/**Function********************************************************************


  Synopsis    [Complements a CTL formula.]

  Description []

  SideEffects []

  SeeAlso     []

******************************************************************************/
Ctlp_Formula_t *
Ctlp_FormulaConverttoComplement(
  Ctlp_Formula_t * formula)
{
  Ctlp_Formula_t *result;

  if (formula == NIL(Ctlp_Formula_t)) {
	return NIL(Ctlp_Formula_t);
  }
  result = Ctlp_FormulaCreate(Ctlp_NOT_c,formula,NIL(Ctlp_Formula_t));
  CtlpFormulaIncrementRefCount(formula);
#if 0
  result = FormulaCreateWithType(Ctlp_NOT_c);
  result->left = FormulaCreateWithType(formula->type);
  result->left->left = formula->left;
  result->left->right = formula->right;
  switch (formula->type) {
  case Ctlp_ID_c:
  case Ctlp_TRUE_c:
  case Ctlp_FALSE_c:
  case Ctlp_Init_c:
    break;
  default:
    CtlpFormulaIncrementRefCount(formula->left);
    CtlpFormulaIncrementRefCount(formula->right);
    break;
  }
#endif
  return result;
} /* Ctlp_FormulaConverttoComplement */


/**Function********************************************************************


  Synopsis    [Converts AFp to A (TRUE) U p.]

  Description []

  SideEffects [Should be called only for AF formula]

  SeeAlso     []

******************************************************************************/
Ctlp_Formula_t *
Ctlp_FormulaConvertAFtoAU(
  Ctlp_Formula_t * formula)
{
  Ctlp_Formula_t *result;
  /* AFf --> (A true U f)  */
  assert(formula->type == Ctlp_AF_c);
  result = FormulaCreateWithType(Ctlp_AU_c);
  result->left  = FormulaCreateWithType(Ctlp_TRUE_c);
  result->right = formula->left;
  CtlpFormulaIncrementRefCount(formula->left);
  return result;
}


/**Function********************************************************************


  Synopsis    [Converts EFp to p + EX(EFp).]

  Description []

  SideEffects [Should be called only for EF formula]

  SeeAlso     []

******************************************************************************/
Ctlp_Formula_t *
Ctlp_FormulaConvertEFtoOR(
  Ctlp_Formula_t * formula)
{
  Ctlp_Formula_t *result;
  /* EFf --> f + EX(EF(f))  */
  assert(formula->type == Ctlp_EF_c);
  result = FormulaCreateWithType(Ctlp_OR_c);
  result->left = formula->left;
  result->right = FormulaCreateWithType(Ctlp_EX_c);
  result->right->left = formula;
  CtlpFormulaIncrementRefCount(formula->left);
  CtlpFormulaIncrementRefCount(formula);
  return result;
}


/**Function********************************************************************


  Synopsis    [Converts EpUq to q + p*EX(EpUq).]

  Description []

  SideEffects [Should be called only for EU formula]

  SeeAlso     []

******************************************************************************/
Ctlp_Formula_t *
Ctlp_FormulaConvertEUtoOR(
  Ctlp_Formula_t * formula)
{
  Ctlp_Formula_t *result;
  assert(formula->type == Ctlp_EU_c);
  /* E(f1)U(f2) --> f2 + f1*EX( E(f1)U(f2) )  */
  result = FormulaCreateWithType(Ctlp_OR_c);
  result->left = formula->right;
  result->right = FormulaCreateWithType(Ctlp_AND_c);
  result->right->left = formula->left;
  result->right->right = FormulaCreateWithType(Ctlp_EX_c);
  result->right->right->left = formula;
  CtlpFormulaIncrementRefCount(formula->left);
  CtlpFormulaIncrementRefCount(formula->right);
  CtlpFormulaIncrementRefCount(formula);
  return result;
}


/**Function********************************************************************


  Synopsis    [Convert p^q to !p*q + p*!q.]

  Description []

  SideEffects [Should be called only for XOR formula]

  SeeAlso     []

******************************************************************************/
Ctlp_Formula_t *
Ctlp_FormulaConvertXORtoOR(
  Ctlp_Formula_t * formula)
{
  Ctlp_Formula_t *result;
  assert(formula->type == Ctlp_XOR_c);
  result = FormulaCreateWithType(Ctlp_OR_c);
  result->left = FormulaCreateWithType(Ctlp_AND_c);
  result->right = FormulaCreateWithType(Ctlp_AND_c);
  result->left->left = Ctlp_FormulaConverttoComplement(formula->left);
  result->left->right = formula->right;
  result->right->left = formula->left;
  result->right->right = Ctlp_FormulaConverttoComplement(formula->right);
  CtlpFormulaIncrementRefCount(formula->left);
  CtlpFormulaIncrementRefCount(formula->right);
  return result;
}


/**Function********************************************************************


  Synopsis    [Converts p<->q to p*q + !p*!q.]

  Description []

  SideEffects [Should be called only for EQ formula]

  SeeAlso     []

******************************************************************************/
Ctlp_Formula_t *
Ctlp_FormulaConvertEQtoOR(
  Ctlp_Formula_t * formula)
{
  Ctlp_Formula_t *result;
  assert(formula->type == Ctlp_EQ_c);
  result = FormulaCreateWithType(Ctlp_OR_c);
  result->left = FormulaCreateWithType(Ctlp_AND_c);
  result->right = FormulaCreateWithType(Ctlp_AND_c);
  result->left->left = formula->left;
  result->left->right = formula->right;
  result->right->left = Ctlp_FormulaConverttoComplement(formula->left);
  result->right->right = Ctlp_FormulaConverttoComplement(formula->right);
  CtlpFormulaIncrementRefCount(formula->left);
  CtlpFormulaIncrementRefCount(formula->right);
  return result;
}


/**Function********************************************************************

  Synopsis    [Returns formula with a negation pushed down]

  Description [Returns formula with a negation pushed down. It is assumed that
	       the formula being passsed to it is not a proposition]

  SideEffects []

******************************************************************************/
Ctlp_Formula_t *
Ctlp_FormulaPushNegation(
  Ctlp_Formula_t * formula)
{
  Ctlp_Formula_t *result;
  if (formula == NIL(Ctlp_Formula_t)) {
    return NIL(Ctlp_Formula_t);
  }

  switch(formula->type) {
    /*
     * The cases are listed in rough order of their expected frequency.
     */
  case Ctlp_OR_c: /*!(f1 v f2) = !f1 * !f2*/
    result = FormulaCreateWithType(Ctlp_AND_c);
    result->left  = Ctlp_FormulaConverttoComplement(formula->left);
    result->right = Ctlp_FormulaConverttoComplement(formula->right);
    break;
  case Ctlp_AND_c: /*!(f1 * f2) = !f1 v !f2*/
    result = FormulaCreateWithType(Ctlp_OR_c);
    result->left  = Ctlp_FormulaConverttoComplement(formula->left);
    result->right = Ctlp_FormulaConverttoComplement(formula->right);
    break;
  case Ctlp_THEN_c: /*!(f1 -> f2) = !(!f1 v f2) = f1 * !f2 */
    result = FormulaCreateWithType(Ctlp_AND_c);
    result->left  = formula->left;
    CtlpFormulaIncrementRefCount(formula->left);
    result->right = Ctlp_FormulaConverttoComplement(formula->right);
    break;
  case Ctlp_XOR_c: /*!(f1 ^ f2) = f1 <-> f2*/
    result = FormulaCreateWithType(Ctlp_EQ_c);
    result->left  = formula->left;
    result->right = formula->right;
    CtlpFormulaIncrementRefCount(formula->left);
    CtlpFormulaIncrementRefCount(formula->right);
    break;
  case Ctlp_EQ_c: /*!(f1 <-> f2) = f1 ^ f2*/
    result = FormulaCreateWithType(Ctlp_XOR_c);
    result->left  = formula->left;
    result->right = formula->right;
    CtlpFormulaIncrementRefCount(formula->left);
    CtlpFormulaIncrementRefCount(formula->right);
    break;
  case Ctlp_NOT_c:/* !(!f1) = f1*/
    result = formula->left;
    CtlpFormulaIncrementRefCount(formula->left);
    break;
  case Ctlp_EX_c:/* !(EX(f1)) = AX(!f1)*/
    result = FormulaCreateWithType(Ctlp_AX_c);
    result->left  = Ctlp_FormulaConverttoComplement(formula->left);
    break;
  case Ctlp_EG_c: /*!(EG(f1)) = AF(!f1)*/
    result = FormulaCreateWithType(Ctlp_AF_c);
    result->left  = Ctlp_FormulaConverttoComplement(formula->left);
    break;
  case Ctlp_EF_c: /*!(EF(f1)) = AG(!f1)*/
    result = FormulaCreateWithType(Ctlp_AG_c);
    result->left  = Ctlp_FormulaConverttoComplement(formula->left);
    break;
  case Ctlp_EU_c: /* !(EpUq) = A(!q)U(!(p+q) + AG(!q))*/
    result = FormulaCreateWithType(Ctlp_AU_c);
    result->left  = Ctlp_FormulaConverttoComplement(formula->right);
    result->right = FormulaCreateWithType(Ctlp_OR_c);
    result->right->left = FormulaCreateWithType(Ctlp_NOT_c);
    result->right->left->left = FormulaCreateWithType(Ctlp_OR_c);
    result->right->left->left->left = formula->left;
    result->right->left->left->right = formula->right;
    result->right->right = FormulaCreateWithType(Ctlp_AG_c);
    result->right->right->left = Ctlp_FormulaConverttoComplement(formula->right);
    CtlpFormulaIncrementRefCount(formula->left);
    CtlpFormulaIncrementRefCount(formula->right);
    break;
  case Ctlp_AX_c:/* !AX(f1) = EX(!f1)*/
    result = FormulaCreateWithType(Ctlp_EX_c);
    result->left  = Ctlp_FormulaConverttoComplement(formula->left);
    break;
  case Ctlp_AG_c:/* !AG(f1) = EF(!f1) */
    result = FormulaCreateWithType(Ctlp_EF_c);
    result->left  = Ctlp_FormulaConverttoComplement(formula->left);
    break;
  case Ctlp_AF_c: /* !AF(f1) = EG(!f1)*/
    result = FormulaCreateWithType(Ctlp_EG_c);
    result->left  = Ctlp_FormulaConverttoComplement(formula->left);
    break;
  case Ctlp_AU_c: /* !(ApUq) = (E(!q)U(!(p+q)))+(EG!q)*/
    result = FormulaCreateWithType(Ctlp_OR_c);
    result->left = FormulaCreateWithType(Ctlp_EU_c);
    result->left->left  = Ctlp_FormulaConverttoComplement(formula->right);
    result->left->right = FormulaCreateWithType(Ctlp_NOT_c);
    result->left->right->left= FormulaCreateWithType(Ctlp_OR_c);
    result->left->right->left->left = formula->left;
    result->left->right->left->right = formula->right;
    result->right = FormulaCreateWithType(Ctlp_EG_c);
    result->right->left = Ctlp_FormulaConverttoComplement(formula->right);
    CtlpFormulaIncrementRefCount(formula->left);
    CtlpFormulaIncrementRefCount(formula->right);
    break;
  case Ctlp_TRUE_c:/* !TRUE = FALSE*/
    result = FormulaCreateWithType(Ctlp_FALSE_c);
    break;
  case Ctlp_FALSE_c:/*!FALSE = TRUE*/
    result = FormulaCreateWithType(Ctlp_TRUE_c);
    break;
  case Ctlp_Init_c: /* ???*/
  case Ctlp_Cmp_c: /* ???*/
    /* if (formula->compareValue == 0)
	tmpString = util_strcat3("[", s1, "] = ");
      else
	tmpString = util_strcat3("[", s1, "] != ");
      result    = util_strcat3(tmpString, s2, "");
      break;*/
  case Ctlp_FwdU_c: /* ???*/
  case Ctlp_FwdG_c:  /* ??? */
  case Ctlp_EY_c: /* ??? */
    result = formula;
    break;
  default:
      fail("Unexpected type");
  }

  return result;
}
/*****************************************************************************/

/**Function********************************************************************

  Synopsis    [Frees an array of CTL formulas.]

  Description [Calls CtlpFormulaDecrementRefCount on each formula in
  formulaArray, and then frees the array itself. It is okay to call this
  function with an empty array, but it is an error to call it with a NULL
  array.]

  SideEffects []

  SeeAlso     [Ctlp_FormulaFree]

******************************************************************************/
void
Ctlp_FormulaArrayFree(
  array_t * formulaArray /* of Ctlp_Formula_t */)
{
  int i;
  int numFormulas = array_n(formulaArray);

  assert(formulaArray != NIL(array_t));
  for (i = 0; i < numFormulas; i++) {
    Ctlp_Formula_t *formula = array_fetch(Ctlp_Formula_t *, formulaArray, i);

    CtlpFormulaDecrementRefCount(formula);
  }

  array_free(formulaArray);
}


Ctlp_Formula_t * Ctlp_ConvertToCmpFormula(Ctlp_Formula_t * formula) {

      Ctlp_Formula_t * andl = FormulaCreateWithType(Ctlp_AND_c);
      andl->left =  FormulaCreateWithType(Ctlp_Init_c);
      andl->right = formula;

      Ctlp_Formula_t * wrapped = FormulaCreateWithType(Ctlp_Cmp_c);
      wrapped->left = andl;
      wrapped->right = FormulaCreateWithType(Ctlp_FALSE_c);
      wrapped->compareValue = 1;
      
      return wrapped;
}

/**Function********************************************************************

  Synopsis    [Converts a CTL formula to existential form.]

  Description [Converts a CTL formula to existential form.  That is, all
  universal path quantifiers are replaced with the appropriate combination of
  existential quantifiers and Int negation.  Also converts "finally"
  operators to "until" operators.<p>

  <p>Returns a new formula that shares absolutely nothing with the original
  formula (not even the strings). Also, the new formula does not have any MDDs
  associated with it.  The "originalFormula" field of each new subformula is
  set to point to the formula passed as an argument.  In addition, if (and only
  if) the original formula is of type AG, AX, AU, AF, or EF, the "converted
  flag" is set. Returns NULL if called with a NULL formula.

  <p>Note: the debugger takes advantage of the exact way that the conversion is
  done.  Hence, you cannot play around much with the syntax of the converted
  formula.

  <p>A list of conversion rules shouls be included here (RB).

  <br> AG phi turns into !EU(true,! phi'), where the top ! has converted set
  and points to the AG, and phi' points to phi.

  <br> AU?

  <br> AX?

  <br> Any others?
  ]

  SideEffects []


  SeeAlso [Ctlp_FormulaArrayConvertToExistentialForm,
  Ctlp_ConvertToSimpleExistentialForm]

******************************************************************************/
Ctlp_Formula_t *
Ctlp_FormulaConvertToExistentialForm(
  Ctlp_Formula_t * formula)
{
  Ctlp_Formula_t *new_;
  char *variableNameCopy, *valueNameCopy;

  if (formula == NIL(Ctlp_Formula_t)) {
    return NIL(Ctlp_Formula_t);
  }

  /*
   * Recursively convert each subformula.
   */

  switch(formula->type) {
    case Ctlp_EF_c:
      /* EFf --> (E true U f)  */
      new_ = FormulaCreateWithType(Ctlp_EU_c);
      new_->left  = FormulaCreateWithType(Ctlp_TRUE_c);
      new_->right = Ctlp_FormulaConvertToExistentialForm(formula->left);
      new_->dbgInfo.convertedFlag = TRUE;
      break;

    case Ctlp_AX_c:
      /* AXf --> !(EX(!f)) */
      new_ = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left = FormulaCreateWithType(Ctlp_EX_c);
      new_->left->left = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left->left->left = Ctlp_FormulaConvertToExistentialForm(formula->left);
      new_->dbgInfo.convertedFlag = TRUE;
      break;

    case Ctlp_AG_c:
      /* AGf --> ![(E true U !f)] */
      new_ = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left = FormulaCreateWithType(Ctlp_EU_c);
      new_->left->left = FormulaCreateWithType(Ctlp_TRUE_c);
      new_->left->right = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left->right->left = Ctlp_FormulaConvertToExistentialForm(formula->left);
      new_->dbgInfo.convertedFlag = TRUE;
      break;

    case Ctlp_AF_c:
      /* AFf --> ![EG(!f)] */
      new_ = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left = FormulaCreateWithType(Ctlp_EG_c);
      new_->left->left = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left->left->left = Ctlp_FormulaConvertToExistentialForm(formula->left);
      new_->dbgInfo.convertedFlag = TRUE;
      break;

    case Ctlp_AU_c:
      /* A[fUg] --> !((E[!g U (!f*!g)]) + (EG!g)) */

      new_ = FormulaCreateWithType(Ctlp_NOT_c); /* top */
      new_->left = FormulaCreateWithType(Ctlp_OR_c); /* ((E[!g U (!f*!g)]) + (EG!g)) */

      new_->left->right = FormulaCreateWithType(Ctlp_EG_c); /* EG !g */
      new_->left->right->left = FormulaCreateWithType(Ctlp_NOT_c); /* !g */
      new_->left->right->left->left = Ctlp_FormulaConvertToExistentialForm(formula->right); /* g */

      new_->left->left = FormulaCreateWithType(Ctlp_EU_c); /* E[!g U (!f*!g)] */
      new_->left->left->left = FormulaCreateWithType(Ctlp_NOT_c); /* !g */
      new_->left->left->left->left = Ctlp_FormulaConvertToExistentialForm(formula->right); /* g */
      new_->left->left->right = FormulaCreateWithType(Ctlp_AND_c); /* !f*!g */
      new_->left->left->right->left = FormulaCreateWithType(Ctlp_NOT_c); /* !f */
      new_->left->left->right->left->left = Ctlp_FormulaConvertToExistentialForm(formula->left); /* f */
      new_->left->left->right->right = FormulaCreateWithType(Ctlp_NOT_c); /* !g */
      new_->left->left->right->right->left = Ctlp_FormulaConvertToExistentialForm(formula->right); /* g */
      new_->dbgInfo.convertedFlag = TRUE;
      break;

    case Ctlp_ID_c:
      /* Make a copy of the name, and create a new formula. */
      variableNameCopy = util_strsav((char *)(formula->left));
      valueNameCopy = util_strsav((char *)(formula->right));
      new_ = Ctlp_FormulaCreate(Ctlp_ID_c, variableNameCopy, valueNameCopy);
      break;

    case Ctlp_THEN_c:
    case Ctlp_EX_c:
    case Ctlp_EG_c:
    case Ctlp_EU_c:
    case Ctlp_OR_c:
    case Ctlp_AND_c:
    case Ctlp_NOT_c:
    case Ctlp_XOR_c:
    case Ctlp_EQ_c:
    case Ctlp_TRUE_c:
    case Ctlp_FALSE_c:
      /* These are already in the correct form.  Just convert subformulas. */
      new_ = FormulaCreateWithType(formula->type);
      new_->left = Ctlp_FormulaConvertToExistentialForm(formula->left);
      new_->right = Ctlp_FormulaConvertToExistentialForm(formula->right);
      break;

    default:
      fail("Unexpected type");
  }

  new_->dbgInfo.originalFormula = formula;
  return new_;
}

/**Function********************************************************************

  Synopsis    [Converts a CTL formula to simple existential form.]

  Description [Converts a CTL formula to simple existential form.
  That is, all universal path quantifiers are replaced with the
  appropriate combination of existential quantifiers and Int
  negation. Only `NOT' and `AND' Int operators are allowed.  Also
  converts "finally" operators to "until" operators.

  The converted bit is set in the converted formula only when temporal
  operators are converted, not when int operators are converted.

  A  new formula is created that needs to be freed seperately.]

  SideEffects []

  Remarks     [Ctl formulas in simple existential form are incompatible
  with the mc debug routines.  The reason is that the debug routines
  count on a very specific form of the converted formula to conclude
  what the original formula was.]

  SeeAlso     [Ctlp_FormulaArrayConvertToExistentialForm]

******************************************************************************/
Ctlp_Formula_t *
Ctlp_FormulaConvertToSimpleExistentialForm(
  Ctlp_Formula_t * formula)
{
  Ctlp_Formula_t *new_;
  char *variableNameCopy, *valueNameCopy;

  if (formula == NIL(Ctlp_Formula_t)) {
    return NIL(Ctlp_Formula_t);
  }

  /*
   * Recursively convert each subformula.
   */

  switch(formula->type) {
    case Ctlp_EF_c:
      /* EFf --> (E true U f)  */
      new_ = FormulaCreateWithType(Ctlp_EU_c);
      new_->left  = FormulaCreateWithType(Ctlp_TRUE_c);
      new_->right = Ctlp_FormulaConvertToSimpleExistentialForm(formula->left);
      new_->dbgInfo.convertedFlag = TRUE;
      break;

    case Ctlp_AX_c:
      /* AXf --> !(EX(!f)) */
      new_ = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left = FormulaCreateWithType(Ctlp_EX_c);
      new_->left->left = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left->left->left =
	Ctlp_FormulaConvertToSimpleExistentialForm(formula->left);
      new_->dbgInfo.convertedFlag = TRUE;
      break;

    case Ctlp_AG_c:
      /* AGf --> ![(E true U !f)] */
      new_ = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left = FormulaCreateWithType(Ctlp_EU_c);
      new_->left->left = FormulaCreateWithType(Ctlp_TRUE_c);
      new_->left->right = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left->right->left =
	Ctlp_FormulaConvertToSimpleExistentialForm(formula->left);
      new_->dbgInfo.convertedFlag = TRUE;
      break;

    case Ctlp_AF_c:
      /* AFf --> ![EG(!f)] */
      new_ = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left = FormulaCreateWithType(Ctlp_EG_c);
      new_->left->left = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left->left->left = Ctlp_FormulaConvertToSimpleExistentialForm(formula->left);
      new_->dbgInfo.convertedFlag = TRUE;
      break;

    case Ctlp_AU_c:
      /* A[fUg] --> !(E[!g U (!f*!g)]) * !(EG!g)) */

      new_ = FormulaCreateWithType(Ctlp_AND_c); /* top */
      new_->left = FormulaCreateWithType(Ctlp_NOT_c);
      new_->right = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left->left = FormulaCreateWithType(Ctlp_EG_c); /* EG !g */
      new_->left->left->left = FormulaCreateWithType(Ctlp_NOT_c); /* !g */
      new_->left->left->left->left =
	Ctlp_FormulaConvertToSimpleExistentialForm(formula->right); /* g */

      new_->right->left = FormulaCreateWithType(Ctlp_EU_c); /* E[!g U (!f*!g)] */
      new_->right->left->left = FormulaCreateWithType(Ctlp_NOT_c); /* !g */
      new_->right->left->left->left =
	Ctlp_FormulaConvertToSimpleExistentialForm(formula->right); /* g */
      new_->right->left->right = FormulaCreateWithType(Ctlp_AND_c); /* !f*!g */
      new_->right->left->right->left = FormulaCreateWithType(Ctlp_NOT_c); /* !f */
      new_->right->left->right->left->left =
	Ctlp_FormulaConvertToSimpleExistentialForm(formula->left); /* f */
      new_->right->left->right->right = FormulaCreateWithType(Ctlp_NOT_c); /* !g */
      new_->right->left->right->right->left =
	Ctlp_FormulaConvertToSimpleExistentialForm(formula->right); /* g */
      new_->dbgInfo.convertedFlag = TRUE;
      break;

    case Ctlp_ID_c:
      /* Make a copy of the name, and create a new formula. */
      variableNameCopy = util_strsav((char *)(formula->left));
      valueNameCopy = util_strsav((char *)(formula->right));
      new_ = Ctlp_FormulaCreate(Ctlp_ID_c, variableNameCopy, valueNameCopy);
      break;

      /* RB: I think THEN (and also OR, and maybe some others) should also have converted set. */
    case Ctlp_THEN_c:
      /* p->q --> !(p * !q) */
      new_ = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left = FormulaCreateWithType(Ctlp_AND_c);
      new_->left->left =
	Ctlp_FormulaConvertToSimpleExistentialForm(formula->left);
      new_->left->right = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left->right->left =
	Ctlp_FormulaConvertToSimpleExistentialForm(formula->right);
      break;

    case Ctlp_EX_c:
    case Ctlp_EG_c:
    case Ctlp_EU_c:
    case Ctlp_AND_c:
    case Ctlp_NOT_c:
    case Ctlp_TRUE_c:
    case Ctlp_FALSE_c:
      /* These are already in the correct form.  Just convert subformulas. */
      new_ = FormulaCreateWithType(formula->type);
      new_->left = Ctlp_FormulaConvertToSimpleExistentialForm(formula->left);
      new_->right = Ctlp_FormulaConvertToSimpleExistentialForm(formula->right);
      break;

    case Ctlp_OR_c:
      /* p + q --> !(!p * !q) */
      new_ = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left = FormulaCreateWithType(Ctlp_AND_c);
      new_->left->left = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left->right = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left->left->left =
	Ctlp_FormulaConvertToSimpleExistentialForm(formula->left);
      new_->left->right->left =
	Ctlp_FormulaConvertToSimpleExistentialForm(formula->right);
      break;
    case Ctlp_XOR_c:
      /* p ^ q --> !(p * q) * !(!p * !q) */
      new_ = FormulaCreateWithType(Ctlp_AND_c);
      new_->left = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left->left = FormulaCreateWithType(Ctlp_AND_c);
      new_->left->left->left =
	Ctlp_FormulaConvertToSimpleExistentialForm(formula->left);
      new_->left->left->right =
	Ctlp_FormulaConvertToSimpleExistentialForm(formula->right);
      new_->right = FormulaCreateWithType(Ctlp_NOT_c);
      new_->right->left = FormulaCreateWithType(Ctlp_AND_c);
      new_->right->left->left =  FormulaCreateWithType(Ctlp_NOT_c);
      new_->right->left->left->left =
	Ctlp_FormulaConvertToSimpleExistentialForm(formula->left);
      new_->right->left->right = FormulaCreateWithType(Ctlp_NOT_c);
      new_->right->left->right->left =
	Ctlp_FormulaConvertToSimpleExistentialForm(formula->right);
      break;

    case Ctlp_EQ_c:
      /* p <-> q --> !(p * !q) * !(!p * q) */
      new_ = FormulaCreateWithType(Ctlp_AND_c);
      new_->left = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left->left = FormulaCreateWithType(Ctlp_AND_c);
      new_->left->left->left =
	Ctlp_FormulaConvertToSimpleExistentialForm(formula->left);
      new_->left->left->right = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left->left->right->left =
	Ctlp_FormulaConvertToSimpleExistentialForm(formula->right);
      new_->right = FormulaCreateWithType(Ctlp_NOT_c);
      new_->right->left = FormulaCreateWithType(Ctlp_AND_c);
      new_->right->left->left =  FormulaCreateWithType(Ctlp_NOT_c);
      new_->right->left->left->left =
	Ctlp_FormulaConvertToSimpleExistentialForm(formula->left);
      new_->right->left->right =
	Ctlp_FormulaConvertToSimpleExistentialForm(formula->right);
      break;

    default:
      fail("Unexpected type");
  }

  new_->dbgInfo.originalFormula = formula;
  return new_;
}

/**Function********************************************************************

  Synopsis    [Converts an array of CTL formulas to existential form.]

  Description [Calls Ctlp_FormulaConvertToExistentialForm on each formula. It
  is okay to call this function with an empty array (in which case an empty
  array is returned), but it is an error to call it with a NULL array.

  All formulas in the array are copied, and hence Ctlp_FormulaArrayFree() needs
  to be called on the result at the end.]

  SideEffects []

  SeeAlso [Ctlp_FormulaConvertToExistentialForm]

******************************************************************************/
array_t *
Ctlp_FormulaArrayConvertToExistentialFormTree(
  array_t * formulaArray /* of Ctlp_Formula_t  */)
{
  int i;
  int numFormulas = array_n(formulaArray);
  array_t *convertedArray = array_alloc(Ctlp_Formula_t *, numFormulas);

  assert(formulaArray != NIL(array_t));

  for (i = 0; i < numFormulas; i++) {
    Ctlp_Formula_t *formula          = array_fetch(Ctlp_Formula_t *, formulaArray, i);
    Ctlp_Formula_t *convertedFormula = Ctlp_FormulaConvertToExistentialForm(formula);

    array_insert(Ctlp_Formula_t *, convertedArray, i, convertedFormula);
  }

  return convertedArray;
}

/**Function********************************************************************

  Synopsis    [Converts an array of CTL formulas to simple existential form.]

  Description [Calls Ctlp_FormulaConvertToSimpleExistentialForm on
  each formula.  It is okay to call this function with an empty array
  (in which case an empty array is returned), but it is an error to
  call it with a NULL array.]

  SideEffects []

  SeeAlso     [Ctlp_FormulaConvertToExistentialForm]

******************************************************************************/
array_t *
Ctlp_FormulaArrayConvertToSimpleExistentialFormTree(
  array_t * formulaArray /* of Ctlp_Formula_t  */)
{
  int i;
  int numFormulas;
  array_t *convertedArray;

  assert( formulaArray != NIL(array_t));

  numFormulas = array_n(formulaArray);
  convertedArray = array_alloc(Ctlp_Formula_t *, numFormulas);

  for (i = 0; i < numFormulas; i++) {
    Ctlp_Formula_t *formula;
    Ctlp_Formula_t *convertedFormula;

    formula = array_fetch(Ctlp_Formula_t *, formulaArray, i);
    convertedFormula = Ctlp_FormulaConvertToSimpleExistentialForm(formula);

    array_insert(Ctlp_Formula_t *, convertedArray, i, convertedFormula);
  }

  return convertedArray;
}

/**Function********************************************************************

  Synopsis    [Converts an array of CTL formulae to a multi-rooted DAG.]

  Description [The function hashes each subformula of a formula
  (including the formula itself) into a uniqueTable.  It returns an
  array containing the roots of the multi-rooted DAG thus created by
  the sharing of the subformulae.  It is okay to call this function
  with an empty array (in which case an empty array is returned), but
  it is an error to call it with a NULL array.]

  SideEffects [A formula in formulaArray might be freed if it had been
  encountered as a subformula of some other formula. Other formulae in
  formulaArray might be present in the returned array. Therefore, the
  formulae in formulaArray should not be freed. Only formulaArray
  itself should be freed.



  RB:  What does that mean?

  I understand this as follows.  Copies of the formulae are not made,
  but rather pointers to the argument subformulae are kept.  Hence, not only
  should the formulae in the result not be freed, but also you cannot free the
  argument before you are done with the result.  Furthermore, by invocation of
  this function, the argument gets altered.  It is still valid, but pointers
  may have changed.  Is this correct?


  RB rewrite: A formula in formulaArray is freed if it is encountered as a
  subformula of some other formula. Other formulae in formulaArray might be
  present in the returned array. Therefore, the formulae in formulaArray should
  not be freed. Only formulaArray itself should be freed.]

  SeeAlso     []

******************************************************************************/
array_t *
Ctlp_FormulaArrayConvertToDAG(
  array_t *formulaArray)
{
  int i;
  Ctlp_Formula_t *formula, *uniqueFormula;
  st_table *uniqueTable = st_init_table(FormulaCompare, FormulaHash);
  int numFormulae = array_n(formulaArray);
  array_t *rootsOfFormulaDAG = array_alloc(Ctlp_Formula_t *, numFormulae);

  assert(formulaArray != NIL(array_t));

  for(i=0; i < numFormulae; i++) {
    formula = array_fetch(Ctlp_Formula_t *, formulaArray, i);
    uniqueFormula = FormulaHashIntoUniqueTable(formula, uniqueTable);
    if(uniqueFormula != formula) {
      CtlpFormulaDecrementRefCount(formula);
      CtlpFormulaIncrementRefCount(uniqueFormula);
      array_insert(Ctlp_Formula_t *, rootsOfFormulaDAG, i, uniqueFormula);
    }
    else
      array_insert(Ctlp_Formula_t *, rootsOfFormulaDAG, i, formula);
  }
  st_free_table(uniqueTable);
  return rootsOfFormulaDAG;
}

/**Function********************************************************************

  Synopsis    [Converts a DAG of CTL formulae to a DAG of existential CTL
  formulae.]

  Description [The function converts a DAG of CTL formulae to a DAG of
  existential CTL formulae. The function recursively converts each
  subformula of each of the formulae in the DAG and remembers the
  converted formula in the field states. It is okay to call this
  function with an empty array (in which case an empty array is
  returned), but it is an error to call it with a NULL array.

  The states fileds in the formula need to be NULL.]

  SideEffects []

  SeeAlso     [FormulaConvertToExistentialDAG]

******************************************************************************/
array_t *
Ctlp_FormulaDAGConvertToExistentialFormDAG(
  array_t *formulaDAG)
{
  int i;
  Ctlp_Formula_t *formula;
  int numFormulae = array_n(formulaDAG);
  array_t *existentialFormulaDAG = array_alloc(Ctlp_Formula_t *, numFormulae);

  assert( formulaDAG != NIL(array_t));

  for(i=0; i<numFormulae; i++) {
    formula = array_fetch(Ctlp_Formula_t *, formulaDAG, i);
    array_insert(Ctlp_Formula_t *, existentialFormulaDAG, i,
		 FormulaConvertToExistentialDAG(formula));
  }
  for(i=0; i<numFormulae; i++) {
    formula = array_fetch(Ctlp_Formula_t *, formulaDAG, i);
    CtlpFormulaSetStatesToNULL(formula);
  }
  return existentialFormulaDAG;
}


/**Function********************************************************************

  Synopsis    [Creates a CTL formula with the specified fields.]

  Description [Allocates a Ctlp_Formula_t, and sets the 2 fields given as
  arguments.  If the type is Ctlp_ID_c, then the left and right fields
  should contain a pointer to a variable name and a pointer to a value
  respectively. Otherwise, the two fields point to subformulas. refCount is
  set to 1. The states field is set to NULL, the converted flag is set to
  FALSE, and the originalFormula field is set to NULL.]

  Comment     []

  SideEffects []

  SeeAlso     [CtlpFormulaDecrementRefCount]

******************************************************************************/
Ctlp_Formula_t *
Ctlp_FormulaCreate(
  Ctlp_FormulaType  type,
  void * left,
  void * right)
{
  Ctlp_Formula_t *formula = ALLOC(Ctlp_Formula_t, 1);

  formula->type                    = type;
  formula->left                    = (Ctlp_Formula_t *)left;
  formula->right                   = (Ctlp_Formula_t *)right;
  formula->refCount                = 1;
  formula->forward                 = NIL(Ctlp_Formula_t);
  formula->leaves		   = NIL(array_t);
  formula->matchfound_top          = NIL(array_t);
  formula->matchelement_top        = NIL(array_t);
  formula->matchfound_bot          = NIL(array_t);
  formula->matchelement_bot        = NIL(array_t);
  formula->negation_parity         = Ctlp_NoParity_c;
  formula->share                   = 1;
  formula->dbgInfo.data            = NIL(void);
  formula->dbgInfo.freeFn          = (Ctlp_DbgInfoFreeFn) NULL;
  formula->dbgInfo.convertedFlag   = FALSE;
  formula->dbgInfo.originalFormula = NIL(Ctlp_Formula_t);
  formula->top                     = 0;
  formula->compareValue            = 0;
  formula->latest                  = Ctlp_Incomparable_c;
  formula->BotOnionRings           = NIL(array_t);
  formula->TopOnionRings           = NIL(array_t);

  return formula;
}


/**Function********************************************************************

  Synopsis    [Creates a CTL formula with disjunction of atomic propositions]

  Description [This function returns Ctlp_Formula_t for name = {v1,v2,...}.
  In case of failure, a NULL pointer is returned.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
Ctlp_Formula_t *
Ctlp_FormulaCreateOr(
  char *name,
  char *valueStr)
{
  Ctlp_Formula_t *formula, *tempFormula;
  char *preStr;

  preStr = strtok(valueStr,",");
  formula = Ctlp_FormulaCreate(Ctlp_ID_c, util_strsav(name),
			       util_strsav(preStr));
  if (formula == NIL(Ctlp_Formula_t)) {
     return NIL(Ctlp_Formula_t);
  }
  while ((preStr = strtok(NIL(char),",")) != NIL(char)) {
    tempFormula = Ctlp_FormulaCreate(Ctlp_ID_c, util_strsav(name),
				     util_strsav(preStr));
    if (tempFormula == NIL(Ctlp_Formula_t)) {
      Ctlp_FormulaFree(formula);
      return NIL(Ctlp_Formula_t);
    }
    formula = Ctlp_FormulaCreate(Ctlp_OR_c,formula,tempFormula);
  }
  return formula;
}


/**Function********************************************************************

  Synopsis    [Creates a CTL formula with conjunction of atomic propositions]

  Description [This function returns Ctlp_Formula_t for nameVector = value,
  nameVector is a form of var[i:j] or var<j:j> and value is any integer n or
  binary string starting with 'b', for instance, b0011. If n is not
  fitted in var[i:j] or size of binary string is not matched with
  var[i:j], NULL pointer is returned]

  SideEffects []

  SeeAlso     []

******************************************************************************/
Ctlp_Formula_t *
Ctlp_FormulaCreateVectorAnd(
  char *nameVector,
  char *value)
{
  Ctlp_Formula_t *formula, *tempFormula;
  int startValue, endValue, inc, i,j, interval,startInd;
  char *binValueStr, *varName, *name;
  char *bitValue;

  varName = CtlpGetVectorInfoFromStr(nameVector,&startValue,&endValue,&interval,&inc);
  binValueStr = ALLOC(char,interval+1);
  if (!CtlpStringChangeValueStrToBinString(value,binValueStr,interval) ){
    FREE(varName);
    FREE(binValueStr);
    return NIL(Ctlp_Formula_t);
  }

  name = ALLOC(char,MAX_LENGTH_OF_VAR_NAME);
  (void) sprintf(name,"%s[%d]",varName,startValue);
  (void) CtlpChangeBracket(name);

  bitValue = ALLOC(char,3);
  bitValue[0] = '=';
  bitValue[1] = binValueStr[0];
  bitValue[2] = '\0';

  formula = Ctlp_FormulaCreate(Ctlp_ID_c, util_strsav(name),
			       util_strsav(bitValue));
  j = 0;
  startInd = startValue;
  for(i=startValue;i!=endValue;i=i+inc){
    startInd += inc;
    j++;
    (void) sprintf(name,"%s[%d]",varName,startInd);
    (void) CtlpChangeBracket(name);
    bitValue[1] = binValueStr[j];
    tempFormula = Ctlp_FormulaCreate(Ctlp_ID_c, util_strsav(name),
				     util_strsav(bitValue));
    formula = Ctlp_FormulaCreate(Ctlp_AND_c,formula,tempFormula);
  }
  FREE(varName);
  FREE(name);
  FREE(binValueStr);
  FREE(bitValue);
  return formula;
}


/**Function********************************************************************

  Synopsis    [Creates a CTL formula with OR of Vector AND]

  Description [This function returns Ctlp_Formula_t for nameVector = valueStr,
  nameVector is a form of var[i:j] or var<j:j>, valueStr is "k,l,...,q",
  and k,l,...,q is either integer or binary string.  Binary string starting
  with 'b', for instance, b0011. If n is not fitted in var[i:j] or size of
  binary string is not matched with var[i:j], NULL pointer is returned]

  SideEffects []

  SeeAlso     []

******************************************************************************/
Ctlp_Formula_t *
Ctlp_FormulaCreateVectorOr(
  char *nameVector,
  char *valueStr)
{
  Ctlp_Formula_t *formula,*tempFormula;
  char *preStr;

  preStr = strtok(valueStr,",");
  formula = Ctlp_FormulaCreateVectorAnd(nameVector,preStr);
  if ( formula == NIL(Ctlp_Formula_t)){
    return NIL(Ctlp_Formula_t);
  }
  while( (preStr = strtok(NIL(char),",")) != NIL(char) ){
    tempFormula = Ctlp_FormulaCreateVectorAnd(nameVector,preStr);
    if ( tempFormula == NIL(Ctlp_Formula_t)){
      Ctlp_FormulaFree(formula);
      return NIL(Ctlp_Formula_t);
    }
    formula = Ctlp_FormulaCreate(Ctlp_OR_c,formula,tempFormula);
  }
  return formula;
}


/**Function********************************************************************

  Synopsis    [Creates a CTL formula for equivalent property]

  Description [This function assumes that each child is defined in binary
  domain. Enumerate type is not supported]

  SideEffects []

  SeeAlso     []

******************************************************************************/
Ctlp_Formula_t *
Ctlp_FormulaCreateEquiv(
   char *left,
   char *right)
{
   Ctlp_Formula_t *formula,*formula1,*formula2;

   char *one;
   char *zero;

   one = "=1";
   zero = "=0";

   formula1 = Ctlp_FormulaCreate(Ctlp_ID_c, util_strsav(left),
				 util_strsav(one));
   formula2 = Ctlp_FormulaCreate(Ctlp_ID_c, util_strsav(right),
				 util_strsav(zero));
   formula = Ctlp_FormulaCreate(Ctlp_XOR_c,formula1,formula2);
   return formula;
}

/**Function********************************************************************

  Synopsis    [Creates a CTL formula for equivalent of vector]

  Description [This function returns a formula, which is the AND of a bitwise
  equivalence]

  SideEffects []

  SeeAlso     []

******************************************************************************/
Ctlp_Formula_t *
Ctlp_FormulaCreateVectorEquiv(
  char *left,
  char *right)
{
  Ctlp_Formula_t *formula,*tempFormula;
  char *leftName,*rightName;
  char *leftVar, *rightVar;
  int  leftStart,leftEnd,rightStart,rightEnd,leftInterval,rightInterval;
  int  leftInc,rightInc,leftInd,rightInd,i;

  leftName  = CtlpGetVectorInfoFromStr(left,&leftStart,&leftEnd,&leftInterval,&leftInc);
  rightName = CtlpGetVectorInfoFromStr(right,&rightStart,&rightEnd,&rightInterval,&rightInc);
  if (leftInterval != rightInterval){
     return NIL(Ctlp_Formula_t);
  }
  leftVar = ALLOC(char,MAX_LENGTH_OF_VAR_NAME);
  (void) sprintf(leftVar,"%s[%d]",leftName,leftStart);
  (void) CtlpChangeBracket(leftVar);
  rightVar = ALLOC(char,MAX_LENGTH_OF_VAR_NAME);
  (void) sprintf(rightVar,"%s[%d]",rightName,rightStart);
  (void) CtlpChangeBracket(rightVar);

  formula = Ctlp_FormulaCreateEquiv(leftVar,rightVar);
  leftInd  = leftStart;
  rightInd = rightStart;
  for(i=leftStart;i!=leftEnd;i+=leftInc){
     leftInd  += leftInc;
     rightInd += rightInc;
     (void) sprintf(leftVar,"%s[%d]",leftName,leftInd);
     (void) CtlpChangeBracket(leftVar);
     (void) sprintf(rightVar,"%s[%d]",rightName,rightInd);
     (void) CtlpChangeBracket(rightVar);
     tempFormula = Ctlp_FormulaCreateEquiv(leftVar,rightVar);
     formula = Ctlp_FormulaCreate(Ctlp_AND_c,formula,tempFormula);
  }
  FREE(leftName);
  FREE(rightName);
  FREE(leftVar);
  FREE(rightVar);
  return formula;
}


/**Function********************************************************************

  Synopsis    [Creates a CTL formula for multiple AX]

  Description [This function returns a formula, which is the multiple
  times of AX. The given string includes the number of depth]

  SideEffects []

  SeeAlso     []

******************************************************************************/
Ctlp_Formula_t *
Ctlp_FormulaCreateAXMult(
  char *string,
  Ctlp_Formula_t *formula)
{
  int i,num;
  char *str, *ptr;
  Ctlp_Formula_t *result;
  str = strchr(string,':');
  if ( str == NULL) return(NIL(Ctlp_Formula_t));
  str++;

  num = strtol(str,&ptr,0);

  if (num<=0){
    return (NIL(Ctlp_Formula_t));
  }
  result = Ctlp_FormulaCreate(Ctlp_AX_c,formula,NIL(Ctlp_Formula_t));
  for(i=1;i<num;i++){
    result = Ctlp_FormulaCreate(Ctlp_AX_c,result,NIL(Ctlp_Formula_t));
  }
  return result;
}


/**Function********************************************************************

  Synopsis    [Creates a CTL formula for multiple EX]

  Description [This function returns a formula, which is the multiple
  times of EX. The given string includes the number of depth]

  SideEffects []

  SeeAlso     []

******************************************************************************/
Ctlp_Formula_t *
Ctlp_FormulaCreateEXMult(
  char *string,
  Ctlp_Formula_t *formula)
{
  int i, num;
  char *str, *ptr;
  Ctlp_Formula_t *result;
  str = strchr(string,':');
  if ( str == NULL) return(NIL(Ctlp_Formula_t));
  str++;

  num = strtol(str,&ptr,0);

  if (num<=0){
    return (NIL(Ctlp_Formula_t));
  }
  result = Ctlp_FormulaCreate(Ctlp_EX_c,formula,NIL(Ctlp_Formula_t));
  for(i=1;i<num;i++){
    result = Ctlp_FormulaCreate(Ctlp_EX_c,result,NIL(Ctlp_Formula_t));
  }
  return result;
}

/**Function********************************************************************

  Synopsis [Converts an array of CTL formulae to forward form.]

  Description [Converts an array of CTL formulae to forward.  Creates a
  maximally shared representation if noshare is false, a representation without
  any sharing otherwise. ]

  SideEffects []

  SeeAlso     []

******************************************************************************/
array_t *
Ctlp_FormulaArrayConvertToForward(array_t *formulaArray, int singleInitial,
				  int doNotShareFlag)
{
  int i;
  Ctlp_Formula_t *formula, *existentialFormula, *forwardFormula;
  int numFormulae = array_n(formulaArray);
  array_t *forwardFormulaArray = array_alloc(Ctlp_Formula_t *, numFormulae);
  array_t *forwardDagArray;
  int compareValue;

  for(i=0; i < numFormulae; i++) {
    formula = array_fetch(Ctlp_Formula_t *, formulaArray, i);
    existentialFormula = Ctlp_FormulaConvertToExistentialForm(formula);
    assert(existentialFormula != NIL(Ctlp_Formula_t));
    formula = NIL(Ctlp_Formula_t);
   /*
    ** compareValue = 0 : check with FALSE
    ** compareValue = 1 : check with not-FALSE
    */
    if (singleInitial)
      compareValue = FormulaGetCompareValue(existentialFormula);
    else
      compareValue = 0;
    forwardFormula = FormulaCreateWithType(Ctlp_AND_c);
    forwardFormula->left = FormulaCreateWithType(Ctlp_Init_c);
    if (compareValue == 0) {
      forwardFormula->right = FormulaCreateWithType(Ctlp_NOT_c);
      forwardFormula->right->left = existentialFormula;
    } else
      forwardFormula->right = existentialFormula;
    forwardFormula->right->forward = forwardFormula;
    /* if formula was converted before, keep that reference */
    if(existentialFormula->dbgInfo.originalFormula != NIL(Ctlp_Formula_t))
      forwardFormula->dbgInfo.originalFormula =
	existentialFormula->dbgInfo.originalFormula;
    else
      forwardFormula->dbgInfo.originalFormula = existentialFormula;

    forwardFormula->top = 1;
    forwardFormula->compareValue = compareValue;
    FormulaConvertToForward(forwardFormula->right, compareValue);
    CtlpFormulaSetStatesToNULL(forwardFormula);
    FormulaInsertForwardCompareNodes(forwardFormula, NULL, compareValue);
    array_insert(Ctlp_Formula_t *, forwardFormulaArray, i, forwardFormula);
  }

  if (doNotShareFlag)
    return(forwardFormulaArray);

  forwardDagArray = Ctlp_FormulaArrayConvertToDAG(forwardFormulaArray);
  array_free(forwardFormulaArray);
  return forwardDagArray;
}


/**Function********************************************************************

  Synopsis    [Returns a string for each formula type.]

  Description [Returns a string for each formula type.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
char *
Ctlp_FormulaGetTypeString(Ctlp_Formula_t *formula)
{
  char *result;

  switch(formula->type) {
    /*
     * The cases are listed in rough order of their expected frequency.
     */
    case Ctlp_ID_c:
      result = util_strsav("ID");
      break;
    case Ctlp_OR_c:
      result = util_strsav("OR");
      break;
    case Ctlp_AND_c:
      result = util_strsav("AND");
      break;
    case Ctlp_THEN_c:
      result = util_strsav("THEN");
      break;
    case Ctlp_XOR_c:
      result = util_strsav("XOR");
      break;
    case Ctlp_EQ_c:
      result = util_strsav("EQ");
      break;
    case Ctlp_NOT_c:
      result = util_strsav("NOT");
      break;
    case Ctlp_EX_c:
      result = util_strsav("EX");
      break;
    case Ctlp_EG_c:
      result = util_strsav("EG");
      break;
    case Ctlp_EF_c:
      result = util_strsav("EF");
      break;
    case Ctlp_EU_c:
      result = util_strsav("EU");
      break;
    case Ctlp_AX_c:
      result = util_strsav("AX");
      break;
    case Ctlp_AG_c:
      result = util_strsav("AG");
      break;
    case Ctlp_AF_c:
      result = util_strsav("AF");
      break;
    case Ctlp_AU_c:
      result = util_strsav("AU");
      break;
    case Ctlp_TRUE_c:
      result = util_strsav("TRUE");
      break;
    case Ctlp_FALSE_c:
      result = util_strsav("FALSE");
      break;
    case Ctlp_Init_c:
      result = util_strsav("Init");
      break;
    case Ctlp_Cmp_c:
      result = util_strsav("Cmp");
      break;
    case Ctlp_FwdU_c:
      result = util_strsav("FwdU");
      break;
    case Ctlp_FwdG_c:
      result = util_strsav("FwdG");
      break;
    case Ctlp_EY_c:
      result = util_strsav("EY");
      break;
    case Ctlp_EH_c:
      result = util_strsav("EH");
      break;
    default:
      fail("Unexpected type");
  }
  return(result);
}


/**Function********************************************************************

  Synopsis           [Tests if a formula is quantifier-free, ACTL, ECTL or
  mixed.]

  Description [Tests if a formula is Quantifier free, ACTL, ECTL or mixed.  We
  assume that the formula is in existential normal form, and hence only
  contains existential temporal operators.

  <p>The formula is ECTL if every temporal operator appears under an even
  number of negations, and it is ACTL if every temporal operator appears under
  an odd number of negations.  It is quantifier-free if it is both ECTL and
  ACTL (i.e., no temporal operators occur), and it is mixed if it is neither
  ECTL nor ACTL.]

  SideEffects        []

  SeeAlso            []

******************************************************************************/
Ctlp_FormulaClass
Ctlp_CheckClassOfExistentialFormula(
  Ctlp_Formula_t *formula)
{
  Ctlp_FormulaType formulaType;
  Ctlp_Formula_t *rightFormula, *leftFormula;
  Ctlp_FormulaClass resultLeft, resultRight, tempResult, currentClass;

  assert(formula != NIL(Ctlp_Formula_t));
  if(formula == NIL(Ctlp_Formula_t))
    return Ctlp_QuantifierFree_c;

  formulaType = Ctlp_FormulaReadType(formula);
  leftFormula = Ctlp_FormulaReadLeftChild(formula);
  rightFormula = Ctlp_FormulaReadRightChild(formula);

  /* Depending on the formula type, create result or recur */
  switch (formulaType) {
  case Ctlp_EX_c:
  case Ctlp_EG_c:
  case Ctlp_EF_c:
  case Ctlp_EU_c:
  case Ctlp_FwdU_c:
  case Ctlp_FwdG_c:
  case Ctlp_EY_c:
  case Ctlp_EH_c:
    resultLeft = Ctlp_CheckClassOfExistentialFormula(leftFormula);
    if (formulaType == Ctlp_EU_c || formulaType == Ctlp_FwdU_c ||
	formulaType == Ctlp_FwdG_c )
      resultRight = Ctlp_CheckClassOfExistentialFormula(rightFormula);
    else
      resultRight = Ctlp_QuantifierFree_c;

    tempResult = CtlpResolveClass(resultLeft, resultRight);
    currentClass = Ctlp_ECTL_c;
    return CtlpResolveClass(currentClass, tempResult);
  case Ctlp_AX_c:
  case Ctlp_AG_c:
  case Ctlp_AF_c:
  case Ctlp_AU_c:
    /* The formula is supposed to be only existential */
    fprintf(vis_stdout,
	    "** Ctlp Error: unexpected (universal) operator type\n");
    assert(0);
    return Ctlp_Mixed_c;
  case Ctlp_OR_c:
  case Ctlp_AND_c:
    resultLeft = Ctlp_CheckClassOfExistentialFormula(leftFormula);
    resultRight = Ctlp_CheckClassOfExistentialFormula(rightFormula);
    return CtlpResolveClass(resultLeft, resultRight);
  case Ctlp_NOT_c:
    tempResult = Ctlp_CheckClassOfExistentialFormula(leftFormula);
    return CtlpNegateFormulaClass(tempResult);
   case Ctlp_THEN_c:
    resultLeft = Ctlp_CheckClassOfExistentialFormula(leftFormula);
    resultRight = Ctlp_CheckClassOfExistentialFormula(rightFormula);
    tempResult = CtlpNegateFormulaClass(resultLeft);
    return CtlpResolveClass(tempResult, resultRight);
  case Ctlp_XOR_c:
  case Ctlp_EQ_c:
    resultLeft =  Ctlp_CheckClassOfExistentialFormula(leftFormula);
    resultRight = Ctlp_CheckClassOfExistentialFormula(rightFormula);
    tempResult = CtlpResolveClass(resultLeft, resultRight);
    if( tempResult == Ctlp_QuantifierFree_c)
      return Ctlp_QuantifierFree_c;
    else
      return Ctlp_Mixed_c;
  case Ctlp_ID_c:
  case Ctlp_TRUE_c:
  case Ctlp_FALSE_c:
  case Ctlp_Init_c:
     return Ctlp_QuantifierFree_c;
  case Ctlp_Cmp_c:
    return Ctlp_Mixed_c;
  default:
    fprintf(vis_stderr, "**Ctlp Error: Unexpected operator detected.\n");
    assert(0);
    return Ctlp_Mixed_c;
  }
} /* End of Ctlp_CheckTypeOfExistentialFormula */

/**Function********************************************************************

  Synopsis [Tests if an array of simple existential formulae has only
  ACTL, only ECTL or mixture of formula.]

  Description        [Returns Ctlp_ECTL_c if the array contains only
  ECTL formulae, Ctlp_ACTL_c if it contains only ACTL formulae,
  Ctlp_QuantifierFree_c if there are no quantifiers in any formulae,
  and Ctlp_Mixed otherwise.]

  SideEffects        []

  SeeAlso            [Ctlp_CheckClassOfExistentialFormula]

******************************************************************************/
Ctlp_FormulaClass
Ctlp_CheckClassOfExistentialFormulaArray(
  array_t *formulaArray)
{
  Ctlp_FormulaClass result;
  Ctlp_Formula_t *formula;
  int formulanr;

  result = Ctlp_QuantifierFree_c;
  arrayForEachItem(Ctlp_Formula_t *, formulaArray, formulanr, formula){
    Ctlp_FormulaClass  formulaType;
    formulaType = Ctlp_CheckClassOfExistentialFormula(formula);
    result = CtlpResolveClass(result, formulaType);
    if(result == Ctlp_Mixed_c)
      return result;
  }
  return result;
} /* End of Ctlp_CheckTypeOfExistentialFormulaArray */


/**Function********************************************************************

  Synopsis    [Compare two formulas for equality]

  Description [Checks if two formulas are syntactically the same.  Equivalent
  constructions such as TRUE and TRUE+FALSE are not recognized, but copies are
  handled correctly, and strings are compared using strcmp.]

  SideEffects []

  SeeAlso     [FormulaCompare]

******************************************************************************/
int
Ctlp_FormulaIdentical(
  Ctlp_Formula_t *formula1,
  Ctlp_Formula_t *formula2)
{
  return(FormulaCompare((char *)formula1, (char *)formula2) == 0);
}


/**Function********************************************************************

  Synopsis    [Replaces non-monotonic operators in a formula.]

  Description [This function takes a parse tree for a CTL formula and
  transforms it into another tree that has only monotonic operators.
  The non-monotonic operators are <code>XOR</code> and <code>EQ</code>.
  They are replaced by eqivalent trees.  Specificallt, <code>XOR(a,b)</code>
  is replaced by <code>OR(AND(a,NOT(b)),AND(NOT(a),b))</code> and
  <code>f <-> g</code> is replaced by <code>AND(f->g,g->f)</code>.<p>

  It is important that the original formula be a tree.  The idea is that this
  function should be called right after reading the formulae from file.  Since
  the information attached to the parse tree by the model checking procedures
  is not preserved, it is also important that no such information exists.
  These conditions are checked by an assertion.<p>

  This function does not deal with the forward CTL operators.  Reduction to
  monotonic operators should be performed before conversion to forward CTL.]

  SideEffects [The argument is modified.  The new formulae will apprear
  when they are printed out.]

  SeeAlso     [Ctlp_FormulaArrayMakeMonotonic]

******************************************************************************/
void
Ctlp_FormulaMakeMonotonic(
  Ctlp_Formula_t *formula)
{
  Ctlp_Formula_t *left, *right;
  Ctlp_FormulaType type;

  if (formula == NIL(Ctlp_Formula_t)) {
    return;
  }

  /* Make sure (recursively) that this (sub)-formula is still in its pristine
   * state.  Specifically, that the parse graph is a tree.
   */
  assert(formula->refCount == 1 &&
	 formula->forward == NIL(Ctlp_Formula_t) &&
	 formula->latest == Ctlp_Incomparable_c &&
	 formula->dbgInfo.data == NIL(void) &&
	 formula->dbgInfo.freeFn == (Ctlp_DbgInfoFreeFn) NULL &&
	 formula->dbgInfo.convertedFlag == FALSE &&
	 formula->dbgInfo.originalFormula == NIL(Ctlp_Formula_t) &&
	 formula->top == 0 &&
	 formula->compareValue == 0
	 );

  type = Ctlp_FormulaReadType(formula);

  /* Leaves remain unchanged. */
  if (type == Ctlp_ID_c || type == Ctlp_TRUE_c || type == Ctlp_FALSE_c) {
    return;
  }

  /* First recursively fix the children. */
  left  = Ctlp_FormulaReadLeftChild(formula);
  Ctlp_FormulaMakeMonotonic(left);
  right = Ctlp_FormulaReadRightChild(formula);
  Ctlp_FormulaMakeMonotonic(right);

  /* Fix this node if it is not monotonic. */
  switch (type) {
  case Ctlp_EX_c:
  case Ctlp_EG_c:
  case Ctlp_EF_c:
  case Ctlp_EU_c:
  case Ctlp_AX_c:
  case Ctlp_AG_c:
  case Ctlp_AF_c:
  case Ctlp_AU_c:
  case Ctlp_OR_c:
  case Ctlp_AND_c:
  case Ctlp_NOT_c:
  case Ctlp_THEN_c:
  case Ctlp_EY_c:
  case Ctlp_EH_c:
    /* Nothing to be done */
    break;
  case Ctlp_EQ_c: {
    Ctlp_Formula_t *dupLeft, *dupRight;
    Ctlp_Formula_t *thenLeft, *thenRight;
    dupLeft   = Ctlp_FormulaDup(left);
    dupRight  = Ctlp_FormulaDup(right);
    thenLeft  = Ctlp_FormulaCreate(Ctlp_THEN_c, left, right);
    thenRight = Ctlp_FormulaCreate(Ctlp_THEN_c, dupRight, dupLeft);
    /* Fix the node in place. */
    formula->type  = Ctlp_AND_c;
    formula->left  = thenLeft;
    formula->right = thenRight;
    break;
  }
  case Ctlp_XOR_c: {
    Ctlp_Formula_t *dupLeft, *dupRight;
    Ctlp_Formula_t *negLeft, *negRight;
    Ctlp_Formula_t *andLeft, *andRight;
    dupLeft  = Ctlp_FormulaDup(left);
    dupRight = Ctlp_FormulaDup(right);
    negLeft  = Ctlp_FormulaCreate(Ctlp_NOT_c, dupLeft, NIL(Ctlp_Formula_t));
    negRight = Ctlp_FormulaCreate(Ctlp_NOT_c, dupRight, NIL(Ctlp_Formula_t));
    andLeft  = Ctlp_FormulaCreate(Ctlp_AND_c, left, negRight);
    andRight = Ctlp_FormulaCreate(Ctlp_AND_c, negLeft, right);
    /* Fix the node in place. */
    formula->type  = Ctlp_OR_c;
    formula->left  = andLeft;
    formula->right = andRight;
    break;
  }
  default:
    /* Forward CTL operators (that are not supported) and leaves (that are
     * dealt with above) would fall here.
     */
    fail("unexpected CTL formula type\n");
  }

  return;

} /* Ctlp_FormulaMakeMonotonic */


/**Function********************************************************************

  Synopsis    [Replaces non-monotonic operators in an array of formulae.]

  Description [Apply Ctlp_FormulaMakeMonotonic to each formula in the array.]

  SideEffects [The formulae in the array are modified.]

  SeeAlso     [Ctlp_FormulaMakeMonotonic]

******************************************************************************/
void
Ctlp_FormulaArrayMakeMonotonic(
  array_t *formulaArray /* of Ctlp_Formula_t */)
{
  int i;
  Ctlp_Formula_t *formula;

  if (formulaArray == NIL(array_t)) return;
  arrayForEachItem(Ctlp_Formula_t *, formulaArray, i, formula) {
    Ctlp_FormulaMakeMonotonic(formula);
  }

} /* Ctlp_FormulaArrayMakeMonotonic */


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/


/**Function********************************************************************

  Synopsis    [Increments the reference count of a formula.]

  Description [The function increments the reference count of a formula. If
  the formula is NULL, the function does nothing.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
void
CtlpFormulaIncrementRefCount(
  Ctlp_Formula_t *formula)
{
  if(formula!=NIL(Ctlp_Formula_t)) {
    ++(formula->refCount);
  }
}

/**Function********************************************************************

  Synopsis    [Decrements the reference count of a formula.]

  Description [The function decrements the reference count of formula and if
  the reference count reaches 0, the formula is freed. If the formula is NULL,
  the function does nothing. It is an error to decrement the reference count
  below 0.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
void
CtlpFormulaDecrementRefCount(
  Ctlp_Formula_t *formula)
{
  if(formula!=NIL(Ctlp_Formula_t)) {
    assert(formula->refCount>0);
    if(--(formula->refCount) == 0)
      CtlpFormulaFree(formula);
  }
}
/**Function********************************************************************

  Synopsis    [Adds formula to the end of globalFormulaArray.]

  SideEffects [Manipulates the global variable globalFormulaArray.]

  SeeAlso     [CtlpYyparse]

******************************************************************************/
void
CtlpFormulaAddToGlobalArray(
  Ctlp_Formula_t * formula)
{
  array_insert_last(Ctlp_Formula_t *, globalFormulaArray, formula);
}

/**Function********************************************************************

  Synopsis    [Frees a CTL formula.]

  Description [The function frees all memory associated with the formula,
  including all MDDs and all character strings (however, does not free
  dbgInfo.originalFormula). It also decrements the reference counts of its two
  chidren. The function does nothing if formula is NULL.]

  SideEffects []

  SeeAlso     [Ctlp_FormulaArrayFree]

******************************************************************************/
void
CtlpFormulaFree(
  Ctlp_Formula_t * formula)
{
  if (formula != NIL(Ctlp_Formula_t)) {

    /*
     * Free any fields that are not NULL.
     */

    if (formula->type == Ctlp_ID_c) {
      FREE(formula->left);
      FREE(formula->right);
    }
    else {
      if (formula->left  != NIL(Ctlp_Formula_t)) {
	CtlpFormulaDecrementRefCount(formula->left);
      }
      if (formula->right != NIL(Ctlp_Formula_t)) {
	CtlpFormulaDecrementRefCount(formula->right);
      }
    }
    if (formula->matchfound_top != NIL(array_t))
      array_free(formula->matchfound_top);
    if (formula->matchelement_top != NIL(array_t))
      array_free(formula->matchelement_top);
    if (formula->matchfound_bot != NIL(array_t))
      array_free(formula->matchfound_bot);
    if (formula->matchelement_bot != NIL(array_t))
      array_free(formula->matchelement_bot);
    if (formula->leaves != NIL(array_t))
      array_free(formula->leaves);
    if (formula->dbgInfo.data != NIL(void)) {
      (*formula->dbgInfo.freeFn)(formula);
    }
    FREE(formula);
  }
}


/**Function********************************************************************

  Synopsis    [Create a binary string of given value with size of interval ]

  Description [The value is a binary string starting with 'b',
  a hexadecimal string starting with 'x', or an
  integer string. If Value is not fitted in the interval, 0 is return.
  Otherwise 1 is returned. Result string is saved at given pointer.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
int
CtlpStringChangeValueStrToBinString(
  char *value,
  char *binValueStr,
  int  interval)
{
  int i;
  long int num, mask;
  double maxNum;
  char *ptr;

  mask = 1;
  maxNum = pow(2.0,(double)interval);
  errno = 0;

  if(value[0] == 'b'){
    if ((int)strlen(value)-1 == interval){
			for(i=1;i<=interval;i++){
	if (value[i] == '0' || value[i] == '1'){
	  binValueStr[i-1] =  value[i];
	}else{
	  return 0;
	}
      }
      binValueStr[interval] = '\0';
    }else{
      return 0;
    }
  }else if (value[0] == 'x'){
    for(i=1; i < (int)strlen(value); i++){
      if (!isxdigit((int)value[i])){
	return 0;
      }
    }
    num = strtol(++value,&ptr,16);
    if (errno) return 0;
    if ( num >= maxNum) return 0;
    for(i=0;i<interval;i++){
      if(num & (mask<<i)) binValueStr[interval-i-1] = '1';
      else binValueStr[interval-i-1] = '0';
    }
    binValueStr[interval] = '\0';
  }else{
    for(i=0;i<(int)strlen(value);i++){
      if (!isdigit((int)value[i])){
	return 0;
      }
    }
    num = strtol(value,&ptr,0);
    if (errno) return 0;
    if ( num >= maxNum) return 0;
    mask = 1;
    for(i=0;i<interval;i++){
      if(num & (mask<<i)) binValueStr[interval-i-1] = '1';
      else binValueStr[interval-i-1] = '0';
    }
    binValueStr[interval] = '\0';
  }

  return(1);
}


/**Function********************************************************************

  Synopsis    [Change [] to <>]

  Description []

  SideEffects [The input string contains ...[num] and this function changes it
  to ...<num>.]

  SeeAlso     []

******************************************************************************/
void
CtlpChangeBracket(
  char *inStr)
{
  int i, j;
  char *str;

  j = 0;
  for (i=0;i<(int)strlen(inStr)+1;i++){
    if (inStr[i] != ' '){
      inStr[i-j] = inStr[i];
    }else{
      j++;
    }
  }

  if (changeBracket == 0)
    return;

  str = strchr(inStr,'[');
  if (str == NULL) return;

  *str = '<';
  str = strchr(inStr,']');
  *str = '>';
}


/**Function********************************************************************

  Synopsis    [Parse a given vector string.]

  Description [Parse given vector string of the form of var<i:j> and
  string var is returned. Other information such as i, j, interval between
  i and j, and increment or decrement from i to j are saved.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
char *
CtlpGetVectorInfoFromStr(
   char *str,
   int  *start,
   int  *end,
   int  *interval,
   int  *inc)
{
  char *str1, *str2, *str3;
  char *startStr, *endStr;
  char *name, *ptr;

  str1 = strchr(str,'[');
  str2 = strchr(str,':');
  str3 = strchr(str,']');
  startStr = ALLOC(char, str2-str1);
  endStr = ALLOC(char, str3-str2);
  name = ALLOC(char, str1-str+1);

  (void) strncpy(name, str, str1-str);
  (void) strncpy(startStr, str1+1, str2-str1-1);
  (void) strncpy(endStr, str2+1, str3-str2-1);

  startStr[str2-str1-1] = '\0';
  endStr[str3-str2-1] = '\0';
  name[str1-str] = '\0';
  *start = strtol(startStr,&ptr,0);
  *end = strtol(endStr,&ptr,0);
  if(*start > *end){
      *inc = -1;
      *interval = *start - *end + 1;
  } else {
      *inc = 1;
      *interval = *end - *start + 1;
  }
  FREE(startStr);
  FREE(endStr);
  return name;
}

/**Function********************************************************************

  Synopsis    [Insert macro formula into symbol table.]

  Description [If the name is already in table return 0, otherwise insert the
  formula into the table and return 1]

  SideEffects []

  SeeAlso     []

******************************************************************************/
int
CtlpFormulaAddToTable(
   char *name,
   Ctlp_Formula_t *formula,
   st_table *macroTable)
{
   if(macroTable == NIL(st_table)){
      macroTable = st_init_table(strcmp, st_strhash);
   }
   if(st_is_member(macroTable, name)){
      return 0;
   }
   st_insert(macroTable, name, (char *)formula);
   return 1;
}


/**Function********************************************************************

  Synopsis    [Get macro formula from symbol table]

  Description [If macro string is not found in table, NULL pointer is returned]

  SideEffects []

  SeeAlso     []

******************************************************************************/
Ctlp_Formula_t *
CtlpFormulaFindInTable(
   char *name,
   st_table *macroTable)
{
   Ctlp_Formula_t *formula;
   if (st_lookup(macroTable, name, &formula)){
      return (Ctlp_FormulaDup(formula));
   }else{
      return NIL(Ctlp_Formula_t);
   }
}

/**Function********************************************************************

  Synopsis    [Returns the greatest lower bound of two classes]

  Description [Returns the class of a formula that is the combination of a
  formula of class1 and class2.  Does this by computing the glb, where mixed <
  ECTL < quantifierFree, and mixed < ACTL < quantifierfree.]

  SideEffects []

  SeeAlso [Ctlp_CheckClassOfExistentialFormula]

******************************************************************************/
Ctlp_FormulaClass
CtlpResolveClass(
  Ctlp_FormulaClass class1,
  Ctlp_FormulaClass class2)
{
  if ((class1 == Ctlp_Mixed_c) || (class2 == Ctlp_Mixed_c))
    return Ctlp_Mixed_c;
  if (class1 == Ctlp_QuantifierFree_c)
    return class2;
  if (class2 == Ctlp_QuantifierFree_c)
    return class1;
  if (class1 == class2)
    return class1;

  return Ctlp_Mixed_c;

}


/**Function********************************************************************

  Synopsis    [Returns the class of a formula that is the negation of a formula
  of class "class".]

  Description [The negation of a mixed (quantifier free, ECTL, ACTL) formula is
  mixed (quantifier free, ACTL, ECTL).]

  SideEffects []

  SeeAlso [Ctlp_CheckClassOfExistentialFormula]

******************************************************************************/
Ctlp_FormulaClass
CtlpNegateFormulaClass(
  Ctlp_FormulaClass class_
)
{
  switch(class_){
  case Ctlp_Mixed_c:
    return Ctlp_Mixed_c;
  case Ctlp_QuantifierFree_c:
    return Ctlp_QuantifierFree_c;
  case Ctlp_ECTL_c:
    return Ctlp_ACTL_c;
  case Ctlp_ACTL_c:
    return Ctlp_ECTL_c;
  default:
    fprintf(vis_stderr, "**Ctlp Error: Unknown formula class\n");
    assert(0);
    return Ctlp_Mixed_c;
  }
}


/**Function********************************************************************

  Synopsis    [Replaces the smallest important sub-fomula in a w-ACTL
  formula by bottom.]

  Description [Creates a witness formula for a given w-ACTL formula by
  replacing the smallest inpurtant subformula with bottom.  This function is
  used in the approach to vacuity detection of Beer et al. (CAV97).  Formulae
  in w-ACTL are guaranteed to have a unique smallest important subformula,
  which is defined as follow.  If the formula is simple, its operands are not
  important.  (An exception is made for implications.  See
  ReplaceSimpleFormula.)  Otherwise, the non-simple operand is the important
  operand.  For (A p U q) or (E p U q), where p and q are both simple, p is the
  important operand.  For unary formulae, the only child is the important
  operand.  This function works also for ACTL formulae that are not in w-ACTL,
  and for which smallest important subformulae are not unique.
  The function picks up a smallest important subformula.]

  SideEffects [none]

  SeeAlso     [ReplaceSimpleFormula]

******************************************************************************/
Ctlp_Formula_t *
Ctlp_FormulaCreateWitnessFormula(
  Ctlp_Formula_t *formula)
{
  Ctlp_FormulaType formulaType;
  Ctlp_Formula_t *rightFormula, *leftFormula;
  Ctlp_Formula_t *newLeftFormula, *newRightFormula;

  assert(formula != NIL(Ctlp_Formula_t));

  if(formula == NIL(Ctlp_Formula_t))
    return NIL(Ctlp_Formula_t);

  formulaType = Ctlp_FormulaReadType(formula);
  leftFormula = Ctlp_FormulaReadLeftChild(formula);
  rightFormula = Ctlp_FormulaReadRightChild(formula);

  if (Ctlp_FormulaTestIsQuantifierFree(formula)) {
    return ReplaceSimpleFormula(formula);
  }

  switch (formulaType) {
  case Ctlp_AX_c:
  case Ctlp_EX_c:
  case Ctlp_AG_c:
  case Ctlp_EG_c:
  case Ctlp_AF_c:
  case Ctlp_EF_c:
  case Ctlp_NOT_c:
    newLeftFormula = Ctlp_FormulaCreateWitnessFormula(leftFormula);
    newRightFormula = NIL(Ctlp_Formula_t);
    break;
  case Ctlp_AU_c:
  case Ctlp_EU_c:
    /* If both or neither operands are simple, we choose the left one
       as important.  Deal with special case of converted AG p (E true U p). */
    if ((formulaType == Ctlp_EU_c &&
	 Ctlp_FormulaReadType(leftFormula) == Ctlp_TRUE_c) ||
	(Ctlp_FormulaTestIsQuantifierFree(leftFormula) &&
	 !Ctlp_FormulaTestIsQuantifierFree(rightFormula))) {
      newLeftFormula = Ctlp_FormulaDup(leftFormula);
      newRightFormula = Ctlp_FormulaCreateWitnessFormula(rightFormula);
    } else {
      newLeftFormula = Ctlp_FormulaCreateWitnessFormula(leftFormula);
      newRightFormula = Ctlp_FormulaDup(rightFormula);
    }
    break;
  case Ctlp_OR_c:
  case Ctlp_AND_c:
  case Ctlp_THEN_c:
    /* Here we know one is not simple */
    if (Ctlp_FormulaTestIsQuantifierFree(leftFormula)) {
      newLeftFormula = Ctlp_FormulaDup(leftFormula);
      newRightFormula = Ctlp_FormulaCreateWitnessFormula(rightFormula);
    } else {
      newLeftFormula = Ctlp_FormulaCreateWitnessFormula(leftFormula);
      newRightFormula = Ctlp_FormulaDup(rightFormula);
    }
    break;
  case Ctlp_XOR_c:
  case Ctlp_EQ_c:
    newLeftFormula = Ctlp_FormulaDup(leftFormula);
    newRightFormula = Ctlp_FormulaDup(rightFormula);
    break;
  default:
    newLeftFormula = NIL(Ctlp_Formula_t);
    newRightFormula = NIL(Ctlp_Formula_t);
    fprintf(vis_stderr, "**Ctlp Error: Unexpected operator detected.\n");
    assert(0);
    break;
  }
  return Ctlp_FormulaCreate(formulaType,newLeftFormula,newRightFormula);

} /* Ctlp_FormulaCreateWitnessFormula */


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [Creates a CTL formula with just the type set.]

  Description [Calls Ctlp_FormulaCreate with type, and all other fields NULL.]

  SideEffects []

  SeeAlso     [Ctlp_FormulaCreate]

******************************************************************************/
static Ctlp_Formula_t *
FormulaCreateWithType(
  Ctlp_FormulaType  type)
{
  return (Ctlp_FormulaCreate(type, NIL(Ctlp_Formula_t), NIL(Ctlp_Formula_t)));
}


/**Function********************************************************************

  Synopsis    [The comparison function for the formula unique table.]

  Description [The function takes as parameters two CTL formulae. It compares
  the formula type, the left child and the right child, and returns 0 if they
  match. Otherwise, it returns -1.]

  SideEffects []

  SeeAlso     [FormulaHash]

******************************************************************************/
static int
FormulaCompare(
  const char *key1,
  const char *key2)
{
  Ctlp_Formula_t *formula1 = (Ctlp_Formula_t *) key1;
  Ctlp_Formula_t *formula2 = (Ctlp_Formula_t *) key2;

  assert(key1 != NIL(char));
  assert(key2 != NIL(char));


  if(formula1->type != formula2->type) {
    return -1;
  }
  if(formula1->type == Ctlp_ID_c) {
    if(strcmp((char *) (formula1->left), (char *) (formula2->left)) ||
       strcmp((char *) (formula1->right), (char *) (formula2->right))) {
      return -1;
    } else
      return 0;
  } else {
    if(formula1->left != formula2->left)
      return -1;
    if(formula1->right != formula2->right)
      return -1;
    if (formula1->type == Ctlp_Cmp_c &&
	formula1->compareValue != formula2->compareValue) {
      return -1;
    }
    return 0;
  }
}

/**Function********************************************************************

  Synopsis    [The hash function for the formula unique table.]

  Description [The function takes as parameter a CTL formula. If the formula
  type is Ctlp_ID_c, st_strhash is used with a concatenation of left and
  right children as the key string. Otherwise, something very similar to
  st_ptrhash is done.]

  SideEffects []

  SeeAlso     [FormulaCompare]

******************************************************************************/
static int
FormulaHash(
  char *key,
  int modulus)
{
  char *hashString;
  int hashValue;
  Ctlp_Formula_t *formula = (Ctlp_Formula_t *) key;

  if(formula->type==Ctlp_ID_c) {
    hashString = util_strcat3((char *) (formula->left), (char *)
			      (formula->right), "");
    hashValue = st_strhash(hashString, modulus);
    FREE(hashString);
    return hashValue;
  }
  return (int) ((((unsigned long) formula->left >>2) +
		 ((unsigned long) formula->right >>2)) % modulus);
}

/**Function********************************************************************

  Synopsis    [Hashes the formula into the unique table.]

  Description [The function takes a formula and hashes it and all its
  subformulae into a unique table. It returns the unique formula identical to
  the formula being hashed. The formula returned will have maximum sharing
  with the formulae that are already present in uniqueTable. It returns
  NIL(Ctlp_Formula_t) if the formula is NIL(Ctlp_Formula_t).]

  SideEffects [If a copy of some subformula of formula is present in
  uniqueTable then the copy is substituted for it and the reference count of
  the subform
  ula is decremented.]

  SeeAlso     [FormulaCompare]

******************************************************************************/
static Ctlp_Formula_t *
FormulaHashIntoUniqueTable(
  Ctlp_Formula_t *formula,
  st_table *uniqueTable)
{
  Ctlp_Formula_t *uniqueFormula, *uniqueLeft, *uniqueRight;

  if(formula == NIL(Ctlp_Formula_t))
    return NIL(Ctlp_Formula_t);
  if(st_lookup(uniqueTable, formula, &uniqueFormula)) {
    return uniqueFormula;
  }
  else {
    if(formula->type == Ctlp_ID_c) {
      st_insert(uniqueTable, formula, formula);
      return formula;
    }
    else {
      uniqueLeft = FormulaHashIntoUniqueTable(formula->left, uniqueTable);
      if(uniqueLeft != NIL(Ctlp_Formula_t))
	if(uniqueLeft != formula->left) {
	  CtlpFormulaDecrementRefCount(formula->left);
	  formula->left = uniqueLeft;
	  CtlpFormulaIncrementRefCount(formula->left);
	}
      uniqueRight = FormulaHashIntoUniqueTable(formula->right, uniqueTable);
      if(uniqueRight != NIL(Ctlp_Formula_t))
	if(uniqueRight != formula->right) {
	  CtlpFormulaDecrementRefCount(formula->right);
	  formula->right = uniqueRight;
	  CtlpFormulaIncrementRefCount(formula->right);
	}
      if(st_lookup(uniqueTable, formula, &uniqueFormula)) {
	return uniqueFormula;
      }
      else {
	st_insert(uniqueTable, formula, formula);
	return formula;
      }
    }
  }
}

/**Function********************************************************************

  Synopsis    [Converts the formula tree to a DAG.]

  Description [The function recursively builds an existential DAG for the
  formula. If a particular sub-formula has been encountered before, the
  converted existential sub-formula is pointed to by the field states and it
  is returned without creating a new formula.]

  SideEffects [We abuse states to store which converted formula an original
  formula points to.]

  SeeAlso     []

******************************************************************************/
static Ctlp_Formula_t *
FormulaConvertToExistentialDAG(
  Ctlp_Formula_t *formula)
{

  Ctlp_Formula_t *new_;
  char *variableNameCopy, *valueNameCopy;

  if(formula==NIL(Ctlp_Formula_t))
    return NIL(Ctlp_Formula_t);

  if(formula->forward!=NIL(Ctlp_Formula_t)) {
    Ctlp_Formula_t *temp = formula->forward;

    ++(temp->refCount);
    return temp;
  }

  /*
   * Recursively convert each subformula.
   */

  switch(formula->type) {
    case Ctlp_EF_c:
      /* EFf --> (E true U f)  */
      new_ = FormulaCreateWithType(Ctlp_EU_c);
      new_->left  = FormulaCreateWithType(Ctlp_TRUE_c);
      new_->right = FormulaConvertToExistentialDAG(formula->left);
      new_->dbgInfo.convertedFlag = TRUE;
      break;

    case Ctlp_AX_c:
      /* AXf --> !(EX(!f)) */
      new_ = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left = FormulaCreateWithType(Ctlp_EX_c);
      new_->left->left = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left->left->left = FormulaConvertToExistentialDAG(formula->left);
      new_->dbgInfo.convertedFlag = TRUE;
      break;

    case Ctlp_AG_c:
      /* AGf --> ![(E true U !f)] */
      new_ = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left = FormulaCreateWithType(Ctlp_EU_c);
      new_->left->left = FormulaCreateWithType(Ctlp_TRUE_c);
      new_->left->right = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left->right->left = FormulaConvertToExistentialDAG(formula->left);
      new_->dbgInfo.convertedFlag = TRUE;
      break;

    case Ctlp_AF_c:
      /* AFf --> ![EG(!f)] */
      new_ = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left = FormulaCreateWithType(Ctlp_EG_c);
      new_->left->left = FormulaCreateWithType(Ctlp_NOT_c);
      new_->left->left->left = FormulaConvertToExistentialDAG(formula->left);
      new_->dbgInfo.convertedFlag = TRUE;
      break;

    case Ctlp_AU_c:
      /* A[fUg] --> !((E[!g U (!f*!g)]) + (EG!g)) */

      new_ = FormulaCreateWithType(Ctlp_NOT_c); /* top */
      new_->left = FormulaCreateWithType(Ctlp_OR_c); /* ((E[!g U (!f*!g)]) + (EG!g)) */

      new_->left->right = FormulaCreateWithType(Ctlp_EG_c); /* EG !g */
      new_->left->right->left = FormulaCreateWithType(Ctlp_NOT_c); /* !g */
      new_->left->right->left->left = FormulaConvertToExistentialDAG(formula->right); /* g */

      new_->left->left = FormulaCreateWithType(Ctlp_EU_c); /* E[!g U (!f*!g)] */
      new_->left->left->left = FormulaCreateWithType(Ctlp_NOT_c); /* !g */
      new_->left->left->left->left = FormulaConvertToExistentialDAG(formula->right); /* g */
      new_->left->left->right = FormulaCreateWithType(Ctlp_AND_c); /* !f*!g */
      new_->left->left->right->left = FormulaCreateWithType(Ctlp_NOT_c); /* !f */
      new_->left->left->right->left->left = FormulaConvertToExistentialDAG(formula->left); /* f */
      new_->left->left->right->right = FormulaCreateWithType(Ctlp_NOT_c); /* !g */
      new_->left->left->right->right->left = FormulaConvertToExistentialDAG(formula->right); /* g */
      new_->dbgInfo.convertedFlag = TRUE;
      break;

    case Ctlp_ID_c:
      /* Make a copy of the name, and create a new formula. */
      variableNameCopy = util_strsav((char *)(formula->left));
      valueNameCopy = util_strsav((char *)(formula->right));
      new_ = Ctlp_FormulaCreate(Ctlp_ID_c, variableNameCopy, valueNameCopy);
      break;

    case Ctlp_THEN_c:
    case Ctlp_EX_c:
    case Ctlp_EG_c:
    case Ctlp_EU_c:
    case Ctlp_OR_c:
    case Ctlp_AND_c:
    case Ctlp_NOT_c:
    case Ctlp_XOR_c:
    case Ctlp_EQ_c:
    case Ctlp_TRUE_c:
    case Ctlp_FALSE_c:
      /* These are already in the correct form.  Just convert subformulas. */
      new_ = FormulaCreateWithType(formula->type);
      new_->left = FormulaConvertToExistentialDAG(formula->left);
      new_->right = FormulaConvertToExistentialDAG(formula->right);
      break;

    default:
      fail("Unexpected type");
  }

  formula->forward =  new_; /*using states as pointer to the converted
				     formula */
  new_->dbgInfo.originalFormula = formula;
  return new_;
}


/**Function********************************************************************

  Synopsis    [Converts an existential formula to forward form.]

  Description [Converts an existential formula to forward form. To
  convert current formula, it always refer to its parent node and left
  child of parent node. Current formula is right child of parent node.
  Parent node should be always Ctlp_AND_c and left child of parent node
  is already converted.  States is used to store a parent pointer
  throughout the procedure.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
static void
FormulaConvertToForward(Ctlp_Formula_t *formula, int compareValue)
{
  Ctlp_Formula_t *parent, *save;
  Ctlp_Formula_t *p, *q, *r, *f, *g;
  int		 p_qf, q_qf;
  int		 same_order;

  if (formula == NIL(Ctlp_Formula_t))
    return;

  parent = formula->forward;
  r = parent->left; /* already converted */

  /*
   * Recursively convert each subformula.
   */

  switch(formula->type) {
    case Ctlp_EX_c:
      /*
      ** p ^ EXf -> Img(p) ^ f , Img(p) = EY(p)
      **
      **   *           *
      **  / \         / \
      ** p  EX  =>  EY   f
      **    |       |
      **    f       p
      */

      formula->type = Ctlp_EY_c;
      save = formula->left;
      formula->left = parent->left;
      parent->left = formula;
      parent->right = save;

      parent->top = 1;
      parent->compareValue = compareValue;
      parent->right->forward = parent;

      FormulaConvertToForward(parent->right, compareValue);
      break;

    case Ctlp_EF_c:
      /*
      ** EFf --> (E true U f)
      ** p ^ (E true U f) --> FwdU(p, true) ^ f
      **
      **   *             *
      **  / \          /   \
      ** p  EF  =>  FwdU    f
      **    |        / \
      **    f       p  true
      */

      formula->type = Ctlp_FwdU_c;
      save = formula->left;
      formula->right = FormulaCreateWithType(Ctlp_TRUE_c);
      formula->left = parent->left;
      parent->left = formula;
      parent->right = save;

      parent->top = 1;
      parent->compareValue = compareValue;
      parent->right->forward = parent;


      FormulaConvertToForward(parent->right, compareValue);
      break;

    case Ctlp_EU_c:
      /*
      ** p ^ (E q U f) --> FwdU(p, q) ^ f
      **
      **   *             *
      **  / \          /   \
      ** p  EU  =>   FwdU   f
      **    /\        / \
      **   q  f      p   q
      */

      formula->type = Ctlp_FwdU_c;
      save = formula->right;
      formula->right = formula->left;
      formula->left = parent->left;
      parent->left = formula;
      parent->right = save;

      parent->top = 1;
      parent->compareValue = compareValue;
      parent->right->forward = parent;

      FormulaConvertToForward(parent->right, compareValue);
      break;

    case Ctlp_EG_c:
      /*
      ** p ^ EGq --> FwdG(p, q)
      **
      **   *        FwdG
      **  / \        / \
      ** p  EG  =>  p   q
      **    /
      **   q
      */

      parent->type = Ctlp_FwdG_c;
      parent->right = formula->left;

      formula->left = NIL(Ctlp_Formula_t);
      formula->forward = NIL(Ctlp_Formula_t);
      Ctlp_FormulaFree(formula);

      parent->top = 1;
      parent->compareValue = compareValue;
      break;

    case Ctlp_NOT_c:
      if (formula->left->type == Ctlp_NOT_c) {
	/*
	**   *             *
	**  / \      =>   / \
	** r  NOT        r   f
	**     /
	**   NOT
	**   /
	**  f
	*/

	parent->right = formula->left->left;
	formula->left->left = NIL(Ctlp_Formula_t);
	formula->left->forward = NIL(Ctlp_Formula_t);
	Ctlp_FormulaFree(formula->left);
	formula->left = NIL(Ctlp_Formula_t);
	formula->forward = NIL(Ctlp_Formula_t);
	Ctlp_FormulaFree(formula);

	parent->top = 1;
	parent->compareValue = compareValue;
	parent->right->forward = parent;

	FormulaConvertToForward(parent->right, compareValue);
      } else if (formula->left->type == Ctlp_THEN_c) {
	/*
	**   *
	**  / \
	** r  NOT
	**    /
	**  THEN
	**   / \
	**  p   q
	*/

	p = formula->left->left;
	q = formula->left->right;
	p_qf = Ctlp_FormulaTestIsQuantifierFree(p);
	q_qf = Ctlp_FormulaTestIsQuantifierFree(q);

	if (p_qf && q_qf)
	  break;
	else if (p_qf ||
		 (p_qf == 0 && q_qf == 0 && !FormulaIsConvertible(q))) {
	  /*
	  ** !(p -> q) = p * !q
	  **
	  **   *     =>     *     =>      *
	  **  / \          / \          /   \
	  ** r  NOT       r   *        *    NOT
	  **    /            / \      / \   /
	  **  THEN          p  NOT   r   p q
	  **   / \              /
	  **  p   q            q
	  */

	  parent->left = formula->left;
	  parent->left->type = Ctlp_AND_c;
	  parent->left->left = r;
	  parent->left->right = p;
	  parent->right = formula;
	  parent->right->left = q;

	  parent->top = 1;
	  parent->compareValue = compareValue;
	  parent->right->forward = parent;

	  FormulaConvertToForward(parent->right, compareValue);
	} else {
	  /*
	  ** !(p -> q) = p * !q
	  **
	  **   *     =>     *     =>      *
	  **  / \          / \          /   \
	  ** r  NOT       r   *        *     p
	  **    /            / \      / \
	  **  THEN          p  NOT   r  NOT
	  **   / \              /       /
	  **  p   q            q       q
	  */

	  parent->left = formula->left;
	  parent->left->type = Ctlp_AND_c;
	  parent->left->left = r;
	  parent->left->right = formula;
	  parent->left->right->left = q;
	  parent->left->right->right = NIL(Ctlp_Formula_t);
	  parent->right = p;

	  parent->top = 1;
	  parent->compareValue = compareValue;
	  p->forward = parent;

	  FormulaConvertToForward(p, compareValue);
	}
      } else if (formula->left->type == Ctlp_OR_c) {
	/*
	** !(p + q) = !p * !q
	**
	**   *     =>     *     =>        *
	**  / \          / \            /   \
	** r  NOT       r   *          *    NOT
	**    /            / \        / \    /
	**    OR         NOT NOT     r  NOT g=q(p)
	**   / \          /   /         /
	**  p   q        p   q         f=p(q)
	*/

	p = formula->left->left;
	q = formula->left->right;
	p_qf = Ctlp_FormulaTestIsQuantifierFree(p);
	q_qf = Ctlp_FormulaTestIsQuantifierFree(q);

	if (p_qf && q_qf)
	  break;
	else if (p_qf) {
	  f = p;
	  g = q;
	} else if (q_qf) {
	  f = q;
	  g = p;
	} else  {
	  if (!FormulaIsConvertible(q)) {
	    f = p;
	    g = q;
	  } else {
	    f = q;
	    g = p;
	  }
	}

	parent->left = formula->left;
	parent->left->type = Ctlp_AND_c;
	parent->left->left = r;
	parent->left->right = formula;
	parent->left->right->left = f;
	parent->left->right->right = NIL(Ctlp_Formula_t);
	parent->right = FormulaCreateWithType(Ctlp_NOT_c);
	parent->right->left = g;

	parent->top = 1;
	parent->compareValue = compareValue;
	parent->right->forward = parent;

	FormulaConvertToForward(parent->right, compareValue);
      } else if (formula->left->type == Ctlp_AND_c) {
	/*
	** !(p * q) = !p + !q
	**
	**   *          *(+)           *(+)
	**  / \         / \           /    \
	** r  NOT  =>  r  OR    =>   *      *
	**    /           / \       / \    / \
	**   AND        NOT NOT    r  NOT r  NOT
	**   / \        /   /         /      /
	**  p   q      p   q         p      q
	*/

	p = formula->left->left;
	q = formula->left->right;

	if (Ctlp_FormulaTestIsQuantifierFree(p) &&
	    Ctlp_FormulaTestIsQuantifierFree(q)) {
	  break;
	}

	if (compareValue)
	  parent->type = Ctlp_OR_c;
	parent->left = formula->left;
	parent->left->left = r;
	parent->left->right = formula;
	parent->left->right->left = p;

	parent->right = FormulaCreateWithType(Ctlp_AND_c);
	parent->right->left = Ctlp_FormulaDup(r);
	parent->right->right = FormulaCreateWithType(Ctlp_NOT_c);
	parent->right->right->left = q;

	parent->top = 0;
	parent->left->top = 1;
	parent->right->top = 1;
	parent->left->compareValue = compareValue;
	parent->right->compareValue = compareValue;
	parent->left->right->forward = parent->left;
	parent->right->right->forward = parent->right;

	FormulaConvertToForward(parent->left->right, compareValue);
	FormulaConvertToForward(parent->right->right, compareValue);
      } else if (formula->left->type == Ctlp_XOR_c) {
	/*
	** !(p ^ q) = p * q + !p * !q
	**
	**   *              *(+)                   *(+)
	**  / \            /     \               /      \
	** |   |   =>     *       *      =>    *          *
	** |   |         / \     / \          / \      /     \
	** r  NOT       r   *   r   *        *   g    *      NOT
	**    /            / \     / \      / \      / \     /
	**   XOR          p   q  NOT NOT   r   f    r  NOT  g=q(p)
	**   / \                 /   /                 /
	**  p   q               p   q                 f=p(q)
	*/

	p = formula->left->left;
	q = formula->left->right;
	p_qf = Ctlp_FormulaTestIsQuantifierFree(p);
	q_qf = Ctlp_FormulaTestIsQuantifierFree(q);

	same_order = 1;
	if (p_qf && q_qf)
	  break;
	else if (p_qf) {
	  f = p;
	  g = q;
	} else if (q_qf) {
	  f = q;
	  g = p;
	} else {
	  if (FormulaIsConvertible(p) && !FormulaIsConvertible(q)) {
	    /*
	    ** !(p ^ q) = q * p + !p * !q
	    **
	    **   *              *(+)                   *(+)
	    **  / \            /     \               /      \
	    ** |   |   =>     *       *      =>    *          *
	    ** |   |         / \     / \          / \      /     \
	    ** r  NOT       r   *   r   *        *   p    *      NOT
	    **    /            / \     / \      / \      / \     /
	    **   XOR          p   q  NOT NOT   r   q    r  NOT  q
	    **   / \                 /   /                 /
	    **  p   q               p   q                 p
	    */

	    f = q;
	    g = p;
	    same_order = 0;
	  } else if (!FormulaIsConvertible(p) && FormulaIsConvertible(q)) {
	    /*
	    ** !(p ^ q) = p * q + !q * !p
	    **
	    **   *              *(+)                   *(+)
	    **  / \            /     \               /      \
	    ** |   |   =>     *       *      =>    *          *
	    ** |   |         / \     / \          / \      /     \
	    ** r  NOT       r   *   r   *        *   q    *      NOT
	    **    /            / \     / \      / \      / \     /
	    **   XOR          p   q  NOT NOT   r   p    r  NOT  p
	    **   / \                 /   /                 /
	    **  p   q               p   q                 q
	    */

	    f = p;
	    g = q;
	    same_order = 0;
	  } else {
	    f = p;
	    g = q;
	  }
	}

	if (compareValue)
	  parent->type = Ctlp_OR_c;
	parent->left = formula;
	parent->left->type = Ctlp_AND_c;
	parent->left->left->type = Ctlp_AND_c;
	parent->left->left->left = r;
	parent->left->left->right = f;
	parent->left->right = g;

	parent->right = FormulaCreateWithType(Ctlp_AND_c);
	parent->right->left = FormulaCreateWithType(Ctlp_AND_c);
	parent->right->left->left = Ctlp_FormulaDup(r);
	parent->right->left->right = FormulaCreateWithType(Ctlp_NOT_c);
	if (same_order)
	  parent->right->left->right->left = Ctlp_FormulaDup(f);
	else
	  parent->right->left->right->left = Ctlp_FormulaDup(g);
	parent->right->right = FormulaCreateWithType(Ctlp_NOT_c);
	if (same_order)
	  parent->right->right->left = Ctlp_FormulaDup(g);
	else
	  parent->right->right->left = Ctlp_FormulaDup(f);

	parent->top = 0;
	parent->left->top = 1;
	parent->right->top = 1;
	parent->left->compareValue = compareValue;
	parent->right->compareValue = compareValue;
	parent->left->right->forward = parent->left;
	parent->right->right->forward = parent->right;

	FormulaConvertToForward(parent->left->right, compareValue);
	FormulaConvertToForward(parent->right->right, compareValue);
      } else if (formula->left->type == Ctlp_EQ_c) {
	/*
	** !(EQ(p, q) = XOR(p, q)
	**
	**   *              *(+)                   *(+)
	**  / \            /     \                /     \
	** |   |   =>     *       *      =>     *         *
	** |   |         / \     / \          /   \     /   \
	** r  NOT       r   *   r   *        *     g   *    NOT
	**    /            / \     / \      / \       / \   /
	**   EQ           p  NOT NOT  q    r  NOT    r   f g=q(p)
	**   / \             /   /            /
	**  p   q           q   p            f=p(q)
	*/

	p = formula->left->left;
	q = formula->left->right;
	p_qf = Ctlp_FormulaTestIsQuantifierFree(p);
	q_qf = Ctlp_FormulaTestIsQuantifierFree(q);

	if (p_qf && q_qf)
	  break;
	else if (p_qf) {
	  f = p;
	  g = q;
	} else if (q_qf) {
	  f = q;
	  g = p;
	} else {
	  if (FormulaIsConvertible(p) && FormulaIsConvertible(q)) {
	    /*
	    ** !(EQ(p, q) = XOR(p, q)
	    **
	    **   *              *(+)                   *(+)
	    **  / \            /     \                /     \
	    ** |   |   =>     *       *      =>     *         *
	    ** |   |         / \     / \          /   \     /   \
	    ** r  NOT       r   *   r   *        *     p   *     q
	    **    /            / \     / \      / \       / \
	    **   EQ           p  NOT NOT  q    r  NOT    r  NOT
	    **   / \             /   /            /         /
	    **  p   q           q   p            q         p
	    */

	    if (compareValue)
	      parent->type = Ctlp_OR_c;
	    parent->left = parent->right;
	    parent->left->type = Ctlp_AND_c;
	    parent->left->left->type = Ctlp_AND_c;
	    parent->left->left->left = r;
	    parent->left->left->right = FormulaCreateWithType(Ctlp_NOT_c);
	    parent->left->left->right->left = q;
	    parent->left->right = p;

	    parent->right = FormulaCreateWithType(Ctlp_AND_c);
	    parent->right->left = FormulaCreateWithType(Ctlp_AND_c);
	    parent->right->left->left = Ctlp_FormulaDup(r);
	    parent->right->left->right = FormulaCreateWithType(Ctlp_NOT_c);
	    parent->right->left->right->left = Ctlp_FormulaDup(p);
	    parent->right->right = Ctlp_FormulaDup(q);

	    parent->top = 0;
	    parent->left->top = 1;
	    parent->right->top = 1;
	    parent->left->compareValue = compareValue;
	    parent->right->compareValue = compareValue;
	    parent->left->right->forward = parent->left;
	    parent->right->right->forward = parent->right;

	    FormulaConvertToForward(parent->left->right, compareValue);
	    FormulaConvertToForward(parent->right->right, compareValue);
	    break;
	  } else if (!FormulaIsConvertible(p) && !FormulaIsConvertible(q)) {
	    /*
	    ** !(EQ(p, q) = XOR(p, q)
	    **
	    **   *              *(+)                   *(+)
	    **  / \            /     \                /     \
	    ** |   |   =>     *       *      =>     *         *
	    ** |   |         / \     / \          /   \     /   \
	    ** r  NOT       r   *   r   *        *    NOT  *    NOT
	    **    /            / \     / \      / \   /   / \   /
	    **   EQ           p  NOT NOT  q    r   p q   r   q p
	    **   / \             /   /
	    **  p   q           q   p
	    */

	    if (compareValue)
	      parent->type = Ctlp_OR_c;
	    parent->left = parent->right;
	    parent->left->type = Ctlp_AND_c;
	    parent->left->left->type = Ctlp_AND_c;
	    parent->left->left->left = r;
	    parent->left->left->right = p;
	    parent->left->right = FormulaCreateWithType(Ctlp_NOT_c);
	    parent->left->right->left = q;

	    parent->right = FormulaCreateWithType(Ctlp_AND_c);
	    parent->right->left = FormulaCreateWithType(Ctlp_AND_c);
	    parent->right->left->left = Ctlp_FormulaDup(r);
	    parent->right->left->right = Ctlp_FormulaDup(q);
	    parent->right->right = FormulaCreateWithType(Ctlp_NOT_c);
	    parent->right->right->left = Ctlp_FormulaDup(p);

	    parent->top = 0;
	    parent->left->top = 1;
	    parent->right->top = 1;
	    parent->left->compareValue = compareValue;
	    parent->right->compareValue = compareValue;
	    parent->left->right->forward = parent->left;
	    parent->right->right->forward = parent->right;

	    FormulaConvertToForward(parent->left->right, compareValue);
	    FormulaConvertToForward(parent->right->right, compareValue);
	    break;
	  } else {
	    f = p;
	    g = q;
	  }
	}

	if (compareValue)
	  parent->type = Ctlp_OR_c;
	parent->left = parent->right;
	parent->left->type = Ctlp_AND_c;
	parent->left->left->type = Ctlp_AND_c;
	parent->left->left->left = r;
	parent->left->left->right = FormulaCreateWithType(Ctlp_NOT_c);
	parent->left->left->right->left = f;
	parent->left->right = g;

	parent->right = FormulaCreateWithType(Ctlp_AND_c);
	parent->right->left = FormulaCreateWithType(Ctlp_AND_c);
	parent->right->left->left = Ctlp_FormulaDup(r);
	parent->right->left->right = Ctlp_FormulaDup(f);
	parent->right->right = FormulaCreateWithType(Ctlp_NOT_c);
	parent->right->right->left = Ctlp_FormulaDup(g);

	parent->top = 0;
	parent->left->top = 1;
	parent->right->top = 1;
	parent->left->compareValue = compareValue;
	parent->right->compareValue = compareValue;
	parent->left->right->forward = parent->left;
	parent->right->right->forward = parent->right;

	FormulaConvertToForward(parent->left->right, compareValue);
	FormulaConvertToForward(parent->right->right, compareValue);
      }
      break;

    case Ctlp_THEN_c:
      /*
      ** (p -> q) = !p + q
      **
      **   *              *(+)
      **  / \            /    \
      ** r THEN  =>     *      *
      **    / \        / \    / \
      **   p   q      r  NOT r   q
      **                 /
      **                p
      */

      p = formula->left;
      q = formula->right;

      if (Ctlp_FormulaTestIsQuantifierFree(p) &&
	  Ctlp_FormulaTestIsQuantifierFree(q)) {
	break;
      }

      if (compareValue)
	parent->type = Ctlp_OR_c;
      parent->left = formula;
      parent->left->type = Ctlp_AND_c;
      parent->left->left = r;
      parent->left->right = FormulaCreateWithType(Ctlp_NOT_c);
      parent->left->right->left = p;

      parent->right = FormulaCreateWithType(Ctlp_AND_c);
      parent->right->left = Ctlp_FormulaDup(r);
      parent->right->right = q;

      parent->top = 0;
      parent->left->top = 1;
      parent->right->top = 1;
      parent->left->compareValue = compareValue;
      parent->right->compareValue = compareValue;
      parent->left->right->forward = parent->left;
      parent->right->right->forward = parent->right;

      FormulaConvertToForward(parent->left->right, compareValue);
      FormulaConvertToForward(parent->right->right, compareValue);
      break;

    case Ctlp_OR_c:
      /*
      **   *              *(+)
      **  / \            /    \
      ** r  OR   =>     *      *
      **    / \        / \    / \
      **   p   q      r   p  r   q
      */

      p = formula->left;
      q = formula->right;

      if (Ctlp_FormulaTestIsQuantifierFree(p) &&
	  Ctlp_FormulaTestIsQuantifierFree(q)) {
	break;
      }

      if (compareValue)
	parent->type = Ctlp_OR_c;
      parent->left = formula;
      parent->left->type = Ctlp_AND_c;
      parent->left->left = r;
      parent->left->right = p;

      parent->right = FormulaCreateWithType(Ctlp_AND_c);
      parent->right->left = Ctlp_FormulaDup(r);
      parent->right->right = q;

      parent->top = 0;
      parent->left->top = 1;
      parent->right->top = 1;
      parent->left->compareValue = compareValue;
      parent->right->compareValue = compareValue;
      parent->left->right->forward = parent->left;
      parent->right->right->forward = parent->right;

      FormulaConvertToForward(parent->left->right, compareValue);
      FormulaConvertToForward(parent->right->right, compareValue);
      break;

    case Ctlp_AND_c:
      /*
      **   *     =>     *
      **  / \          / \
      ** r  AND      AND  g=q(p)
      **    / \      / \
      **   p   q    r   f=p(q)
      */

      p = formula->left;
      q = formula->right;
      p_qf = Ctlp_FormulaTestIsQuantifierFree(p);
      q_qf = Ctlp_FormulaTestIsQuantifierFree(q);

      if (p_qf && q_qf)
	break;
      else if (q_qf) {
	f = q;
	g = p;
      } else {
	f = p;
	g = q;
      }

      parent->left = parent->right;
      parent->left->left = r;
      parent->left->right = f;
      parent->right = g;

      parent->top = 1;
      parent->compareValue = compareValue;
      g->forward = parent;

      FormulaConvertToForward(g, compareValue);
      break;

    case Ctlp_XOR_c:
      /*
      **   *             *(+)                  *(+)
      **  / \           /    \                /     \
      ** |   |   =>    *      *     =>      *         *
      ** |   |        / \    / \          /   \      / \
      ** r  XOR      r   *  r   *        *    NOT   *   g=q(p)
      **    / \         / \    / \      / \   /    / \
      **   p   q       q  NOT NOT p    r   f g    r  NOT
      **                  /   /                      /
      **                 p   q                      f=p(q)
      */

      p = formula->left;
      q = formula->right;
      p_qf = Ctlp_FormulaTestIsQuantifierFree(p);
      q_qf = Ctlp_FormulaTestIsQuantifierFree(q);

      if (p_qf && q_qf)
	break;
      else if (p_qf) {
	f = p;
	g = q;
      } else if (q_qf) {
	f = q;
	g = p;
      } else {
	if (FormulaIsConvertible(p) && FormulaIsConvertible(q)) {
	  /*
	  **   *             *(+)                  *(+)
	  **  / \           /    \                /     \
	  ** |   |   =>    *      *     =>      *         *
	  ** |   |        / \    / \          /   \      / \
	  ** r  XOR      r   *  r   *        *     q    *   p
	  **    / \         / \    / \      / \        / \
	  **   p   q       q  NOT NOT p    r  NOT     r  NOT
	  **                  /   /           /          /
	  **                 p   q           p          q
	  */

	  if (compareValue)
	    parent->type = Ctlp_OR_c;
	  parent->left = parent->right;
	  parent->left->type = Ctlp_AND_c;
	  parent->left->left->type = Ctlp_AND_c;
	  parent->left->left->left = r;
	  parent->left->left->right = FormulaCreateWithType(Ctlp_NOT_c);
	  parent->left->left->right->left = p;
	  parent->left->right = q;

	  parent->right = FormulaCreateWithType(Ctlp_AND_c);
	  parent->right->left = FormulaCreateWithType(Ctlp_AND_c);
	  parent->right->left->left = Ctlp_FormulaDup(r);
	  parent->right->left->right = FormulaCreateWithType(Ctlp_NOT_c);
	  parent->right->left->right->left = Ctlp_FormulaDup(q);
	  parent->right->right = Ctlp_FormulaDup(p);

	  parent->top = 0;
	  parent->left->top = 1;
	  parent->right->top = 1;
	  parent->left->compareValue = compareValue;
	  parent->right->compareValue = compareValue;
	  parent->left->right->forward = parent->left;
	  parent->right->right->forward = parent->right;

	  FormulaConvertToForward(parent->left->right, compareValue);
	  FormulaConvertToForward(parent->right->right, compareValue);
	  break;
	} else if (!FormulaIsConvertible(p) && !FormulaIsConvertible(q)) {
	  /*
	  **   *             *(+)                   *(+)
	  **  / \           /    \                /      \
	  ** |   |   =>    *      *     =>      *          *
	  ** |   |        / \    / \          /   \      /   \
	  ** r  XOR      r   *  r   *        *    NOT   *    NOT
	  **    / \         / \    / \      / \   /    / \   /
	  **   p   q       q  NOT NOT p    r   p q    r   q p
	  **                  /   /
	  **                 p   q
	  */

	  if (compareValue)
	    parent->type = Ctlp_OR_c;
	  parent->left = parent->right;
	  parent->left->type = Ctlp_AND_c;
	  parent->left->left->type = Ctlp_AND_c;
	  parent->left->left->left = r;
	  parent->left->left->right = p;
	  parent->left->right = FormulaCreateWithType(Ctlp_NOT_c);
	  parent->left->right->left = q;

	  parent->right = FormulaCreateWithType(Ctlp_AND_c);
	  parent->right->left = FormulaCreateWithType(Ctlp_AND_c);
	  parent->right->left->left = Ctlp_FormulaDup(r);
	  parent->right->left->right = Ctlp_FormulaDup(q);
	  parent->right->right = FormulaCreateWithType(Ctlp_NOT_c);
	  parent->right->right->left = Ctlp_FormulaDup(p);

	  parent->top = 0;
	  parent->left->top = 1;
	  parent->right->top = 1;
	  parent->left->compareValue = compareValue;
	  parent->right->compareValue = compareValue;
	  parent->left->right->forward = parent->left;
	  parent->right->right->forward = parent->right;

	  FormulaConvertToForward(parent->left->right, compareValue);
	  FormulaConvertToForward(parent->right->right, compareValue);
	  break;
	} else {
	  f = p;
	  g = q;
	}
      }

      if (compareValue)
	parent->type = Ctlp_OR_c;
      parent->left = parent->right;
      parent->left->type = Ctlp_AND_c;
      parent->left->left = FormulaCreateWithType(Ctlp_AND_c);
      parent->left->left->left = r;
      parent->left->left->right = f;
      parent->left->right = FormulaCreateWithType(Ctlp_NOT_c);
      parent->left->right->left = g;

      parent->right = FormulaCreateWithType(Ctlp_AND_c);
      parent->right->left = FormulaCreateWithType(Ctlp_AND_c);
      parent->right->left->left = Ctlp_FormulaDup(r);
      parent->right->left->right = FormulaCreateWithType(Ctlp_NOT_c);
      parent->right->left->right->left = Ctlp_FormulaDup(f);
      parent->right->right = Ctlp_FormulaDup(g);

      parent->top = 0;
      parent->left->top = 1;
      parent->right->top = 1;
      parent->left->compareValue = compareValue;
      parent->right->compareValue = compareValue;
      parent->left->right->forward = parent->left;
      parent->right->right->forward = parent->right;

      FormulaConvertToForward(parent->left->right, compareValue);
      FormulaConvertToForward(parent->right->right, compareValue);
      break;

    case Ctlp_EQ_c:
      /*
      **   *              *(+)                   *(+)
      **  / \            /     \               /      \
      ** |   |   =>     *       *      =>    *          *
      ** |   |         / \     / \          / \      /     \
      ** r  EQ        r   *   r   *        *   g    *      NOT
      **    /\           / \     / \      / \      / \     /
      **   p  q         p   q  NOT NOT   r   f    r  NOT  g=q(p)
      **                       /   /                 /
      **                      p   q                 f=p(q)
      */

      p = formula->left;
      q = formula->right;
      p_qf = Ctlp_FormulaTestIsQuantifierFree(p);
      q_qf = Ctlp_FormulaTestIsQuantifierFree(q);

      same_order = 1;
      if (p_qf && q_qf)
	break;
      else if (p_qf) {
	f = p;
	g = q;
      } else if (q_qf) {
	f = q;
	g = p;
      } else {
	if (FormulaIsConvertible(p) && !FormulaIsConvertible(q)) {
	  /*
	  **   *              *(+)                   *(+)
	  **  / \            /     \               /      \
	  ** |   |   =>     *       *      =>    *          *
	  ** |   |         / \     / \          / \      /     \
	  ** r  EQ        r   *   r   *        *   p    *      NOT
	  **    /\           / \     / \      / \      / \     /
	  **   p  q         p   q  NOT NOT   r   q    r  NOT  q
	  **                       /   /                 /
	  **                      p   q                 p
	  */

	  f = q;
	  g = p;
	  same_order = 0;
	} else if (!FormulaIsConvertible(p) && FormulaIsConvertible(q)) {
	  /*
	  **   *              *(+)                   *(+)
	  **  / \            /     \               /      \
	  ** |   |   =>     *       *      =>    *          *
	  ** |   |         / \     / \          / \      /     \
	  ** r  EQ        r   *   r   *        *   q    *      NOT
	  **    /\           / \     / \      / \      / \     /
	  **   p  q         p   q  NOT NOT   r   p    r  NOT  p
	  **                       /   /                 /
	  **                      p   q                 q
	  */

	  f = p;
	  g = q;
	  same_order = 0;
	} else {
	  f = p;
	  g = q;
	}
      }

      if (compareValue)
	parent->type = Ctlp_OR_c;
      parent->left = parent->right;
      parent->left->type = Ctlp_AND_c;
      parent->left->left = FormulaCreateWithType(Ctlp_AND_c);
      parent->left->left->left = r;
      parent->left->left->right = f;
      parent->left->right = g;

      parent->right = FormulaCreateWithType(Ctlp_AND_c);
      parent->right->left = FormulaCreateWithType(Ctlp_AND_c);
      parent->right->left->left = Ctlp_FormulaDup(r);
      parent->right->left->right = FormulaCreateWithType(Ctlp_NOT_c);
      if (same_order)
	parent->right->left->right->left = Ctlp_FormulaDup(f);
      else
	parent->right->left->right->left = Ctlp_FormulaDup(g);
      parent->right->right = FormulaCreateWithType(Ctlp_NOT_c);
      if (same_order)
	parent->right->right->left = Ctlp_FormulaDup(g);
      else
	parent->right->right->left = Ctlp_FormulaDup(f);

      parent->top = 0;
      parent->left->top = 1;
      parent->right->top = 1;
      parent->left->compareValue = compareValue;
      parent->right->compareValue = compareValue;
      parent->left->right->forward = parent->left;
      parent->right->right->forward = parent->right;

      FormulaConvertToForward(parent->left->right, compareValue);
      FormulaConvertToForward(parent->right->right, compareValue);
      break;

    case Ctlp_AX_c:
    case Ctlp_AG_c:
    case Ctlp_AF_c:
    case Ctlp_AU_c:
      fprintf(vis_stderr,
	"** ctl error : invalid node type in converting to forward formula.\n"
	      );
      assert(0);
      break;

    case Ctlp_ID_c:
    case Ctlp_TRUE_c:
    case Ctlp_FALSE_c:
      break;

    default:
      fail("Unexpected type");
  }
}


/**Function********************************************************************

  Synopsis    [Inserts compare nodes into forward formula.]

  Description [Inserts compare nodes into forward formula.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
static void
FormulaInsertForwardCompareNodes(Ctlp_Formula_t *formula,
				 Ctlp_Formula_t *parent,
				 int compareValue)
{
  Ctlp_Formula_t *new_;

  if (formula->top) {
    if (!parent) {
      new_ = FormulaCreateWithType(Ctlp_Cmp_c);
      memcpy((void *)new_, (void *)formula, sizeof(Ctlp_Formula_t));
      formula->type = Ctlp_Cmp_c;
      formula->left = new_;
      formula->right = FormulaCreateWithType(Ctlp_FALSE_c);
      formula->compareValue = compareValue;
    } else {
      new_ = FormulaCreateWithType(Ctlp_Cmp_c);
      new_->left = formula;
      new_->right = FormulaCreateWithType(Ctlp_FALSE_c);
      new_->compareValue = compareValue;
      if (parent->left == formula)
	parent->left = new_;
      else
	parent->right = new_;
    }
    return;
  }
  FormulaInsertForwardCompareNodes(formula->left, formula, compareValue);
  FormulaInsertForwardCompareNodes(formula->right, formula, compareValue);
}


/**Function********************************************************************

  Synopsis    [Determines which conversion formula is better.]

  Description [Determines which conversion formula is better between
  init ^ f != FALSE and init ^ !f == FALSE.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
static int
FormulaGetCompareValue(Ctlp_Formula_t *formula)
{
  int	compareValue;

  if (FormulaIsConvertible(formula))
    compareValue = 1; /* check whether the result is not FALSE */
  else
    compareValue = 0; /* check whether the result is FALSE. */

  return(compareValue);
}


/**Function********************************************************************

  Synopsis    [Checks whether a formula can be converted to forward further.]

  Description [Checks whether a formula can be converted to forward further.
  It returns 1 if it is convertible, and returns 0 if it is not convertible,
  and returns 2 if it is partially convertible.]

  SideEffects []

  SeeAlso     []

******************************************************************************/
static int
FormulaIsConvertible(Ctlp_Formula_t *formula)
{
  int	convertible;
  int	value1, value2;

  if (Ctlp_FormulaTestIsQuantifierFree(formula))
    convertible = 1;
  else if (formula->type == Ctlp_ID_c)
    convertible = 1;
  else if (formula->type == Ctlp_EX_c ||
	   formula->type == Ctlp_EU_c ||
	   formula->type == Ctlp_EF_c ||
	   formula->type == Ctlp_EG_c) {
    convertible = 1;
  } else if (formula->type == Ctlp_NOT_c) {
    if (formula->left->type == Ctlp_NOT_c) {
      if (formula->left->left->type == Ctlp_EX_c ||
	  formula->left->left->type == Ctlp_EU_c ||
	  formula->left->left->type == Ctlp_EF_c ||
	  formula->left->left->type == Ctlp_EG_c) {
	convertible = 1;
      } else {
	convertible = FormulaIsConvertible(formula->left->left);
      }
    } else {
      if (formula->left->type == Ctlp_EX_c ||
	  formula->left->type == Ctlp_EU_c ||
	  formula->left->type == Ctlp_EF_c ||
	  formula->left->type == Ctlp_EG_c) {
	convertible = 0;
      } else {
	value1 = FormulaIsConvertible(formula->left);
	value2 = FormulaIsConvertible(formula->right);
	switch (formula->type) {
	case Ctlp_AND_c:
	case Ctlp_OR_c:
	  convertible = (value1 & 0x1) * (value2 & 0x1);
	  break;
	case Ctlp_THEN_c:
	  convertible = value1 * (value2 & 0x1);
	  break;
	default :
	  convertible = 2;
	  break;
	}
      }
    }
  } else {
    value1 = FormulaIsConvertible(formula->left);
    value2 = FormulaIsConvertible(formula->right);
    switch (formula->type) {
    case Ctlp_AND_c:
    case Ctlp_OR_c:
      convertible = value1 * value2;
      break;
    case Ctlp_THEN_c:
      convertible = (value1 & 0x1) * value2;
      break;
    default :
      convertible = 2;
      break;
    }
  }

  return(convertible);
}


/**Function********************************************************************

  Synopsis    [Replaces a simple formula with bottom.]

  Description [This function is used in generating the witness formula from a
  w-ACTL formula in Beer's approach to vacuity detection.  Replaces a simple
  (propositional) subformula with bottom, which is FALSE for even negation
  parity and TRUE for odd negation parity.  An exception is made when the top
  node of the subformula is an implication.  In that case, the top node and the
  antecedent (left child) are retained in the replacement, while the
  consequent (right child) is replaced by bottom.]

  SideEffects [none]

  SeeAlso     [Ctlp_FormulaCreateWitnessFormula]

******************************************************************************/
static Ctlp_Formula_t *
ReplaceSimpleFormula(
  Ctlp_Formula_t * formula)
{
  Ctlp_Formula_t *newFormula;

  assert(formula->negation_parity == Ctlp_Even_c ||
	 formula->negation_parity == Ctlp_Odd_c);

  if (Ctlp_FormulaReadType(formula) == Ctlp_THEN_c) {
    Ctlp_Formula_t *leftFormula, *rightFormula;
    Ctlp_Formula_t *newLeftFormula, *newRightFormula;

    leftFormula = Ctlp_FormulaReadLeftChild(formula);
    rightFormula = Ctlp_FormulaReadRightChild(formula);
    newLeftFormula = Ctlp_FormulaDup(leftFormula);
    newRightFormula = ReplaceSimpleFormula(rightFormula);
    return Ctlp_FormulaCreate(Ctlp_THEN_c,newLeftFormula,newRightFormula);
  }

  fprintf(vis_stderr, "Replacing formula ");
  Ctlp_FormulaPrint(vis_stderr, formula);
  if (formula->negation_parity == Ctlp_Even_c){
    fprintf(vis_stderr, " by FALSE\n");
    newFormula = FormulaCreateWithType(Ctlp_FALSE_c);
  } else{
    fprintf(vis_stderr, " by TRUE\n");
    newFormula = FormulaCreateWithType(Ctlp_TRUE_c);
  }
  newFormula->negation_parity = formula->negation_parity;
  return(newFormula);
}
