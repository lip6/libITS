/**CHeaderFile*****************************************************************

  FileName    [ctlp.h]

  PackageName [ctlp]

  Synopsis    [Routines for parsing, writing and accessing CTL formulas.]

  Description [This package implements a parser for CTL (Computation Tree
  Logic) formulas.  CTL is a language used to describe properties of systems.
  For the syntax of CTL formulas, refer to <A HREF="../ctl_ltl/ctl_ltl/ctl_ltl.html">
  the VIS CTL and LTL syntax manual</A>.]

  SeeAlso     [mc]

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

  Revision    [$Id: ctlp.h,v 1.29 2005/05/08 14:51:46 fabio Exp $]

******************************************************************************/

#ifndef _CTLP
#define _CTLP

//#include "vm.h"
#include "array.h"
#include <stdio.h>

/*---------------------------------------------------------------------------*/
/* Structure declarations                                                    */
/*---------------------------------------------------------------------------*/

/**Enum************************************************************************

  Synopsis    [Types of operators allowed in CTL formulas.]

  Description [The types of nodes in a CTL formula parse tree. ID, TRUE, and
  FALSE are leaves, and all others are internal nodes.]

  SeeAlso     [Ctlp_FormulaReadType]

******************************************************************************/
typedef enum {
  Ctlp_EX_c,     /* there exists a next state */
  Ctlp_EG_c,     /* there exists a path globally */
  Ctlp_EF_c,     /* there exists a path finally */
  Ctlp_EU_c,     /* there exists a path, until (not symmetric in args) */ 
  Ctlp_AX_c,     /* for all next states */
  Ctlp_AG_c,     /* for all paths globally */
  Ctlp_AF_c,     /* for all paths finally */
  Ctlp_AU_c,     /* for all paths, until (not symmetric in args) */
  Ctlp_OR_c,     /* Boolean disjunction */
  Ctlp_AND_c,    /* Boolean conjunction */
  Ctlp_NOT_c,    /* Boolean negation*/
  Ctlp_THEN_c,   /* Boolean implies (not symmetric) */
  Ctlp_XOR_c,    /* Boolean not equal */
  Ctlp_EQ_c,     /* Boolean equal */
  Ctlp_ID_c,     /* an atomic proposition */
  Ctlp_TRUE_c,   /* tautology (used only for translation to exist. form) */
  Ctlp_FALSE_c,  /* empty (used only for translation to exist. form) */
  Ctlp_FwdU_c,   /* forward until */
  Ctlp_FwdG_c,   /* forward global */
  Ctlp_EY_c,	 /* there exists a previous state */
  Ctlp_EH_c,	 /* there exists a path globally in the past */
  Ctlp_Init_c,	 /* initial states */
  Ctlp_Cmp_c	 /* compare two leaves in forward CTL */
} Ctlp_FormulaType;

  
/**Enum************************************************************************

  Synopsis    [Classification of CTL formulae according to quantification.]

  Description [A CTL formula is quantifier-free if it contains no
  quantifiers.  A CTL formula is classifed in ECTL if it contains quantifiers
  and every existential quantifier is under an even number of negations, while
  every universal quantifier is under an odd number of negations.  For an ACTL
  formula, the negation parity is exchanged.  All remaining formulae are
  mixed.]

  SeeAlso     []

******************************************************************************/
typedef enum {
  Ctlp_ECTL_c,
  Ctlp_ACTL_c,
  Ctlp_Mixed_c,
  Ctlp_QuantifierFree_c
} Ctlp_FormulaClass;


/**Enum************************************************************************

  Synopsis    [Classification of ACTL formulae according to Beer et al..]

  Description [w-ACTL is the subset of ACTL to which the vacuity detection
  algorithm of Beer et al. (CAV 97) is applicable.  An ACTL formula is in
  w-ACTL if at least one child of every binary operator is propositional.
  A simple formula is a propositional, while a state formula is a
  non-propositional w-ACTL.]

  SeeAlso     []

******************************************************************************/
typedef enum {
    Ctlp_WACTLsimple_c,
    Ctlp_WACTLstate_c,
    Ctlp_NONWACTL_c
} Ctlp_FormulaACTLSubClass;


