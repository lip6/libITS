/* Driver template for the LEMON parser generator.
** The author disclaims copyright to this source code.
*/
/* First off, code is included that follows the "include" declaration
** in the input grammar file. */
#include <stdio.h>
#line 1 "ltsmin-grammar.lemon"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "etf/etf-util.h"
#include "etf/etf-internal.h"
#include "etf/runtime.h"
#include "etf/ltsmin-parse-env.h"
#include "etf/chunk_support.h"

// Comment this line to get debug output
#define eWarning if(0) Warning


#line 17 "ltsmin-grammar.c"
/* Next is all token values, in a form suitable for use by makeheaders.
** This section will be null unless lemon is run with the -m switch.
*/
/* 
** These constants (all generated automatically by the parser generator)
** specify the various kinds of tokens (terminals) that the parser
** understands. 
**
** Each symbol here is a terminal symbol in the grammar.
*/
/* Make sure the INTERFACE macro is defined.
*/
#ifndef INTERFACE
# define INTERFACE 1
#endif
/* The next thing included is series of defines which control
** various aspects of the generated parser.
**    YYCODETYPE         is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 terminals
**                       and nonterminals.  "int" is used otherwise.
**    YYNOCODE           is a number of type YYCODETYPE which corresponds
**                       to no legal terminal or nonterminal number.  This
**                       number is used to fill in empty slots of the hash 
**                       table.
**    YYFALLBACK         If defined, this indicates that one or more tokens
**                       have fall-back values which should be used if the
**                       original value of the token will not parse.
**    YYACTIONTYPE       is the data type used for storing terminal
**                       and nonterminal numbers.  "unsigned char" is
**                       used if there are fewer than 250 rules and
**                       states combined.  "int" is used otherwise.
**    ParseTOKENTYPE     is the data type used for minor tokens given 
**                       directly to the parser from the tokenizer.
**    YYMINORTYPE        is the data type used for all minor tokens.
**                       This is typically a union of many types, one of
**                       which is ParseTOKENTYPE.  The entry in the union
**                       for base tokens is called "yy0".
**    YYSTACKDEPTH       is the maximum depth of the parser's stack.  If
**                       zero the stack is dynamically sized using realloc()
**    ParseARG_SDECL     A static variable declaration for the %extra_argument
**    ParseARG_PDECL     A parameter declaration for the %extra_argument
**    ParseARG_STORE     Code to store %extra_argument into yypParser
**    ParseARG_FETCH     Code to extract %extra_argument from yypParser
**    YYNSTATE           the combined number of states.
**    YYNRULE            the number of rules in the grammar
**    YYERRORSYMBOL      is the code number of the error symbol.  If not
**                       defined, then do no error processing.
*/
#define YYCODETYPE unsigned char
#define YYNOCODE 55
#define YYACTIONTYPE unsigned char
#define ParseTOKENTYPE  int 
typedef union {
  int yyinit;
  ParseTOKENTYPE yy0;
  int yy4;
  etf_list_t yy14;
  etf_rel_t yy21;
  ltsmin_expr_t yy22;
  etf_map_t yy24;
  string_index_t yy102;
  int yy109;
} YYMINORTYPE;
#ifndef YYSTACKDEPTH
#define YYSTACKDEPTH 100
#endif
#define ParseARG_SDECL  ltsmin_parse_env_t env ;
#define ParseARG_PDECL , ltsmin_parse_env_t env 
#define ParseARG_FETCH  ltsmin_parse_env_t env  = yypParser->env 
#define ParseARG_STORE yypParser->env  = env 
#define YYNSTATE 99
#define YYNRULE 52
#define YYERRORSYMBOL 35
#define YYERRSYMDT yy109
#define YY_NO_ACTION      (YYNSTATE+YYNRULE+2)
#define YY_ACCEPT_ACTION  (YYNSTATE+YYNRULE+1)
#define YY_ERROR_ACTION   (YYNSTATE+YYNRULE)

/* The yyzerominor constant is used to initialize instances of
** YYMINORTYPE objects to zero. */
static const YYMINORTYPE yyzerominor = { 0 };


/* Next are the tables used to determine what action to take based on the
** current state and lookahead token.  These tables are used to implement
** functions that take a state number and lookahead value and return an
** action integer.  
**
** Suppose the action integer is N.  Then the action is determined as
** follows
**
**   0 <= N < YYNSTATE                  Shift N.  That is, push the lookahead
**                                      token onto the stack and goto state N.
**
**   YYNSTATE <= N < YYNSTATE+YYNRULE   Reduce by rule N-YYNSTATE.
**
**   N == YYNSTATE+YYNRULE              A syntax error has occurred.
**
**   N == YYNSTATE+YYNRULE+1            The parser accepts its input.
**
**   N == YYNSTATE+YYNRULE+2            No such action.  Denotes unused
**                                      slots in the yy_action[] table.
**
** The action table is constructed as a single large table named yy_action[].
** Given state S and lookahead X, the action is computed as
**
**      yy_action[ yy_shift_ofst[S] + X ]
**
** If the index value yy_shift_ofst[S]+X is out of range or if the value
** yy_lookahead[yy_shift_ofst[S]+X] is not equal to X or if yy_shift_ofst[S]
** is equal to YY_SHIFT_USE_DFLT, it means that the action is not in the table
** and that yy_default[S] should be used instead.  
**
** The formula above is for computing the action when the lookahead is
** a terminal symbol.  If the lookahead is a non-terminal (as occurs after
** a reduce action) then the yy_reduce_ofst[] array is used in place of
** the yy_shift_ofst[] array and YY_REDUCE_USE_DFLT is used in place of
** YY_SHIFT_USE_DFLT.
**
** The following are the tables generated in this section:
**
**  yy_action[]        A single table containing all actions.
**  yy_lookahead[]     A table containing the lookahead for each entry in
**                     yy_action.  Used to detect hash collisions.
**  yy_shift_ofst[]    For each state, the offset into yy_action for
**                     shifting terminals.
**  yy_reduce_ofst[]   For each state, the offset into yy_action for
**                     shifting non-terminals after a reduce.
**  yy_default[]       Default action for each state.
*/
static const YYACTIONTYPE yy_action[] = {
 /*     0 */    97,   46,   94,   17,   76,   96,   65,   66,   67,   68,
 /*    10 */     9,    8,    6,   43,   92,   82,    7,   26,   93,   95,
 /*    20 */    59,  136,   61,   12,   14,   63,   21,    9,    8,    6,
 /*    30 */   152,   18,   44,   22,   19,   13,   90,   80,    1,   20,
 /*    40 */    77,   78,    9,    8,    6,   55,    9,    8,    6,    2,
 /*    50 */    57,   48,    8,    6,   15,   41,   85,   42,   70,   71,
 /*    60 */    84,    9,    8,    6,   52,   73,   76,    5,   99,   53,
 /*    70 */    56,   25,   74,   27,   49,   53,   98,   51,   69,   28,
 /*    80 */    58,   40,   32,   37,   33,   29,   34,   30,   35,   36,
 /*    90 */    47,   31,   87,    3,    4,   38,   72,   75,   39,   45,
 /*   100 */    79,   81,   50,   83,  127,   86,   23,   88,   54,   89,
 /*   110 */    24,   91,  100,    6,   60,   62,   16,  153,   10,   64,
 /*   120 */   153,  153,   11,
};
static const YYCODETYPE yy_lookahead[] = {
 /*     0 */     8,   48,   10,   50,    5,   13,   40,   41,   42,   43,
 /*    10 */    21,   22,   23,   48,   25,   16,   24,   52,   26,   27,
 /*    20 */    28,    0,   30,   31,   32,   33,    7,   21,   22,   23,
 /*    30 */    36,   12,    8,   14,   15,   29,   35,   13,   37,   38,
 /*    40 */    16,   17,   21,   22,   23,    3,   21,   22,   23,    1,
 /*    50 */     2,    8,   22,   23,   29,    5,   13,    5,    8,    9,
 /*    60 */    17,   21,   22,   23,    5,   13,    5,   19,    0,   10,
 /*    70 */     5,    3,   10,   53,   10,   10,   53,    3,    7,   53,
 /*    80 */    53,   53,   53,   46,   53,   53,   53,   53,   53,   53,
 /*    90 */    51,   45,   39,   49,   47,   44,   12,   14,   44,   18,
 /*   100 */     8,   15,   11,   16,   16,   10,    6,    6,   11,   10,
 /*   110 */     4,    4,    0,   23,   10,   10,   34,   54,   29,   10,
 /*   120 */    54,   54,   29,
};
#define YY_SHIFT_USE_DFLT (-12)
#define YY_SHIFT_MAX 64
static const signed char yy_shift_ofst[] = {
 /*     0 */    48,   68,   42,   -1,   61,   -8,   -8,   -8,   -8,   -8,
 /*    10 */    -8,   -8,   -8,   -8,   -8,   -8,   -8,   43,   62,   64,
 /*    20 */    74,  -12,  -12,  -12,  -12,   19,   24,   21,  -11,    6,
 /*    30 */    25,   50,   40,   40,   40,   40,   40,   52,   59,   65,
 /*    40 */    30,   71,   84,   83,   81,   92,   86,   87,   88,   91,
 /*    50 */    95,  100,  101,   97,   99,  106,  107,  112,   90,  104,
 /*    60 */    89,  105,   93,  109,   82,
};
#define YY_REDUCE_USE_DFLT (-48)
#define YY_REDUCE_MAX 24
static const signed char yy_reduce_ofst[] = {
 /*     0 */    -6,  -34,    1,  -47,  -35,   20,   23,   26,   27,   28,
 /*    10 */    29,   31,   32,   33,   34,   35,   36,   39,   37,   44,
 /*    20 */    53,   46,   47,   51,   54,
};
static const YYACTIONTYPE yy_default[] = {
 /*     0 */   151,  151,  151,  124,  131,  151,  151,  151,  151,  151,
 /*    10 */   151,  151,  151,  151,  151,  151,  151,  151,  151,  151,
 /*    20 */   151,  110,  129,  113,  113,  151,  151,  151,  151,  151,
 /*    30 */   151,  151,  146,  147,  148,  149,  150,  151,  151,  151,
 /*    40 */   145,  151,  151,  151,  134,  151,  151,  151,  125,  151,
 /*    50 */   151,  151,  151,  151,  151,  151,  151,  151,  144,  151,
 /*    60 */   151,  151,  151,  151,  151,  102,  103,  104,  105,  109,
 /*    70 */   111,  112,  115,  117,  116,  118,  119,  130,  132,  133,
 /*    80 */   135,  120,  122,  123,  126,  128,  121,  101,  108,  114,
 /*    90 */   106,  107,  137,  138,  139,  140,  141,  142,  143,
};
#define YY_SZ_ACTTAB (int)(sizeof(yy_action)/sizeof(yy_action[0]))

