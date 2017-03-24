/****************************************************************************/
/*								            */
/* This file is part of the PNDDD, Petri Net Data Decision Diagram  package.*/
/*     						                            */
/*     Copyright (C) 2004 Denis Poitrenaud and Yann Thierry-Mieg            */
/*     						                            */
/*     This program is free software; you can redistribute it and/or modify */
/*     it under the terms of the GNU General Public License as published by */
/*     the Free Software Foundation; either version 2 of the License, or    */
/*     (at your option) any later version.                                  */
/*     This program is distributed in the hope that it will be useful,      */
/*     but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/*     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/*     GNU General Public License for more details.                         */
/*     						                            */
/*     You should have received a copy of the GNU General Public License    */
/*     along with this program; if not, write to the Free Software          */
/*Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */
/*     						                            */
/****************************************************************************/
 


/****************************************************************************************
*  created by   : Mr Denis POITRENAUD and Cherif Reda HASSAINE
*  created date : 04/2003
*  subject      : lexical of Expression DDD
****************************************************************************************/


%{
/************************* INCLUDES AND EXTERNALS *************************************/
#include <string>
#include "Moteur_expression.hpp"
#include "expression_yacc.h"

%}
%option noyywrap
%option yylineno
%option prefix="expression"
%%
"!"					                            {return('!');}
"!="                                    {return(DIFF);}
";"					                            {return(';');}
"."					                            {return('.');}
"=="                                    {return(EGAL);}
">"                                     {return('>');}
">="                                    {return(ESUP);}
"<"                                     {return('<');}
"<="                                    {return(EINF);}
"+"                                     {return('+');}
"*"                                     {return('*');}
"-"                                     {return('-');}
"&&"                                    {return(ET);}
"||"				                            {return(OU);}
"("                                     {return('(');}
")"                                     {return(')');}
"@"					                            {return('@');}
">>"					                          {return(TETE);}
"<<"					                          {return(QUE);}
"/"				                              {return('/');}
"%"				                              {return('%');}
"//"					                          {return(COMMENTAIRE);}
[\n]*                                   {return(ESPACE);}
"--".*\n                                {return(ESPACE);}
"empty"				                          {return(EMPTY);}
".size"				                          {return(SIZ);}                                   
[0-9]+                                  {sscanf(yytext,"%d", &expressionlval.j);return(ENTIER);}        
[a-zA-Z_][a-zA-Z0-9_]*[\0]*             {expressionlval.s=strdup(yytext); return(IDENT);}
[ \t]			{};

%%









