/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_DVE_YY_DVE_GRAMMAR_TAB_HH_INCLUDED
# define YY_DVE_YY_DVE_GRAMMAR_TAB_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int dve_yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    T_NO_TOKEN = 258,
    T_FOREIGN_ID = 259,
    T_FOREIGN_SQUARE_BRACKETS = 260,
    T_EXCLAM = 261,
    T_CONST = 262,
    T_TRUE = 263,
    T_FALSE = 264,
    T_SYSTEM = 265,
    T_PROCESS = 266,
    T_STATE = 267,
    T_CHANNEL = 268,
    T_COMMIT = 269,
    T_INIT = 270,
    T_ACCEPT = 271,
    T_ASSERT = 272,
    T_TRANS = 273,
    T_GUARD = 274,
    T_SYNC = 275,
    T_ASYNC = 276,
    T_PROPERTY = 277,
    T_EFFECT = 278,
    T_BUCHI = 279,
    T_GENBUCHI = 280,
    T_STREETT = 281,
    T_RABIN = 282,
    T_MULLER = 283,
    T_ID = 284,
    T_INT = 285,
    T_BYTE = 286,
    T_NAT = 287,
    T_WIDE_ARROW = 288,
    T_ASSIGNMENT = 289,
    T_IMPLY = 290,
    T_BOOL_OR = 291,
    T_BOOL_AND = 292,
    T_OR = 293,
    T_AND = 294,
    T_XOR = 295,
    T_EQ = 296,
    T_NEQ = 297,
    T_LT = 298,
    T_LEQ = 299,
    T_GEQ = 300,
    T_GT = 301,
    T_LSHIFT = 302,
    T_RSHIFT = 303,
    T_MINUS = 304,
    T_PLUS = 305,
    T_MULT = 306,
    T_DIV = 307,
    T_MOD = 308,
    T_TILDE = 309,
    T_BOOL_NOT = 310,
    T_UNARY_MINUS = 311,
    T_INCREMENT = 312,
    T_DECREMENT = 313,
    UOPERATOR = 314,
    T_PARENTHESIS = 315,
    T_SQUARE_BRACKETS = 316,
    T_DOT = 317,
    T_ARROW = 318
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 103 "dve_grammar.yy" /* yacc.c:1909  */

  bool flag;
  int number;
  char string[MAXLEN];

#line 124 "dve_grammar.tab.hh" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE dve_yylval;
extern YYLTYPE dve_yylloc;
int dve_yyparse (void);

#endif /* !YY_DVE_YY_DVE_GRAMMAR_TAB_HH_INCLUDED  */