/* The next table maps tokens into fallback tokens.  If a construct
** like the following:
** 
**      %fallback ID X Y Z.
**
** appears in the grammar, then ID becomes a fallback token for X, Y,
** and Z.  Whenever one of the tokens X, Y, or Z is input to the parser
** but it does not parse, the type of the token is changed to ID and
** the parse is retried before an error is thrown.
*/
#ifdef YYFALLBACK
static const YYCODETYPE yyFallback[] = {
};
#endif /* YYFALLBACK */

/* The following structure represents a single element of the
** parser's stack.  Information stored includes:
**
**   +  The state number for the parser at this level of the stack.
**
**   +  The value of the token stored at this level of the stack.
**      (In other words, the "major" token.)
**
**   +  The semantic value stored at this level of the stack.  This is
**      the information used by the action routines in the grammar.
**      It is sometimes called the "minor" token.
*/
struct yyStackEntry {
  YYACTIONTYPE stateno;  /* The state-number */
  YYCODETYPE major;      /* The major token value.  This is the code
                         ** number for the token at this stack level */
  YYMINORTYPE minor;     /* The user-supplied minor token value.  This
                         ** is the value of the token  */
};
typedef struct yyStackEntry yyStackEntry;

/* The state of the parser is completely contained in an instance of
** the following structure */
struct yyParser {
  int yyidx;                    /* Index of top element in stack */
#ifdef YYTRACKMAXSTACKDEPTH
  int yyidxMax;                 /* Maximum value of yyidx */
#endif
  int yyerrcnt;                 /* Shifts left before out of the error */
  ParseARG_SDECL                /* A place to hold %extra_argument */
#if YYSTACKDEPTH<=0
  int yystksz;                  /* Current side of the stack */
  yyStackEntry *yystack;        /* The parser's stack */
#else
  yyStackEntry yystack[YYSTACKDEPTH];  /* The parser's stack */
#endif
};
typedef struct yyParser yyParser;

#ifndef NDEBUG
#include <stdio.h>
static FILE *yyTraceFILE = 0;
static char *yyTracePrompt = 0;
#endif /* NDEBUG */

#ifndef NDEBUG
/* 
** Turn parser tracing on by giving a stream to which to write the trace
** and a prompt to preface each trace message.  Tracing is turned off
** by making either argument NULL 
**
** Inputs:
** <ul>
** <li> A FILE* to which trace output should be written.
**      If NULL, then tracing is turned off.
** <li> A prefix string written at the beginning of every
**      line of trace output.  If NULL, then tracing is
**      turned off.
** </ul>
**
** Outputs:
** None.
*/
void ParseTrace(FILE *TraceFILE, char *zTracePrompt){
  yyTraceFILE = TraceFILE;
  yyTracePrompt = zTracePrompt;
  if( yyTraceFILE==0 ) yyTracePrompt = 0;
  else if( yyTracePrompt==0 ) yyTraceFILE = 0;
}
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing shifts, the names of all terminals and nonterminals
** are required.  The following table supplies these names */
static const char *const yyTokenName[] = { 
  "$",             "ETF",           "ERROR",         "BEGIN",       
  "STATE",         "END",           "EDGE",          "INIT",        
  "NUMBER",        "STRING",        "IDENT",         "COLON",       
  "SORT",          "VALUE",         "TRANS",         "MAP",         
  "END_OF_LINE",   "STAR",          "SLASH",         "EXPR",        
  "QUANTIFIER",    "BIN3",          "BIN2",          "BIN1",        
  "LPAR",          "RPAR",          "STATE_VAR",     "EDGE_VAR",    
  "MU_SYM",        "DOT",           "NU_SYM",        "EXISTS_SYM",  
  "ALL_SYM",       "IF",            "THEN",          "error",       
  "input",         "etf_spec",      "state_section",  "edge_section",
  "init_section",  "trans_section",  "map_section",   "sort_section",
  "decl_list",     "valref_list",   "sort_list",     "trans_list",  
  "end",           "map_list",      "map_entry",     "value",       
  "step_list",     "expr",        
};
#endif /* NDEBUG */

