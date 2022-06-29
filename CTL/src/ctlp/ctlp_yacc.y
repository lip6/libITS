%{
/**CFile*****************************************************************

  FileName    [ctlp.y]

  PackageName [ctlp]

  Synopsis    [Yacc for CTL formula parser.]

  SeeAlso     [ctlp.h]

  Author      [Gary York, Ramin Hojati, Tom Shiple, Yuji Kukimoto
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

#include "ctlpInt.h"

static char rcsid[] UNUSED = "$Id: ctlp.y,v 1.19 2009/04/11 01:30:00 fabio Exp $";

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/

#include "visfd.h"
#include "ctlp.h"

extern int CtlpYylex ();
extern void CtlpYyerror(const char * errmsg);
extern int CtlpYylineno;
extern char * CtlpYytext;

%}

/*---------------------------------------------------------------------------*/
/*      Grammar declarations                                                 */
/*---------------------------------------------------------------------------*/

%union {
  Ctlp_Formula_t *sf;	/* state formula */
  char *str;
}

%type <sf> error stateformula formula
%type <str> name ax_mult ex_mult comparator

%error_verbose 

%token TOK_DEFINE
%token TOK_MACRO
%token TOK_ID
%token QUOTED_TOK_ID
%token TOK_ID2
%token TOK_ID_VECTOR
%token TOK_ID_UNION
%token TOK_COMMA
%token TOK_EQIV
%token TOK_FORALL
%token TOK_EXISTS
%token TOK_UNTIL
%token TOK_FORALL_NEXT
%token TOK_FORALL_EVENTUALLY
%token TOK_FORALL_GLOBALLY
%token TOK_EXISTS_NEXT
%token TOK_EXISTS_EVENTUALLY
%token TOK_EXISTS_GLOBALLY
%token TOK_FORALL_NEXT_MULT
%token TOK_EXISTS_NEXT_MULT
%token TOK_THEN
%token TOK_EQ
%token TOK_XOR
%token TOK_AND
%token TOK_OR
%token TOK_NOT
%token TOK_ASSIGN
%token TOK_LT
%token TOK_NEQ
%token TOK_GT
%token TOK_LEQ
%token TOK_GEQ
%token TOK_TRUE
%token TOK_FALSE

/* precedence is specified here from lowest to highest */
%nonassoc TOK_UNTIL
%left TOK_THEN
%left TOK_EQ
%left TOK_XOR
%left TOK_OR
%left TOK_AND
%nonassoc TOK_FORALL_NEXT TOK_FORALL_EVENTUALLY TOK_FORALL_GLOBALLY TOK_EXISTS_NEXT TOK_EXISTS_EVENTUALLY TOK_EXISTS_GLOBALLY
%nonassoc TOK_NOT

%%

/*---------------------------------------------------------------------------*/
/*      Grammar rules                                                        */
/*---------------------------------------------------------------------------*/
formulas     : /* nothing */
	     | formulas formula
	     ;

formula : stateformula ';'
	       { CtlpFormulaAddToGlobalArray($1);
		       $$ = $1;
	   /*
	    * The formula is echoed by the lexical analyzer to stdout.
	    * Here we are just printing the line number after it.
	    */
	   CtlpGlobalFormula = NIL(Ctlp_Formula_t);
	 }
       | TOK_DEFINE name stateformula
	 { if (!CtlpFormulaAddToTable($2, $3, CtlpMacroTable)){
	     CtlpYyerror("** ctl error : MACRO ERROR");
			 (void) fprintf(vis_stderr, "Macro \"%s\" is already in table.\n\n",$2);
	     Ctlp_FormulaFree(CtlpGlobalFormula);
	     $$ = NULL;
	   }else{
	     $$ = $3;
	   }
	   CtlpGlobalFormula = NIL(Ctlp_Formula_t);
	 }
	     | error ';'
	       { $$ = NULL;
	   /*
	    * Error was detected reading the formula. Garbage collect
	    * and print error message.
	    */
		       Ctlp_FormulaFree(CtlpGlobalFormula);
		       (void) fprintf(vis_stderr, "** ctl error : Invalid CTL formula, line %d\n\n", CtlpYylineno);
	   CtlpGlobalFormula = NIL(Ctlp_Formula_t);
	 }
	     ;

stateformula : '(' stateformula ')'
       { $$ = $2;
	 CtlpGlobalFormula= $$;}
     | TOK_EXISTS '(' stateformula TOK_UNTIL stateformula ')'
       { $$ = Ctlp_FormulaCreate(Ctlp_EU_c, $3, $5);
	 CtlpGlobalFormula= $$; }
     | TOK_EXISTS_NEXT stateformula
       { $$ = Ctlp_FormulaCreate(Ctlp_EX_c, $2, NIL(Ctlp_Formula_t));
	 CtlpGlobalFormula= $$;}
     | ex_mult '(' stateformula ')'
       { $$ = Ctlp_FormulaCreateEXMult($1, $3);
	 if ($$ == NIL(Ctlp_Formula_t)){
	    CtlpYyerror("** ctl error : MULTIPLE EX ERROR");
	    (void) fprintf(vis_stderr,"Error during parsing line %d.\n\n", CtlpYylineno);
	    Ctlp_FormulaFree(CtlpGlobalFormula);
	 }
	 FREE($1);
	 CtlpGlobalFormula= $$;
       }
     | TOK_EXISTS_EVENTUALLY stateformula
       { $$ = Ctlp_FormulaCreate(Ctlp_EF_c, $2, NIL(Ctlp_Formula_t));
	 CtlpGlobalFormula= $$; }
     | TOK_EXISTS_GLOBALLY stateformula
       { $$ = Ctlp_FormulaCreate(Ctlp_EG_c, $2, NIL(Ctlp_Formula_t));
	 CtlpGlobalFormula= $$;}
     | TOK_FORALL '(' stateformula TOK_UNTIL stateformula ')'
       { $$ = Ctlp_FormulaCreate(Ctlp_AU_c, $3, $5);
	 CtlpGlobalFormula= $$; }
     | TOK_FORALL_NEXT stateformula
       { $$ = Ctlp_FormulaCreate(Ctlp_AX_c, $2, NIL(Ctlp_Formula_t));
	 CtlpGlobalFormula= $$; }
     | ax_mult '(' stateformula ')'
       { $$ = Ctlp_FormulaCreateAXMult($1, $3);
	 if ($$ == NIL(Ctlp_Formula_t)){
	    CtlpYyerror("** ctl error : MULTIPLE AX ERROR");
	    (void) fprintf(vis_stderr,"Error during parsing line %d.\n\n", CtlpYylineno);
	    Ctlp_FormulaFree(CtlpGlobalFormula);
	 }
	 FREE($1);
	 CtlpGlobalFormula= $$;
       }
     | TOK_FORALL_EVENTUALLY stateformula
       { $$ = Ctlp_FormulaCreate(Ctlp_AF_c, $2, NIL(Ctlp_Formula_t));
	 CtlpGlobalFormula= $$; }
     | TOK_FORALL_GLOBALLY stateformula
       { $$ = Ctlp_FormulaCreate(Ctlp_AG_c, $2, NIL(Ctlp_Formula_t));
	 CtlpGlobalFormula= $$; }
     | stateformula TOK_AND stateformula
       { $$ = Ctlp_FormulaCreate(Ctlp_AND_c, $1, $3);
	 CtlpGlobalFormula= $$; }
     | stateformula TOK_OR stateformula
       { $$ = Ctlp_FormulaCreate(Ctlp_OR_c, $1, $3);
	 CtlpGlobalFormula= $$; }
     | TOK_NOT stateformula
       { $$ = Ctlp_FormulaCreate(Ctlp_NOT_c, $2, NIL(Ctlp_Formula_t));
	 CtlpGlobalFormula= $$; }
     | stateformula TOK_THEN stateformula
       { $$ = Ctlp_FormulaCreate(Ctlp_THEN_c, $1, $3);
	 CtlpGlobalFormula= $$; }
     | stateformula TOK_EQ stateformula
       { $$ = Ctlp_FormulaCreate(Ctlp_EQ_c, $1, $3);
	 CtlpGlobalFormula= $$; }
     | stateformula TOK_XOR stateformula
       { $$ = Ctlp_FormulaCreate(Ctlp_XOR_c, $1, $3);
	 CtlpGlobalFormula= $$; }    
     | name comparator name
       { $$ = Ctlp_FormulaCreate(Ctlp_ID_c, $1, util_strcat3($2,"",$3));
       FREE($3);
       FREE($2);
	 CtlpGlobalFormula= $$; }    
     | name TOK_EQIV name
       { $$ = Ctlp_FormulaCreateEquiv($1, $3);
	 if ($$ == NIL(Ctlp_Formula_t)){
	    CtlpYyerror("** ctl error : Matching ERROR");
	    (void) fprintf(vis_stderr,"LHS token is not matched to RHS token, line %d.\n\n", CtlpYylineno);
	    Ctlp_FormulaFree(CtlpGlobalFormula);
	 }
	 FREE($1);
	 FREE($3);
	 CtlpGlobalFormula= $$;
       }    
     | name comparator TOK_FORALL
       { $$ = Ctlp_FormulaCreate(Ctlp_ID_c, $1, util_strcat3($2,"","A"));
	 FREE($2);
	 CtlpGlobalFormula= $$; }
     | name comparator TOK_EXISTS
       { $$ = Ctlp_FormulaCreate(Ctlp_ID_c, $1, util_strcat3($2,"","E"));
	 FREE($2);
	 CtlpGlobalFormula= $$; }
     | name comparator TOK_UNTIL
       { $$ = Ctlp_FormulaCreate(Ctlp_ID_c, $1, util_strcat3($2,"","U"));
	 FREE($2);
	 CtlpGlobalFormula= $$; }
     | TOK_FORALL comparator name
       { $$ = Ctlp_FormulaCreate(Ctlp_ID_c, util_strsav("A"), util_strcat3($2,"",$3));
	 FREE($2);
	 FREE($3);
	 CtlpGlobalFormula= $$; }
     | TOK_EXISTS comparator name
       { $$ = Ctlp_FormulaCreate(Ctlp_ID_c, util_strsav("E"), util_strcat3($2,"",$3));
	 FREE($2);
	 FREE($3);
	 CtlpGlobalFormula= $$; }
     | TOK_UNTIL comparator name
       { $$ = Ctlp_FormulaCreate(Ctlp_ID_c, util_strsav("U"), util_strcat3($2,"",$3));
	 FREE($2);
	 FREE($3);
	 CtlpGlobalFormula= $$; }
     | TOK_TRUE
       { $$ = Ctlp_FormulaCreate(Ctlp_TRUE_c, NIL(Ctlp_Formula_t), NIL(Ctlp_Formula_t));
	 CtlpGlobalFormula= $$; }
     | TOK_FALSE
       { $$ = Ctlp_FormulaCreate(Ctlp_FALSE_c, NIL(Ctlp_Formula_t), NIL(Ctlp_Formula_t));
	 CtlpGlobalFormula= $$; }
     | name
       {
	 // empty var name, just copy whole thing in.
	 $$ = Ctlp_FormulaCreate(Ctlp_ID_c, util_strsav($1), util_strsav(""));
	 CtlpGlobalFormula= $$;
	 }
;

name:  TOK_ID
       { $$ = util_strsav(CtlpYytext); }
     | TOK_ID2
       { (void) CtlpChangeBracket(CtlpYytext);
	 $$ = util_strsav(CtlpYytext); };
     | QUOTED_TOK_ID
       {
	 CtlpYytext++;
	 CtlpYytext[strlen(CtlpYytext)-1]='\0';
	 $$ = util_strsav(CtlpYytext);
       }

comparator : TOK_ASSIGN  { $$ = util_strsav(CtlpYytext); }
             | TOK_GT { $$ = util_strsav(CtlpYytext); }
             | TOK_LT { $$ = util_strsav(CtlpYytext); } 
             | TOK_GEQ { $$ = util_strsav(CtlpYytext); } 
             | TOK_LEQ { $$ = util_strsav(CtlpYytext); }
//             | TOK_EQIV { $$ = util_strsav(CtlpYytext); }
             | TOK_NEQ { $$ = util_strsav(CtlpYytext); }
;

ax_mult: TOK_FORALL_NEXT_MULT
       { $$ = util_strsav(CtlpYytext); } ;
ex_mult: TOK_EXISTS_NEXT_MULT
       { $$ = util_strsav(CtlpYytext); } ;
%%
