%{
/**CFile***********************************************************************

  FileName    [ctlp.l]

  PackageName [ctlp]

  Synopsis    [Lexical analyzer for CTL formula parser. See ctlp.h for syntax.]

  Author      [Gary York, Ramin Hojati, Tom Shiple, Yuji Kukimoto,
               Jae-Young Jang]

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

  Revision    [$Id: ctlp.l,v 1.7 2002/08/26 14:43:24 fabio Exp $]

******************************************************************************/


/*---------------------------------------------------------------------------*/
/* Constant declarations                                                     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Variable declarations                                                     */
/*---------------------------------------------------------------------------*/


/**AutomaticStart*************************************************************/

/*---------------------------------------------------------------------------*/
/* Static function prototypes                                                */
/*---------------------------------------------------------------------------*/


/**AutomaticEnd***************************************************************/

#include "visfd.h"
#include "ctlp.h"
#include "ctlpInt.h"
#include "ctlp_yacc.h"

/*---------------------------------------------------------------------------*/
/* Definition of internal functions                                          */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [Initializes global variables for parsing file.]

  SideEffects []

******************************************************************************/
void
CtlpFileSetup(
  FILE * fp)
{
  CtlpYyin             = fp;
  CtlpYylineno         = 1;
}

/*---------------------------------------------------------------------------*/
/* Definition of static functions                                            */
/*---------------------------------------------------------------------------*/

/**Function********************************************************************

  Synopsis    [Prints error message and sets global error flag.]

  SideEffects []

******************************************************************************/
void
CtlpYyerror(
	const  char * errmsg)
{
  (void) fprintf(vis_stderr, "%s in line %d with token \"%s\"\n", errmsg, CtlpYylineno, yytext); 
  CtlpGlobalError = 1;
}

%}

/*---------------------------------------------------------------------------*/
/*      Lexical analyzer rules                                               */
/*---------------------------------------------------------------------------*/

%option noyywrap
%option yylineno
%option nounput

/* substitution strings */
/* alnum is the same set in alnumsymbol in io.l except [ and ] being
removed here for the until operator */
/* - , =, and ; are intentionally removed
   - is used for -> and <->
   = is used for foo = bar 
   ; is used for delimitters */
/* ( and ) are intentionally removed since they are used as parentheses */
/* +, *, and ^ have to be used surrounded by spaces
since they are symbols. ! can be attached immediately w/o spaces though
since it is not a symbol */
alnum		[A-Za-z0-9\^\?\|\/\+\*\$\<\>~@\_#\$%\:\'\.]
/*alnum		[A-Za-z0-9\^\?\|\/\[\]\+\*\$\<\>~@\_#\$%\:\"\'\.]*/
%%

[ \t\n\r]	;

"#".*   ;   


\"[^\"]*\" { return (QUOTED_TOK_ID) ; }


A	{ return(TOK_FORALL); }
AX	{ return(TOK_FORALL_NEXT); }
AF	{ return(TOK_FORALL_EVENTUALLY); }
AG	{ return(TOK_FORALL_GLOBALLY); }

E	{ return(TOK_EXISTS); }
EX	{ return(TOK_EXISTS_NEXT); }
EF	{ return(TOK_EXISTS_EVENTUALLY); }
EG	{ return(TOK_EXISTS_GLOBALLY); }

AX:[0-9]+  { return(TOK_FORALL_NEXT_MULT); }
EX:[0-9]+  { return(TOK_EXISTS_NEXT_MULT); }

U	{ return(TOK_UNTIL); }

TRUE    { return(TOK_TRUE); }

FALSE   { return(TOK_FALSE); }

\\DEFINE |
\\Define |
\\define  { return(TOK_DEFINE); }

\+	{ return(TOK_OR); }
\|\|	{ return(TOK_OR); }
\*	{ return(TOK_AND); }
\&\&	{ return(TOK_AND); }
"!="    { return(TOK_NEQ); }
!	{ return(TOK_NOT); }
"^"	{ return(TOK_XOR); }
"->"	{ return(TOK_THEN); }
"<->"	{ return(TOK_EQ); }
==      { return(TOK_EQIV); }
=       { return(TOK_ASSIGN); }
"<"     { return(TOK_LT); }
">"     { return(TOK_GT); }
">="    { return(TOK_LEQ); }
"<="    { return(TOK_GEQ); }
\,      { return(TOK_COMMA); }
\\      { return(TOK_MACRO); }

{alnum}+                                   { return(TOK_ID); }
{alnum}+\[[ ]*[0-9]+[ ]*\]                 { return(TOK_ID2); }
{alnum}+\[[ ]*[0-9]+[ ]*\:[ ]*[0-9]+[ ]*\] { return(TOK_ID_VECTOR); }

.       { return CtlpYytext[0]; }
%%