#ifndef NDEBUG
/* For tracing reduce actions, the names of all rules are required.
*/
static const char *const yyRuleName[] = {
 /*   0 */ "input ::= ETF etf_spec",
 /*   1 */ "input ::= ERROR",
 /*   2 */ "etf_spec ::= state_section edge_section",
 /*   3 */ "etf_spec ::= etf_spec init_section",
 /*   4 */ "etf_spec ::= etf_spec trans_section",
 /*   5 */ "etf_spec ::= etf_spec map_section",
 /*   6 */ "etf_spec ::= etf_spec sort_section",
 /*   7 */ "etf_spec ::= error",
 /*   8 */ "state_section ::= BEGIN STATE decl_list END STATE",
 /*   9 */ "edge_section ::= BEGIN EDGE decl_list END EDGE",
 /*  10 */ "init_section ::= BEGIN INIT valref_list END INIT",
 /*  11 */ "valref_list ::=",
 /*  12 */ "valref_list ::= valref_list NUMBER",
 /*  13 */ "valref_list ::= valref_list STRING",
 /*  14 */ "decl_list ::=",
 /*  15 */ "decl_list ::= decl_list IDENT COLON IDENT",
 /*  16 */ "sort_section ::= BEGIN SORT sort_list END SORT",
 /*  17 */ "sort_list ::= IDENT",
 /*  18 */ "sort_list ::= sort_list VALUE",
 /*  19 */ "trans_section ::= BEGIN TRANS trans_list end TRANS",
 /*  20 */ "end ::= END",
 /*  21 */ "map_section ::= BEGIN MAP map_list end MAP",
 /*  22 */ "map_list ::= IDENT COLON IDENT",
 /*  23 */ "map_list ::= map_list END_OF_LINE",
 /*  24 */ "map_list ::= map_list map_entry value END_OF_LINE",
 /*  25 */ "map_entry ::=",
 /*  26 */ "map_entry ::= map_entry NUMBER",
 /*  27 */ "map_entry ::= map_entry STAR",
 /*  28 */ "value ::= NUMBER",
 /*  29 */ "value ::= VALUE",
 /*  30 */ "trans_list ::=",
 /*  31 */ "trans_list ::= trans_list step_list END_OF_LINE",
 /*  32 */ "step_list ::=",
 /*  33 */ "step_list ::= step_list STAR",
 /*  34 */ "step_list ::= step_list NUMBER SLASH NUMBER",
 /*  35 */ "step_list ::= step_list NUMBER",
 /*  36 */ "step_list ::= step_list VALUE",
 /*  37 */ "input ::= EXPR expr",
 /*  38 */ "expr ::= LPAR expr RPAR",
 /*  39 */ "expr ::= STATE_VAR",
 /*  40 */ "expr ::= IDENT",
 /*  41 */ "expr ::= EDGE_VAR",
 /*  42 */ "expr ::= VALUE",
 /*  43 */ "expr ::= NUMBER",
 /*  44 */ "expr ::= expr BIN1 expr",
 /*  45 */ "expr ::= expr BIN2 expr",
 /*  46 */ "expr ::= expr BIN3 expr",
 /*  47 */ "expr ::= MU_SYM IDENT DOT expr",
 /*  48 */ "expr ::= NU_SYM IDENT DOT expr",
 /*  49 */ "expr ::= EXISTS_SYM expr DOT expr",
 /*  50 */ "expr ::= ALL_SYM expr DOT expr",
 /*  51 */ "expr ::= IF IDENT THEN expr",
};
#endif /* NDEBUG */


#if YYSTACKDEPTH<=0
/*
** Try to increase the size of the parser stack.
*/
static void yyGrowStack(yyParser *p){
  int newSize;
  yyStackEntry *pNew;

  newSize = p->yystksz*2 + 100;
  pNew = realloc(p->yystack, newSize*sizeof(pNew[0]));
  if( pNew ){
    p->yystack = pNew;
    p->yystksz = newSize;
#ifndef NDEBUG
    if( yyTraceFILE ){
      fprintf(yyTraceFILE,"%sStack grows to %d entries!\n",
              yyTracePrompt, p->yystksz);
    }
#endif
  }
}
#endif

/* 
** This function allocates a new parser.
** The only argument is a pointer to a function which works like
** malloc.
**
** Inputs:
** A pointer to the function used to allocate memory.
**
** Outputs:
** A pointer to a parser.  This pointer is used in subsequent calls
** to Parse and ParseFree.
*/
void *ParseAlloc(void *(*mallocProc)(size_t)){
  yyParser *pParser;
  pParser = (yyParser*)(*mallocProc)( (size_t)sizeof(yyParser) );
  if( pParser ){
    pParser->yyidx = -1;
#ifdef YYTRACKMAXSTACKDEPTH
    pParser->yyidxMax = 0;
#endif
#if YYSTACKDEPTH<=0
    pParser->yystack = NULL;
    pParser->yystksz = 0;
    yyGrowStack(pParser);
#endif
  }
  return pParser;
}

/* The following function deletes the value associated with a
** symbol.  The symbol can be either a terminal or nonterminal.
** "yymajor" is the symbol code, and "yypminor" is a pointer to
** the value.
*/
static void yy_destructor(
  yyParser *yypParser,    /* The parser */
  YYCODETYPE yymajor,     /* Type code for object to destroy */
  YYMINORTYPE *yypminor   /* The object to be destroyed */
){
  ParseARG_FETCH;
  switch( yymajor ){
    /* Here is inserted the actions which take place when a
    ** terminal or non-terminal is destroyed.  This can happen
    ** when the symbol is popped from the stack during a
    ** reduce or during error processing or when a parser is 
    ** being destroyed before it is finished parsing.
    **
    ** Note: during a reduce, the only symbols destroyed are those
    ** which appear on the RHS of the rule, but which are not used
    ** inside the C code.
    */
    case 44: /* decl_list */
    case 45: /* valref_list */
    case 50: /* map_entry */
    case 52: /* step_list */
{
#line 146 "ltsmin-grammar.lemon"
 ETFlistFree((yypminor->yy14)); 
#line 459 "ltsmin-grammar.c"
}
      break;
    case 53: /* expr */
{
#line 311 "ltsmin-grammar.lemon"

    (void)env;(void)(yypminor->yy22);
    Fatal(1,error,"Expressions are not supposed to be destroyed.");

#line 469 "ltsmin-grammar.c"
}
      break;
    default:  break;   /* If no destructor action specified: do nothing */
  }
}

/*
** Pop the parser's stack once.
**
** If there is a destructor routine associated with the token which
** is popped from the stack, then call it.
**
** Return the major token number for the symbol popped.
*/
static int yy_pop_parser_stack(yyParser *pParser){
  YYCODETYPE yymajor;
  yyStackEntry *yytos = &pParser->yystack[pParser->yyidx];

  if( pParser->yyidx<0 ) return 0;
#ifndef NDEBUG
  if( yyTraceFILE && pParser->yyidx>=0 ){
    fprintf(yyTraceFILE,"%sPopping %s\n",
      yyTracePrompt,
      yyTokenName[yytos->major]);
  }
#endif
  yymajor = yytos->major;
  yy_destructor(pParser, yymajor, &yytos->minor);
  pParser->yyidx--;
  return yymajor;
}

/* 
** Deallocate and destroy a parser.  Destructors are all called for
** all stack elements before shutting the parser down.
**
** Inputs:
** <ul>
** <li>  A pointer to the parser.  This should be a pointer
**       obtained from ParseAlloc.
** <li>  A pointer to a function used to reclaim memory obtained
**       from malloc.
** </ul>
*/
void ParseFree(
  void *p,                    /* The parser to be deleted */
  void (*freeProc)(void*)     /* Function used to reclaim memory */
){
  yyParser *pParser = (yyParser*)p;
  if( pParser==0 ) return;
  while( pParser->yyidx>=0 ) yy_pop_parser_stack(pParser);
#if YYSTACKDEPTH<=0
  free(pParser->yystack);
#endif
  (*freeProc)((void*)pParser);
}

