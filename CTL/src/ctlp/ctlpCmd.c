/**CFile***********************************************************************

  FileName    [ctlpCmd.c]

  PackageName [ctlp]

  Synopsis    [Command to read in a file containing CTL formulas.]

  Author      [Tom Shiple, In-Ho Moon]

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

#include "ctlpInt.h"
#include "visfd.h"

static char rcsid[] UNUSED = "$Id: ctlpCmd.c,v 1.14 2005/05/19 02:35:25 awedh Exp $";

/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/



/**AutomaticEnd***************************************************************/


/*---------------------------------------------------------------------------*/
/* Definition of exported functions                                          */
/*---------------------------------------------------------------------------*/
/**Function********************************************************************

  Synopsis    [Initializes the CTL parser package.]

  SideEffects []

  SeeAlso     [Ctlp_End]

******************************************************************************/
void
Ctlp_Init(void)
{
//  Cmd_CommandAdd("_ctlp_test",   CommandCtlpTest,   0);
}


/**Function********************************************************************

  Synopsis    [Ends the CTL parser package.]

  SideEffects []

  SeeAlso     [Ctlp_Init]

******************************************************************************/
void
Ctlp_End(void)
{
}


/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [Sets the field states in every subformula of formula to
  NULL.] 

  Description [The function sets the field states in every subformula
  of formula to NULL.]
  
  SideEffects []

******************************************************************************/
void
CtlpFormulaSetStatesToNULL(
  Ctlp_Formula_t *formula)
{
  if(formula!=NIL(Ctlp_Formula_t)) {
    formula->forward = NIL(Ctlp_Formula_t);
    if(formula->type != Ctlp_ID_c) {
      CtlpFormulaSetStatesToNULL(formula->left);
      CtlpFormulaSetStatesToNULL(formula->right);
    }
  }
}


/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [Implements the _ctlp_test command.]

  Description [Implements the _ctlp_test command.  This command is only meant to
  test the CTL command parser.]
  
  CommandName [_ctlp_test]

  CommandSynopsis [test the CTL parser]

  CommandArguments [\[-h\] &lt;file_name&gt;]
  
  CommandDescription [Test the CTL parser.  If the entire file of CTL
  formulas is successfully parsed, then each formula is printed to
  stdout, followed by the equivalent existential normal form formula.
  The formulas read are not stored.  For the input file containing:
  <pre> AG(foo=bar); </pre> the following is produced: <pre> original
  formula: AG(foo=bar) => equivalent existential formula: !(E(TRUE U
  !(foo=bar))) </pre> For the syntax of CTL formulas, refer to <A
  HREF="../ctl/ctl/ctl.html"> the VIS CTL and LTL syntax manual</A>.

  Command options:<p>  

  <dl><dt> -h
  <dd> Print the command usage.
  </dl>
  ]

  SideEffects []

******************************************************************************/
int
CommandCtlpTest(
  int  argc,
  char ** argv)
{
  int     c;
  int      i;
  FILE    *fp;
  array_t *formulaArray;
  array_t *convertedArray;
  array_t *existentialConvertedArray;
  array_t *forwardExistentialArray = NIL(array_t);
  int     forwardTraversal = 0;
  
  /*
   * Parse command line options.
   */
  util_getopt_reset();
  while ((c = util_getopt(argc, argv, "Fh")) != EOF) {
    switch(c) {
      case 'F':
        forwardTraversal = 1;
        break;
      case 'h':
        goto usage;
      default:
        goto usage;
    }
  }

  /*
   * Open the ctl file.
   */
  if (argc - util_optind == 0) {
    (void) fprintf(vis_stderr, "** ctl error: ctl file not provided\n");
    goto usage;
  }
  else if (argc - util_optind > 1) {
    (void) fprintf(vis_stderr, "** ctl error: too many arguments\n");
    goto usage;
  }

  fp = fopen(argv[util_optind], "r");
  if (fp == NIL(FILE)) {
    (void)fprintf(vis_stderr, "** ctl error: Cannot open the file %s\n", argv[util_optind]);
    return 1;
  }

  /*
   * Parse the formulas in the file.
   */
  formulaArray = Ctlp_FileParseFormulaArray(fp);
  (void) fclose(fp);

  if (formulaArray == NIL(array_t)) {
    fflush(vis_stdout);
    fflush(vis_stderr);
    return 1;
  }

  if (forwardTraversal)
    forwardExistentialArray =
      Ctlp_FormulaArrayConvertToForward(formulaArray, 1, FALSE);

  convertedArray = Ctlp_FormulaArrayConvertToDAG(formulaArray);
  array_free(formulaArray);
  existentialConvertedArray =
      Ctlp_FormulaDAGConvertToExistentialFormDAG(convertedArray);
  
  /*
   * Print each original formula and its corresponding converted formula.
   */
  for (i = 0; i < array_n(convertedArray); i++) {
    Ctlp_Formula_t *formula;

    formula = array_fetch(Ctlp_Formula_t *, convertedArray, i);
    (void) fprintf(vis_stdout, "original formula: ");
    Ctlp_FormulaPrint(vis_stdout, formula);
    (void) fprintf(vis_stdout, "\n");

    formula = array_fetch(Ctlp_Formula_t *, existentialConvertedArray, i);
    (void) fprintf(vis_stdout, "=> equivalent existential formula: ");
    Ctlp_FormulaPrint(vis_stdout, formula);
    (void) fprintf(vis_stdout, "\n");

    if (forwardTraversal) {
      formula = array_fetch(Ctlp_Formula_t *, forwardExistentialArray, i);
      (void) fprintf(vis_stdout, "=> equivalent forward existential formula: ");
      Ctlp_FormulaPrint(vis_stdout, formula);
      (void) fprintf(vis_stdout, "\n");
    }
  }

/*   (void)fprintf(vis_stdout, "No. of subformulae (including formulae) = %d\n", */
/*                 FormulaArrayCountSubformulae(existentialConvertedArray)); */
/*   if (forwardTraversal) { */
/*     (void)fprintf(vis_stdout, */
/*                   "No. of forward subformulae (including formulae) = %d\n", */
/*                   FormulaArrayCountSubformulae(forwardExistentialArray)); */
/*   } */


  Ctlp_FormulaArrayFree(convertedArray);
  Ctlp_FormulaArrayFree(existentialConvertedArray);
  if (forwardTraversal)
    Ctlp_FormulaArrayFree(forwardExistentialArray);
  
  fflush(vis_stdout);
  fflush(vis_stderr);

  return 0;		/* normal exit */

usage:
  (void) fprintf(vis_stderr, "usage: _ctlp_test file [-h]\n");
  (void) fprintf(vis_stderr, "   -h  print the command usage\n");
  return 1;		/* error exit */
}


  
  








