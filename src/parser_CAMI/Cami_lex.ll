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
"3:net"                                           {return(NET);}
"11:declaration"                                   {return(DECLARATION);}
"7:version"                                       {return(CAMIVERSION);}
"5:title"                                         {return(TITLE);}
"7:project"                                       {return(PROJECT);}
"4:date"                                          {return(DATE);}
"4:code"                                          {return(CODE);}
"4:name"                                          {return(NAME);}
"7:marking"                                       {return(MARKING);}
"6:domain"                                        {return(DOMAINE);}
"9:component"                                     {return(COMPONENT);}
"5:guard"                                         {return(GUARD);}
"8:priority"                                      {return(PRIORITY);}
"6:action"                                        {return(ACTION);}
"5:delay"                                         {return(DELAY);}
"6:weight"                                        {return(WEIGHT);}
"5:place"                                         {return(PLACE);}
"10:transition"                                    {return(TRANSITION);}
"5:queue"                                         {return(QUEUE);}
"20:immediate transition"                          {return(IMMEDIATE);}
"8:capacity"                                      {return(CAPACITY);}
"9:valuation"                                     {return(VALUATION);}
"3:arc"                                           {return(ARC);}
"13:inhibitor arc"                                 {return(INHIBITOR);}
"9:inhibitor"                                       {return(INHIBITOR);}
"16:latestFiringTime"                               {return (LFT);}
"18:earliestFiringTime"                             {return (EFT);}
"10:visibility"                                      {return (VISIBILITY);}
"5:label"                                           {return (LABEL); }
"9:author(s)"					                            {return(AUTHORS);}
"//"						                                {return(COMMENTAIRE);}
"DB()" 		                        	            {return(DB);}
"FB()"				        	                        {return(FB);}
"CA"				 		                                {return(CA);}
"CT"						                                {return(CT);}
"CN"						                                {return(CN);}
"CM"						                                {return(CM);}
"PO"						                                {return(PO);}
"PT"						                                {return(PT);}
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