/*
** Return the peak depth of the stack for a parser.
*/
#ifdef YYTRACKMAXSTACKDEPTH
int ParseStackPeak(void *p){
  yyParser *pParser = (yyParser*)p;
  return pParser->yyidxMax;
}
#endif

/*
** Find the appropriate action for a parser given the terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_shift_action(
  yyParser *pParser,        /* The parser */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
  int stateno = pParser->yystack[pParser->yyidx].stateno;
 
  if( stateno>YY_SHIFT_MAX || (i = yy_shift_ofst[stateno])==YY_SHIFT_USE_DFLT ){
    return yy_default[stateno];
  }
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
    if( iLookAhead>0 ){
#ifdef YYFALLBACK
      YYCODETYPE iFallback;            /* Fallback token */
      if( iLookAhead<sizeof(yyFallback)/sizeof(yyFallback[0])
             && (iFallback = yyFallback[iLookAhead])!=0 ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE, "%sFALLBACK %s => %s\n",
             yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[iFallback]);
        }
#endif
        return yy_find_shift_action(pParser, iFallback);
      }
#endif
#ifdef YYWILDCARD
      {
        int j = i - iLookAhead + YYWILDCARD;
        if( j>=0 && j<YY_SZ_ACTTAB && yy_lookahead[j]==YYWILDCARD ){
#ifndef NDEBUG
          if( yyTraceFILE ){
            fprintf(yyTraceFILE, "%sWILDCARD %s => %s\n",
               yyTracePrompt, yyTokenName[iLookAhead], yyTokenName[YYWILDCARD]);
          }
#endif /* NDEBUG */
          return yy_action[j];
        }
      }
#endif /* YYWILDCARD */
    }
    return yy_default[stateno];
  }else{
    return yy_action[i];
  }
}

/*
** Find the appropriate action for a parser given the non-terminal
** look-ahead token iLookAhead.
**
** If the look-ahead token is YYNOCODE, then check to see if the action is
** independent of the look-ahead.  If it is, return the action, otherwise
** return YY_NO_ACTION.
*/
static int yy_find_reduce_action(
  int stateno,              /* Current state number */
  YYCODETYPE iLookAhead     /* The look-ahead token */
){
  int i;
#ifdef YYERRORSYMBOL
  if( stateno>YY_REDUCE_MAX ){
    return yy_default[stateno];
  }
#else
  assert( stateno<=YY_REDUCE_MAX );
#endif
  i = yy_reduce_ofst[stateno];
  assert( i!=YY_REDUCE_USE_DFLT );
  assert( iLookAhead!=YYNOCODE );
  i += iLookAhead;
#ifdef YYERRORSYMBOL
  if( i<0 || i>=YY_SZ_ACTTAB || yy_lookahead[i]!=iLookAhead ){
    return yy_default[stateno];
  }
#else
  assert( i>=0 && i<YY_SZ_ACTTAB );
  assert( yy_lookahead[i]==iLookAhead );
#endif
  return yy_action[i];
}

/*
** The following routine is called if the stack overflows.
*/
static void yyStackOverflow(yyParser *yypParser, YYMINORTYPE *yypMinor){
   ParseARG_FETCH;
   yypParser->yyidx--;
#ifndef NDEBUG
   if( yyTraceFILE ){
     fprintf(yyTraceFILE,"%sStack Overflow!\n",yyTracePrompt);
   }
#endif
   while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
   /* Here code is inserted which will execute if the parser
   ** stack every overflows */
#line 25 "ltsmin-grammar.lemon"

    (void)yypMinor;
    Fatal(1,error,"stack overflow");
#line 646 "ltsmin-grammar.c"
   ParseARG_STORE; /* Suppress warning about unused %extra_argument var */
}

/*
** Perform a shift action.
*/
static void yy_shift(
  yyParser *yypParser,          /* The parser to be shifted */
  int yyNewState,               /* The new state to shift in */
  int yyMajor,                  /* The major token to shift in */
  YYMINORTYPE *yypMinor         /* Pointer to the minor token to shift in */
){
  yyStackEntry *yytos;
  yypParser->yyidx++;
#ifdef YYTRACKMAXSTACKDEPTH
  if( yypParser->yyidx>yypParser->yyidxMax ){
    yypParser->yyidxMax = yypParser->yyidx;
  }
#endif
#if YYSTACKDEPTH>0 
  if( yypParser->yyidx>=YYSTACKDEPTH ){
    yyStackOverflow(yypParser, yypMinor);
    return;
  }
#else
  if( yypParser->yyidx>=yypParser->yystksz ){
    yyGrowStack(yypParser);
    if( yypParser->yyidx>=yypParser->yystksz ){
      yyStackOverflow(yypParser, yypMinor);
      return;
    }
  }
#endif
  yytos = &yypParser->yystack[yypParser->yyidx];
  yytos->stateno = (YYACTIONTYPE)yyNewState;
  yytos->major = (YYCODETYPE)yyMajor;
  yytos->minor = *yypMinor;
#ifndef NDEBUG
  if( yyTraceFILE && yypParser->yyidx>0 ){
    int i;
    fprintf(yyTraceFILE,"%sShift %d\n",yyTracePrompt,yyNewState);
    fprintf(yyTraceFILE,"%sStack:",yyTracePrompt);
    for(i=1; i<=yypParser->yyidx; i++)
      fprintf(yyTraceFILE," %s",yyTokenName[yypParser->yystack[i].major]);
    fprintf(yyTraceFILE,"\n");
  }
#endif
}

/* The following table contains information about every rule that
** is used during the reduce.
*/
static const struct {
  YYCODETYPE lhs;         /* Symbol on the left-hand side of the rule */
  unsigned char nrhs;     /* Number of right-hand side symbols in the rule */
} yyRuleInfo[] = {
  { 36, 2 },
  { 36, 1 },
  { 37, 2 },
  { 37, 2 },
  { 37, 2 },
  { 37, 2 },
  { 37, 2 },
  { 37, 1 },
  { 38, 5 },
  { 39, 5 },
  { 40, 5 },
  { 45, 0 },
  { 45, 2 },
  { 45, 2 },
  { 44, 0 },
  { 44, 4 },
  { 43, 5 },
  { 46, 1 },
  { 46, 2 },
  { 41, 5 },
  { 48, 1 },
  { 42, 5 },
  { 49, 3 },
  { 49, 2 },
  { 49, 4 },
  { 50, 0 },
  { 50, 2 },
  { 50, 2 },
  { 51, 1 },
  { 51, 1 },
  { 47, 0 },
  { 47, 3 },
  { 52, 0 },
  { 52, 2 },
  { 52, 4 },
  { 52, 2 },
  { 52, 2 },
  { 36, 2 },
  { 53, 3 },
  { 53, 1 },
  { 53, 1 },
  { 53, 1 },
  { 53, 1 },
  { 53, 1 },
  { 53, 3 },
  { 53, 3 },
  { 53, 3 },
  { 53, 4 },
  { 53, 4 },
  { 53, 4 },
  { 53, 4 },
  { 53, 4 },
};

static void yy_accept(yyParser*);  /* Forward Declaration */