/**Enum************************************************************************

  Synopsis    [Types of approximations that are possible]

  Description [We can have overapproximations, underapproximations, exact
  approximations, or incomparable sets.  Question: is ctlp the right place for
  this?  It is used in ctlp, mc, rch. (RB)]

  SeeAlso     []

******************************************************************************/
typedef enum {
  Ctlp_Incomparable_c,
  Ctlp_Underapprox_c,
  Ctlp_Overapprox_c,
  Ctlp_Exact_c
} Ctlp_Approx_t;


/**Enum************************************************************************

  Synopsis    [Negation parity of a node.]

  Description [It can be even, odd, or both.  The fourth value is for
  uninitialized.  For a formula with no sharing, XOR and EQ nodes cause
  their descendants to have both parities.]

  SeeAlso     []

******************************************************************************/
typedef enum {
  Ctlp_NoParity_c,
  Ctlp_Even_c,
  Ctlp_Odd_c,
  Ctlp_EvenOdd_c
} Ctlp_Parity_t;


/*---------------------------------------------------------------------------*/
/* Type declarations                                                         */
/*---------------------------------------------------------------------------*/
typedef struct CtlpFormulaStruct Ctlp_Formula_t;
typedef void   (*Ctlp_DbgInfoFreeFn) (Ctlp_Formula_t *);
typedef array_t Ctlp_FormulaArray_t; /* array of Ctlp_Formula_t * */

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Function prototypes                                                       */
/*---------------------------------------------------------------------------*/

