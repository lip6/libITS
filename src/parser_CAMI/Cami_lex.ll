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
*  created date : 19/05/2003
*  subject      : lexical of CAMI syntaxe
*  version      : 0.1
****************************************************************************************/

%{
/*********************** INCLUDES AND EXTERNALS ****************************************/
#include <string>
#include "RdPE.h"
#include "Cami.hpp"
#include "Cami_yacc.h"

%}
%option noyywrap
%option yylineno
%option prefix="Cami"
%%
"net"                                           {return(NET);}
"declaration"                                   {return(DECLARATION);}
"version"                                       {return(CAMIVERSION);}
"title"                                         {return(TITLE);}
"project"                                       {return(PROJECT);}
"date"                                          {return(DATE);}
"code"                                          {return(CODE);}
"name"                                          {return(NAME);}
"marking"                                       {return(MARKING);}
"domain"                                        {return(DOMAINE);}
"component"                                     {return(COMPONENT);}
"guard"                                         {return(GUARD);}
"priority"                                      {return(PRIORITY);}
"action"                                        {return(ACTION);}
"delay"                                         {return(DELAY);}
"weight"                                        {return(WEIGHT);}
"place"                                         {return(PLACE);}
"transition"                                    {return(TRANSITION);}
"queue"                                         {return(QUEUE);}
"immediate transition"                          {return(IMMEDIATE);}
"capacity"                                      {return(CAPACITY);}
"valuation"                                     {return(VALUATION);}
"arc"                                           {return(ARC);}
"inhibitor arc"                                 {return(INHIBITOR);}
"author(s)"					                            {return(AUTHORS);}
"//"						                                {return(COMMENTAIRE);}
"DB()" 		                        	            {return(DB);}
"FB()"				        	                        {return(FB);}
"DE()" 		                        	            {return(DB);}
"FE()"				        	                        {return(FB);}
"CA"				 		                                {return(CA);}
"CT"						                                {return(CT);}
"CN"						                                {return(CN);}
"CM"						                                {return(CM);}
"PO"						                                {return(PO);}
"PT"						                                {return(PT);}
"PI"						                                {return(PI); /* position info */ }
"cp"|"CP"|"Cp"|"cP"                             {return(CP);}
"loss"|"LOSS"|"Loss"                            {return(LOSS);}
"."					  	                                {return('.');}
","					  	                                {return(',');}
";"					  	                                {return(';');}
":"					  	                                {return(':');}
"("                                   		      {return('(');}
")"                                   		      {return(')');}
"#"                                   		      {return('#');}
[\0]*[\n]*			         	                      {return(ESPACE);}
"--".*\n                     			              {return(ESPACE);}
[0-9]+                             		          {sscanf(yytext,"%d", &Camilval.j);return(ENTIER);}
[" "*a-zA-Z_][" "*a-zA-Z" "*0-9_]*		          {Camilval.s=strdup(yytext); return(IDENT);}
"-".*";"                                        {Camilval.s=strdup(yytext); return(DENT);}
[ \t]			{};
%%