/*
** Perform a reduce action and the shift that must immediately
** follow the reduce.
*/
static void yy_reduce(
  yyParser *yypParser,         /* The parser */
  int yyruleno                 /* Number of the rule by which to reduce */
){
  int yygoto;                     /* The next state */
  int yyact;                      /* The next action */
  YYMINORTYPE yygotominor;        /* The LHS of the rule reduced */
  yyStackEntry *yymsp;            /* The top of the parser's stack */
  int yysize;                     /* Amount to pop the stack */
  ParseARG_FETCH;
  yymsp = &yypParser->yystack[yypParser->yyidx];
#ifndef NDEBUG
  if( yyTraceFILE && yyruleno>=0 
        && yyruleno<(int)(sizeof(yyRuleName)/sizeof(yyRuleName[0])) ){
    fprintf(yyTraceFILE, "%sReduce [%s].\n", yyTracePrompt,
      yyRuleName[yyruleno]);
  }
#endif /* NDEBUG */

  /* Silence complaints from purify about yygotominor being uninitialized
  ** in some cases when it is copied into the stack after the following
  ** switch.  yygotominor is uninitialized when a rule reduces that does
  ** not set the value of its left-hand side nonterminal.  Leaving the
  ** value of the nonterminal uninitialized is utterly harmless as long
  ** as the value is never used.  So really the only thing this code
  ** accomplishes is to quieten purify.  
  **
  ** 2007-01-16:  The wireshark project (www.wireshark.org) reports that
  ** without this code, their parser segfaults.  I'm not sure what there
  ** parser is doing to make this happen.  This is the second bug report
  ** from wireshark this week.  Clearly they are stressing Lemon in ways
  ** that it has not been previously stressed...  (SQLite ticket #2172)
  */
  /*memset(&yygotominor, 0, sizeof(yygotominor));*/
  yygotominor = yyzerominor;


  switch( yyruleno ){
  /* Beginning here are the reduction cases.  A typical example
  ** follows:
  **   case 0:
  **  #line <lineno> <grammarfile>
  **     { ... }           // User supplied code
  **  #line <lineno> <thisfile>
  **     break;
  */
      case 0: /* input ::= ETF etf_spec */
#line 35 "ltsmin-grammar.lemon"
{
    eWarning(info,"parsing finished");
    etf_model_t model=env->etf;
    lts_type_set_state_label_count(model->ltstype,model->map_count);
    for(int i=0;i<model->map_count;i++){
        lts_type_set_state_label_name(model->ltstype,i,model->map_names[i]);
        lts_type_set_state_label_type(model->ltstype,i,model->map_types[i]);
    }
    eWarning(debug,"ETF model has %d transition sections",model->trans_count);
    eWarning(debug,"ETF model has %d map sections",model->map_count);
    eWarning(debug,"ETF model has %d types",lts_type_get_type_count(model->ltstype));
}
#line 823 "ltsmin-grammar.c"
        break;
      case 1: /* input ::= ERROR */
#line 47 "ltsmin-grammar.lemon"
{
  fprintf(stderr,"The error token is meant to give the lexer a way of passing the error.");
}
#line 830 "ltsmin-grammar.c"
        break;
      case 2: /* etf_spec ::= state_section edge_section */
      case 3: /* etf_spec ::= etf_spec init_section */
      case 4: /* etf_spec ::= etf_spec trans_section */
      case 5: /* etf_spec ::= etf_spec map_section */
      case 6: /* etf_spec ::= etf_spec sort_section */
#line 51 "ltsmin-grammar.lemon"
{
}
#line 840 "ltsmin-grammar.c"
        break;
      case 7: /* etf_spec ::= error */
#line 57 "ltsmin-grammar.lemon"
{
    fprintf(stderr,"ETF syntax has changed. Possible causes for syntax errors are:\n");
    fprintf(stderr," - keywords must be escaped when used as identifiers e.g. \\state\n");
    fprintf(stderr," - every variable name and type must be defined (don't use \"_\")\n");
    fprintf(stderr," - you can escape illegal identifier characters with '\\'\n");
    exit(0);
}
#line 851 "ltsmin-grammar.c"
        break;
      case 8: /* state_section ::= BEGIN STATE decl_list END STATE */
#line 65 "ltsmin-grammar.lemon"
{
    int N=ETFlistLength(yymsp[-2].minor.yy14);
    if (N==0) {
        Fatal(1,error,"state vector length must be at least 1");
    }
    eWarning(info,"The state length is %d",N);
    lts_type_set_state_length(env->etf->ltstype,N);
    etf_list_t list=yymsp[-2].minor.yy14;
    for(int i=N-1;i>=0;i--){
        if (list->fst!=SI_INDEX_FAILED) {
            char *name=SIget(env->idents,list->fst);
            lts_type_set_state_name(env->etf->ltstype,i,name);
        }
        if (list->snd!=SI_INDEX_FAILED) {
            char*sort=SIget(env->idents,list->snd);
            int typeno=ETFgetType(env->etf,sort);
            lts_type_set_state_typeno(env->etf->ltstype,i,typeno);
        }
        list=list->prev;
    }
    ETFlistFree(yymsp[-2].minor.yy14);
    eWarning(info,"done");
}
#line 878 "ltsmin-grammar.c"
        break;
      case 9: /* edge_section ::= BEGIN EDGE decl_list END EDGE */
#line 90 "ltsmin-grammar.lemon"
{
    int N=ETFlistLength(yymsp[-2].minor.yy14);
    eWarning(info,"There are %d edge labels",N);
    lts_type_set_edge_label_count(env->etf->ltstype,N);
    etf_list_t list=yymsp[-2].minor.yy14;
    for(int i=N-1;i>=0;i--){
        char *name=SIget(env->idents,list->fst);
        lts_type_set_edge_label_name(env->etf->ltstype,i,name);
        char*sort=SIget(env->idents,list->snd);
        int typeno=ETFgetType(env->etf,sort);
        lts_type_set_edge_label_typeno(env->etf->ltstype,i,typeno);
        list=list->prev;
    }
    ETFlistFree(yymsp[-2].minor.yy14);
    eWarning(info,"done");
}
#line 898 "ltsmin-grammar.c"
        break;
      case 10: /* init_section ::= BEGIN INIT valref_list END INIT */
#line 108 "ltsmin-grammar.lemon"
{
    int N=ETFlistLength(yymsp[-2].minor.yy14);
    if (N!=lts_type_get_state_length(env->etf->ltstype)){
        Fatal(1,error,"incorrect length of initial state: %d instead of %d.",
            N,lts_type_get_state_length(env->etf->ltstype));
    }
    if (env->etf->initial_state) Fatal(1,error,"more than one init section");
    env->etf->initial_state=(int*)RTmalloc(N*sizeof(int));
    etf_list_t list=yymsp[-2].minor.yy14;
    for(int i=N-1;i>=0;i--){
        switch(list->fst){
            case REFERENCE_VALUE:
                env->etf->initial_state[i]=list->snd;
                break;
            case INLINE_VALUE: {
                char *val=SIget(env->idents,list->snd);
                int typeno=lts_type_get_state_typeno(env->etf->ltstype,i);
                env->etf->initial_state[i]=SIput(env->etf->type_values[typeno],val);
                break;
            }
            default:
                Fatal(1,error,"unknown discriminator %d",list->fst);
        }
	list=list->prev;
    }
    ETFlistFree(yymsp[-2].minor.yy14);
    eWarning(info,"initial state found");
}
#line 930 "ltsmin-grammar.c"
        break;
      case 11: /* valref_list ::= */
      case 32: /* step_list ::= */
#line 140 "ltsmin-grammar.lemon"
{yygotominor.yy14=NULL;}
#line 936 "ltsmin-grammar.c"
        break;
      case 12: /* valref_list ::= valref_list NUMBER */
      case 35: /* step_list ::= step_list NUMBER */
#line 141 "ltsmin-grammar.lemon"
{ yygotominor.yy14=ETFlistAppend(yymsp[-1].minor.yy14,REFERENCE_VALUE,yymsp[0].minor.yy0); }
#line 942 "ltsmin-grammar.c"
        break;
      case 13: /* valref_list ::= valref_list STRING */
      case 36: /* step_list ::= step_list VALUE */
#line 142 "ltsmin-grammar.lemon"
{ yygotominor.yy14=ETFlistAppend(yymsp[-1].minor.yy14,INLINE_VALUE,yymsp[0].minor.yy0); }
#line 948 "ltsmin-grammar.c"
        break;
      case 14: /* decl_list ::= */
      case 25: /* map_entry ::= */
#line 147 "ltsmin-grammar.lemon"
{ yygotominor.yy14=NULL; }
#line 954 "ltsmin-grammar.c"
        break;
      case 15: /* decl_list ::= decl_list IDENT COLON IDENT */
#line 148 "ltsmin-grammar.lemon"
{ yygotominor.yy14=ETFlistAppend(yymsp[-3].minor.yy14,yymsp[-2].minor.yy0,yymsp[0].minor.yy0); }
#line 959 "ltsmin-grammar.c"
        break;
      case 16: /* sort_section ::= BEGIN SORT sort_list END SORT */
#line 152 "ltsmin-grammar.lemon"
{
    eWarning(info,"read %d values",SIgetCount(yymsp[-2].minor.yy102));
}
#line 966 "ltsmin-grammar.c"
        break;
      case 17: /* sort_list ::= IDENT */
#line 156 "ltsmin-grammar.lemon"
{
    char *name=SIget(env->idents,yymsp[0].minor.yy0);
    eWarning(info,"reading values for sort %s",name);
    int typeno=ETFgetType(env->etf,name);
    yygotominor.yy102=env->etf->type_values[typeno];
    if (SIgetCount(yygotominor.yy102)!=0) {
        Fatal(1,error,"sort %s not empty",name);
    }
}
#line 979 "ltsmin-grammar.c"
        break;
      case 18: /* sort_list ::= sort_list VALUE */
#line 165 "ltsmin-grammar.lemon"
{
    yygotominor.yy102=yymsp[-1].minor.yy102;
    chunk c;
    c.data=SIgetC(env->values,yymsp[0].minor.yy0,(int*)&c.len);
    int idx=SIgetCount(yygotominor.yy102);
    if (idx!=SIputC(yygotominor.yy102,c.data,c.len)) {
	Fatal(1,error,"non-sequential index");
    }
}
#line 992 "ltsmin-grammar.c"
        break;
      case 19: /* trans_section ::= BEGIN TRANS trans_list end TRANS */
#line 177 "ltsmin-grammar.lemon"
{
    if (ETFrelCount(yymsp[-2].minor.yy21)){
        ensure_access(env->etf->trans_manager,env->etf->trans_count);
        env->etf->trans[env->etf->trans_count]=yymsp[-2].minor.yy21;
        env->etf->trans_count++;
    } else {
        eWarning(info,"skipping empty trans section");
    }
}
#line 1005 "ltsmin-grammar.c"
        break;
      case 20: /* end ::= END */
#line 187 "ltsmin-grammar.lemon"
{ env->linebased=0; }
#line 1010 "ltsmin-grammar.c"
        break;
      case 21: /* map_section ::= BEGIN MAP map_list end MAP */
#line 191 "ltsmin-grammar.lemon"
{
    env->etf->map[env->etf->map_count]=yymsp[-2].minor.yy24;
    char*name=env->etf->map_names[env->etf->map_count];
    char*sort=env->etf->map_types[env->etf->map_count];
    //int typeno=ETFgetType(env->etf,sort);
    eWarning(info,"added map %d (%s:%s) with %d entries",
            env->etf->map_count,name,sort,ETFmapCount(yymsp[-2].minor.yy24));
    env->etf->map_count++;
    env->etf_current_idx=NULL;
    env->linebased=0;
}
#line 1025 "ltsmin-grammar.c"
        break;
      case 22: /* map_list ::= IDENT COLON IDENT */
#line 203 "ltsmin-grammar.lemon"
{
    char *name=SIget(env->idents,yymsp[-2].minor.yy0);
    char *sort=SIget(env->idents,yymsp[0].minor.yy0);
    int typeno=ETFgetType(env->etf,sort);
    ensure_access(env->etf->map_manager,env->etf->map_count);
    env->etf->map_names[env->etf->map_count]=strdup(name);
    env->etf->map_types[env->etf->map_count]=strdup(sort);
    yygotominor.yy24=ETFmapCreate(lts_type_get_state_length(env->etf->ltstype));
    env->etf_current_idx=env->etf->type_values[typeno];
    env->linebased=1;
}
#line 1040 "ltsmin-grammar.c"
        break;
      case 23: /* map_list ::= map_list END_OF_LINE */
#line 215 "ltsmin-grammar.lemon"
{ yygotominor.yy24=yymsp[-1].minor.yy24; }
#line 1045 "ltsmin-grammar.c"
        break;
      case 24: /* map_list ::= map_list map_entry value END_OF_LINE */
#line 217 "ltsmin-grammar.lemon"
{
    int N=ETFlistLength(yymsp[-2].minor.yy14);
    if(N!=lts_type_get_state_length(env->etf->ltstype)) {
        Fatal(1,error,"bad state length in map entry");
    }
    int state[N];
    etf_list_t list=yymsp[-2].minor.yy14;
    for(int i=N-1;i>=0;i--){
        state[i]=list->fst;
        list=list->prev;
    }
    ETFlistFree(yymsp[-2].minor.yy14);
    ETFmapAdd(yymsp[-3].minor.yy24,state,yymsp[-1].minor.yy4);
    yygotominor.yy24=yymsp[-3].minor.yy24;
}
#line 1064 "ltsmin-grammar.c"
        break;
      case 26: /* map_entry ::= map_entry NUMBER */
#line 237 "ltsmin-grammar.lemon"
{yygotominor.yy14=ETFlistAppend(yymsp[-1].minor.yy14,yymsp[0].minor.yy0+1,0); }
#line 1069 "ltsmin-grammar.c"
        break;
      case 27: /* map_entry ::= map_entry STAR */
#line 238 "ltsmin-grammar.lemon"
{yygotominor.yy14=ETFlistAppend(yymsp[-1].minor.yy14,0,0);}
#line 1074 "ltsmin-grammar.c"
        break;
      case 28: /* value ::= NUMBER */
#line 242 "ltsmin-grammar.lemon"
{ yygotominor.yy4=yymsp[0].minor.yy0; }
#line 1079 "ltsmin-grammar.c"
        break;
      case 29: /* value ::= VALUE */
#line 243 "ltsmin-grammar.lemon"
{
    chunk c;
    c.data=SIgetC(env->values,yymsp[0].minor.yy0,(int*)&c.len);
    yygotominor.yy4=SIputC(env->etf_current_idx,c.data,c.len);
}
#line 1088 "ltsmin-grammar.c"
        break;
      case 30: /* trans_list ::= */
#line 248 "ltsmin-grammar.lemon"
{
    yygotominor.yy21=ETFrelCreate(lts_type_get_state_length(env->etf->ltstype),
                   lts_type_get_edge_label_count(env->etf->ltstype));
    env->linebased=1;
}
#line 1097 "ltsmin-grammar.c"
        break;
      case 31: /* trans_list ::= trans_list step_list END_OF_LINE */
#line 253 "ltsmin-grammar.lemon"
{
    yygotominor.yy21=yymsp[-2].minor.yy21;
    int len=ETFlistLength(yymsp[-1].minor.yy14);
    if(len){
        int N=lts_type_get_state_length(env->etf->ltstype);
        int K=lts_type_get_edge_label_count(env->etf->ltstype);
        if(len!=N+K){
            Fatal(1,error,"bad length in trans entry: %d",len);
        }
        int src[N];
        int dst[N];
        int lbl[K];
        etf_list_t list=yymsp[-1].minor.yy14;
        for(int i=K-1;i>=0;i--){
            switch(list->fst){
                case REFERENCE_VALUE:
                    lbl[i]=list->snd;
                    break;
                case INLINE_VALUE: {
                    chunk c;
                    c.data=SIgetC(env->values,list->snd,(int*)&c.len);
                    int typeno=lts_type_get_edge_label_typeno(env->etf->ltstype,i);
                    lbl[i]=SIputC(env->etf->type_values[typeno],c.data ,c.len);
                    break;
                }
                default:
                    Fatal(1,error,"unknown discriminator %d",list->fst);
            }
            list=list->prev;
        }
        for(int i=N-1;i>=0;i--){
            src[i]=list->fst;
            dst[i]=list->snd;
            list=list->prev;
        }
        ETFlistFree(yymsp[-1].minor.yy14);
        ETFrelAdd(yygotominor.yy21,src,dst,lbl);
    }
}
#line 1140 "ltsmin-grammar.c"
        break;
      case 33: /* step_list ::= step_list STAR */
#line 297 "ltsmin-grammar.lemon"
{ yygotominor.yy14=ETFlistAppend(yymsp[-1].minor.yy14,0,0); }
#line 1145 "ltsmin-grammar.c"
        break;
      case 34: /* step_list ::= step_list NUMBER SLASH NUMBER */
#line 298 "ltsmin-grammar.lemon"
{yygotominor.yy14=ETFlistAppend(yymsp[-3].minor.yy14,yymsp[-2].minor.yy0+1,yymsp[0].minor.yy0+1);}
#line 1150 "ltsmin-grammar.c"
        break;
      case 37: /* input ::= EXPR expr */
#line 316 "ltsmin-grammar.lemon"
{
    (void)yymsp[0].minor.yy22;
//    log_printf(info,"Expression is: ");
//    LTSminPrintExpr(info,env,yymsp[0].minor.yy22);
//    log_printf(info,"\n");
    env->expr=yymsp[0].minor.yy22;
}
#line 1161 "ltsmin-grammar.c"
        break;
      case 38: /* expr ::= LPAR expr RPAR */
#line 329 "ltsmin-grammar.lemon"
{ yygotominor.yy22=yymsp[-1].minor.yy22; }
#line 1166 "ltsmin-grammar.c"
        break;
      case 39: /* expr ::= STATE_VAR */
#line 330 "ltsmin-grammar.lemon"
{
    yygotominor.yy22=RT_NEW(struct ltsmin_expr_s );
    yygotominor.yy22->node_type=SVAR;
    yygotominor.yy22->idx=yymsp[0].minor.yy0;
}
#line 1175 "ltsmin-grammar.c"
        break;
      case 40: /* expr ::= IDENT */
#line 335 "ltsmin-grammar.lemon"
{
    yygotominor.yy22=RT_NEW(struct ltsmin_expr_s );
    yygotominor.yy22->node_type=VAR;
    yygotominor.yy22->idx=yymsp[0].minor.yy0;
}
#line 1184 "ltsmin-grammar.c"
        break;
      case 41: /* expr ::= EDGE_VAR */
#line 340 "ltsmin-grammar.lemon"
{
    yygotominor.yy22=RT_NEW(struct ltsmin_expr_s );
    yygotominor.yy22->node_type=EVAR;
    yygotominor.yy22->idx=yymsp[0].minor.yy0;
}
#line 1193 "ltsmin-grammar.c"
        break;
      case 42: /* expr ::= VALUE */
#line 345 "ltsmin-grammar.lemon"
{
    yygotominor.yy22=RT_NEW(struct ltsmin_expr_s );
    yygotominor.yy22->node_type=CHUNK;
    yygotominor.yy22->idx=yymsp[0].minor.yy0;
}
#line 1202 "ltsmin-grammar.c"
        break;
      case 43: /* expr ::= NUMBER */
#line 350 "ltsmin-grammar.lemon"
{
    yygotominor.yy22=RT_NEW(struct ltsmin_expr_s );
    yygotominor.yy22->node_type=INT;
    yygotominor.yy22->idx=yymsp[0].minor.yy0;
}
#line 1211 "ltsmin-grammar.c"
        break;
      case 44: /* expr ::= expr BIN1 expr */
      case 45: /* expr ::= expr BIN2 expr */
      case 46: /* expr ::= expr BIN3 expr */
#line 356 "ltsmin-grammar.lemon"
{
    yygotominor.yy22=RT_NEW(struct ltsmin_expr_s );
    yygotominor.yy22->node_type=BINARY_OP;
    yygotominor.yy22->idx=yymsp[-1].minor.yy0;
    yygotominor.yy22->arg1=yymsp[-2].minor.yy22;
    yygotominor.yy22->arg2=yymsp[0].minor.yy22;
}
#line 1224 "ltsmin-grammar.c"
        break;
      case 47: /* expr ::= MU_SYM IDENT DOT expr */
#line 378 "ltsmin-grammar.lemon"
{
    yygotominor.yy22=RT_NEW(struct ltsmin_expr_s );
    yygotominor.yy22->node_type=MU_FIX;
    yygotominor.yy22->idx=yymsp[-2].minor.yy0;
    yygotominor.yy22->arg1=yymsp[0].minor.yy22;
}
#line 1234 "ltsmin-grammar.c"
        break;
      case 48: /* expr ::= NU_SYM IDENT DOT expr */
#line 385 "ltsmin-grammar.lemon"
{
    yygotominor.yy22=RT_NEW(struct ltsmin_expr_s );
    yygotominor.yy22->node_type=NU_FIX;
    yygotominor.yy22->idx=yymsp[-2].minor.yy0;
    yygotominor.yy22->arg1=yymsp[0].minor.yy22;
}
#line 1244 "ltsmin-grammar.c"
        break;
      case 49: /* expr ::= EXISTS_SYM expr DOT expr */
#line 392 "ltsmin-grammar.lemon"
{
    yygotominor.yy22=RT_NEW(struct ltsmin_expr_s );
    yygotominor.yy22->node_type=EXISTS_STEP;
    yygotominor.yy22->idx=0;
    yygotominor.yy22->arg1=yymsp[-2].minor.yy22;
    yygotominor.yy22->arg2=yymsp[0].minor.yy22;  
}
#line 1255 "ltsmin-grammar.c"
        break;
      case 50: /* expr ::= ALL_SYM expr DOT expr */
#line 400 "ltsmin-grammar.lemon"
{
    yygotominor.yy22=RT_NEW(struct ltsmin_expr_s );
    yygotominor.yy22->node_type=FORALL_STEP;
    yygotominor.yy22->idx=0;
    yygotominor.yy22->arg1=yymsp[-2].minor.yy22;
    yygotominor.yy22->arg2=yymsp[0].minor.yy22;  
}
#line 1266 "ltsmin-grammar.c"
        break;
      case 51: /* expr ::= IF IDENT THEN expr */
#line 408 "ltsmin-grammar.lemon"
{
    yygotominor.yy22 = yymsp[0].minor.yy22 ;
}
#line 1273 "ltsmin-grammar.c"
        break;
  };
  yygoto = yyRuleInfo[yyruleno].lhs;
  yysize = yyRuleInfo[yyruleno].nrhs;
  yypParser->yyidx -= yysize;
  yyact = yy_find_reduce_action(yymsp[-yysize].stateno,(YYCODETYPE)yygoto);
  if( yyact < YYNSTATE ){
#ifdef NDEBUG
    /* If we are not debugging and the reduce action popped at least
    ** one element off the stack, then we can push the new element back
    ** onto the stack here, and skip the stack overflow test in yy_shift().
    ** That gives a significant speed improvement. */
    if( yysize ){
      yypParser->yyidx++;
      yymsp -= yysize-1;
      yymsp->stateno = (YYACTIONTYPE)yyact;
      yymsp->major = (YYCODETYPE)yygoto;
      yymsp->minor = yygotominor;
    }else
#endif
    {
      yy_shift(yypParser,yyact,yygoto,&yygotominor);
    }
  }else{
    assert( yyact == YYNSTATE + YYNRULE + 1 );
    yy_accept(yypParser);
  }
}