EXTERN void Ctlp_Init(void);
EXTERN void Ctlp_End(void);
EXTERN array_t * Ctlp_FileParseFormulaArray(FILE * fp);
EXTERN char * Ctlp_FormulaConvertToString(Ctlp_Formula_t * formula);
EXTERN void Ctlp_FormulaPrint(FILE * fp, Ctlp_Formula_t * formula);
EXTERN Ctlp_FormulaType Ctlp_FormulaReadType(Ctlp_Formula_t * formula);
EXTERN int Ctlp_FormulaReadCompareValue(Ctlp_Formula_t * formula);
EXTERN char * Ctlp_FormulaReadVariableName(Ctlp_Formula_t * formula);
EXTERN char * Ctlp_FormulaReadValueName(Ctlp_Formula_t * formula);
EXTERN Ctlp_Formula_t * Ctlp_FormulaReadLeftChild(Ctlp_Formula_t * formula);
EXTERN Ctlp_Formula_t * Ctlp_FormulaReadRightChild(Ctlp_Formula_t * formula);
EXTERN void Ctlp_FormulaSetDbgInfo(Ctlp_Formula_t * formula, void *data, Ctlp_DbgInfoFreeFn freeFn);
EXTERN void * Ctlp_FormulaReadDebugData(Ctlp_Formula_t * formula);
EXTERN int Ctlp_FormulaTestIsConverted(Ctlp_Formula_t * formula);
EXTERN int Ctlp_FormulaTestIsQuantifierFree(Ctlp_Formula_t *formula);
EXTERN Ctlp_Formula_t * Ctlp_FormulaReadOriginalFormula(Ctlp_Formula_t * formula);
EXTERN void Ctlp_FormulaFree(Ctlp_Formula_t *formula);
EXTERN void Ctlp_FlushStates(Ctlp_Formula_t * formula);
EXTERN Ctlp_Formula_t * Ctlp_FormulaDup(Ctlp_Formula_t * formula);
EXTERN Ctlp_Formula_t * Ctlp_FormulaConverttoComplement(Ctlp_Formula_t * formula);
EXTERN Ctlp_Formula_t * Ctlp_FormulaConvertAFtoAU(Ctlp_Formula_t * formula);
EXTERN Ctlp_Formula_t * Ctlp_FormulaConvertEFtoOR(Ctlp_Formula_t * formula);
EXTERN Ctlp_Formula_t * Ctlp_FormulaConvertEUtoOR(Ctlp_Formula_t * formula);
EXTERN Ctlp_Formula_t * Ctlp_FormulaConvertXORtoOR(Ctlp_Formula_t * formula);
EXTERN Ctlp_Formula_t * Ctlp_FormulaConvertEQtoOR(Ctlp_Formula_t * formula);
EXTERN Ctlp_Formula_t * Ctlp_FormulaPushNegation(Ctlp_Formula_t * formula);
EXTERN Ctlp_Formula_t * Ctlp_ConvertToCmpFormula(Ctlp_Formula_t * formula);
EXTERN void Ctlp_FormulaArrayFree(array_t * formulaArray);
EXTERN Ctlp_Formula_t * Ctlp_FormulaConvertToExistentialForm(Ctlp_Formula_t * formula);
EXTERN Ctlp_Formula_t * Ctlp_FormulaConvertToSimpleExistentialForm(Ctlp_Formula_t * formula);
EXTERN array_t * Ctlp_FormulaArrayConvertToExistentialFormTree(array_t * formulaArray);
EXTERN array_t * Ctlp_FormulaArrayConvertToSimpleExistentialFormTree(array_t * formulaArray);
EXTERN array_t * Ctlp_FormulaArrayConvertToDAG(array_t *formulaArray);
EXTERN array_t * Ctlp_FormulaDAGConvertToExistentialFormDAG(array_t *formulaDAG);
EXTERN Ctlp_Formula_t * Ctlp_FormulaCreate(Ctlp_FormulaType type, void * left, void * right);
EXTERN Ctlp_Formula_t * Ctlp_FormulaCreateOr(char *name, char *valueStr);
EXTERN Ctlp_Formula_t * Ctlp_FormulaCreateVectorAnd(char *nameVector, char *value);
EXTERN Ctlp_Formula_t * Ctlp_FormulaCreateVectorOr(char *nameVector, char *valueStr);
EXTERN Ctlp_Formula_t * Ctlp_FormulaCreateEquiv(char *left, char *right);
EXTERN Ctlp_Formula_t * Ctlp_FormulaCreateVectorEquiv(char *left, char *right);
EXTERN Ctlp_Formula_t * Ctlp_FormulaCreateAXMult(char *string, Ctlp_Formula_t *formula);
EXTERN Ctlp_Formula_t * Ctlp_FormulaCreateEXMult(char *string, Ctlp_Formula_t *formula);
EXTERN array_t * Ctlp_FormulaArrayConvertToForward(array_t *formulaArray, int singleInitial, int doNotShareFlag);
EXTERN char * Ctlp_FormulaGetTypeString(Ctlp_Formula_t *formula);
EXTERN Ctlp_FormulaClass Ctlp_CheckClassOfExistentialFormula(Ctlp_Formula_t *formula);
EXTERN Ctlp_FormulaClass Ctlp_CheckClassOfExistentialFormulaArray(array_t *formulaArray);
EXTERN int Ctlp_FormulaIdentical(Ctlp_Formula_t *formula1, Ctlp_Formula_t *formula2);
EXTERN void Ctlp_FormulaMakeMonotonic(Ctlp_Formula_t *formula);
EXTERN void Ctlp_FormulaArrayMakeMonotonic(array_t *formulaArray);
EXTERN void Ctlp_FormulaNegations(Ctlp_Formula_t *formula, Ctlp_Parity_t parity);
EXTERN Ctlp_FormulaACTLSubClass Ctlp_CheckIfWACTL(Ctlp_Formula_t *formula);
EXTERN Ctlp_Formula_t * Ctlp_FormulaCreateWitnessFormula(Ctlp_Formula_t *formula);

/**AutomaticEnd***************************************************************/

#endif /* _CTLP */