/*
** The following code executes when the parse fails
*/
static void yy_parse_failed(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sFail!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser fails */
#line 22 "ltsmin-grammar.lemon"
  exit(0); 
#line 1320 "ltsmin-grammar.c"
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following code executes when a syntax error first occurs.
*/
static void yy_syntax_error(
  yyParser *yypParser,           /* The parser */
  int yymajor,                   /* The major type of the error token */
  YYMINORTYPE yyminor            /* The minor type of the error token */
){
  ParseARG_FETCH;
#define TOKEN (yyminor.yy0)
#line 18 "ltsmin-grammar.lemon"

    (void)yymajor;(void)yyminor;
    fprintf(stderr,"syntax error near line %d, pos %d",env->lineno+1,env->linepos+1);
#line 1338 "ltsmin-grammar.c"
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/*
** The following is executed when the parser accepts
*/
static void yy_accept(
  yyParser *yypParser           /* The parser */
){
  ParseARG_FETCH;
#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sAccept!\n",yyTracePrompt);
  }
#endif
  while( yypParser->yyidx>=0 ) yy_pop_parser_stack(yypParser);
  /* Here code is inserted which will be executed whenever the
  ** parser accepts */
#line 23 "ltsmin-grammar.lemon"
 eWarning(info,"success!"); 
#line 1359 "ltsmin-grammar.c"
  ParseARG_STORE; /* Suppress warning about unused %extra_argument variable */
}

/* The main parser program.
** The first argument is a pointer to a structure obtained from
** "ParseAlloc" which describes the current state of the parser.
** The second argument is the major token number.  The third is
** the minor token.  The fourth optional argument is whatever the
** user wants (and specified in the grammar) and is available for
** use by the action routines.
**
** Inputs:
** <ul>
** <li> A pointer to the parser (an opaque structure.)
** <li> The major token number.
** <li> The minor token number.
** <li> An option argument of a grammar-specified type.
** </ul>
**
** Outputs:
** None.
*/
void Parse(
  void *yyp,                   /* The parser */
  int yymajor,                 /* The major token code number */
  ParseTOKENTYPE yyminor       /* The value for the token */
  ParseARG_PDECL               /* Optional %extra_argument parameter */
){
  YYMINORTYPE yyminorunion;
  int yyact;            /* The parser action. */
  int yyendofinput;     /* True if we are at the end of input */
#ifdef YYERRORSYMBOL
  int yyerrorhit = 0;   /* True if yymajor has invoked an error */
#endif
  yyParser *yypParser;  /* The parser */

  /* (re)initialize the parser, if necessary */
  yypParser = (yyParser*)yyp;
  if( yypParser->yyidx<0 ){
#if YYSTACKDEPTH<=0
    if( yypParser->yystksz <=0 ){
      /*memset(&yyminorunion, 0, sizeof(yyminorunion));*/
      yyminorunion = yyzerominor;
      yyStackOverflow(yypParser, &yyminorunion);
      return;
    }
#endif
    yypParser->yyidx = 0;
    yypParser->yyerrcnt = -1;
    yypParser->yystack[0].stateno = 0;
    yypParser->yystack[0].major = 0;
  }
  yyminorunion.yy0 = yyminor;
  yyendofinput = (yymajor==0);
  ParseARG_STORE;

#ifndef NDEBUG
  if( yyTraceFILE ){
    fprintf(yyTraceFILE,"%sInput %s\n",yyTracePrompt,yyTokenName[yymajor]);
  }
#endif

  do{
    yyact = yy_find_shift_action(yypParser,(YYCODETYPE)yymajor);
    if( yyact<YYNSTATE ){
      assert( !yyendofinput );  /* Impossible to shift the $ token */
      yy_shift(yypParser,yyact,yymajor,&yyminorunion);
      yypParser->yyerrcnt--;
      yymajor = YYNOCODE;
    }else if( yyact < YYNSTATE + YYNRULE ){
      yy_reduce(yypParser,yyact-YYNSTATE);
    }else{
      assert( yyact == YY_ERROR_ACTION );
#ifdef YYERRORSYMBOL
      int yymx;
#endif
#ifndef NDEBUG
      if( yyTraceFILE ){
        fprintf(yyTraceFILE,"%sSyntax Error!\n",yyTracePrompt);
      }
#endif
#ifdef YYERRORSYMBOL
      /* A syntax error has occurred.
      ** The response to an error depends upon whether or not the
      ** grammar defines an error token "ERROR".  
      **
      ** This is what we do if the grammar does define ERROR:
      **
      **  * Call the %syntax_error function.
      **
      **  * Begin popping the stack until we enter a state where
      **    it is legal to shift the error symbol, then shift
      **    the error symbol.
      **
      **  * Set the error count to three.
      **
      **  * Begin accepting and shifting new tokens.  No new error
      **    processing will occur until three tokens have been
      **    shifted successfully.
      **
      */
      if( yypParser->yyerrcnt<0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yymx = yypParser->yystack[yypParser->yyidx].major;
      if( yymx==YYERRORSYMBOL || yyerrorhit ){
#ifndef NDEBUG
        if( yyTraceFILE ){
          fprintf(yyTraceFILE,"%sDiscard input token %s\n",
             yyTracePrompt,yyTokenName[yymajor]);
        }
#endif
        yy_destructor(yypParser, (YYCODETYPE)yymajor,&yyminorunion);
        yymajor = YYNOCODE;
      }else{
         while(
          yypParser->yyidx >= 0 &&
          yymx != YYERRORSYMBOL &&
          (yyact = yy_find_reduce_action(
                        yypParser->yystack[yypParser->yyidx].stateno,
                        YYERRORSYMBOL)) >= YYNSTATE
        ){
          yy_pop_parser_stack(yypParser);
        }
        if( yypParser->yyidx < 0 || yymajor==0 ){
          yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
          yy_parse_failed(yypParser);
          yymajor = YYNOCODE;
        }else if( yymx!=YYERRORSYMBOL ){
          YYMINORTYPE u2;
          u2.YYERRSYMDT = 0;
          yy_shift(yypParser,yyact,YYERRORSYMBOL,&u2);
        }
      }
      yypParser->yyerrcnt = 3;
      yyerrorhit = 1;
#else  /* YYERRORSYMBOL is not defined */
      /* This is what we do if the grammar does not define ERROR:
      **
      **  * Report an error message, and throw away the input token.
      **
      **  * If the input token is $, then fail the parse.
      **
      ** As before, subsequent error messages are suppressed until
      ** three input tokens have been successfully shifted.
      */
      if( yypParser->yyerrcnt<=0 ){
        yy_syntax_error(yypParser,yymajor,yyminorunion);
      }
      yypParser->yyerrcnt = 3;
      yy_destructor(yypParser,(YYCODETYPE)yymajor,&yyminorunion);
      if( yyendofinput ){
        yy_parse_failed(yypParser);
      }
      yymajor = YYNOCODE;
#endif
    }
  }while( yymajor!=YYNOCODE && yypParser->yyidx>=0 );
  return;
}
