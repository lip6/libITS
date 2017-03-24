/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         dve_ppparse
#define yylex           dve_pplex
#define yyerror         dve_pperror
#define yydebug         dve_ppdebug
#define yynerrs         dve_ppnerrs

#define yylval dve_yylval
#define yychar          dve_ppchar
#define yylloc dve_yylloc

/* Copy the first part of user declarations.  */


  #ifdef yylex
   #undef yylex
  #endif
  #include <iostream>
  #include "divine/legacy/system/dve/syntax_analysis/dve_flex_lexer.hh"
  #include "divine/legacy/system/dve/syntax_analysis/dve_grammar.hh"
  #include "divine/legacy/common/error.hh"
//  #define YY_DECL int lexer_flex(void)
//  #define yylex lexer_flex
  #define YY_DECL static yyFlexLexer mylexer;

  #define YYINITDEPTH 10000

  #define YYMAXDEPTH 30000
  #ifdef yylex
   #undef yylex
  #endif
  #define yylex mylexer.yylex
  using namespace divine;
  using namespace std;

  YY_DECL;
  
  static dve_parser_t * parser;
  static error_vector_t * pterr;
  
  #define CALL(p1,p2,method) parser->set_fpos(p1.first_line,p1.first_column); parser->set_lpos(p2.last_line,p2.last_column); parser->method;
  #define PERROR(mes) (*pterr) << last_loc.first_line << ":" << last_loc.first_column << "-" << last_loc.last_line << ":" << last_loc.last_column << "  " << mes.message << thr(mes.type,mes.id)
  
  const ERR_type_t ERRTYPE = 351;
  
  ERR_c_triplet_t PE_VARDECL("Invalid variable declaration",ERRTYPE,34000);
  ERR_c_triplet_t PE_LOCALDECL("Error in local declarations",ERRTYPE,34001);
  ERR_c_triplet_t PE_EXPR("Error in local declarations",ERRTYPE,34002);
  ERR_c_triplet_t PE_VARINIT("Error in a variable initialization",ERRTYPE,34003);
  ERR_c_triplet_t PE_EFFECT_LIST("Error in declaration of list of effects",
                                 ERRTYPE,34004);
  ERR_c_triplet_t PE_PROCESS_CONTEXT("Error during parsing of expression"
                                     " inside a process context",
                                 ERRTYPE, 34005);
  ERR_c_triplet_t PE_ASSERT("Error in an assertion definition",ERRTYPE,34003);
  
  static YYLTYPE last_loc=YYLTYPE();



# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "dve_proc_grammar.tab.hh".  */
#ifndef YY_DVE_PP_DVE_PROC_GRAMMAR_TAB_HH_INCLUDED
# define YY_DVE_PP_DVE_PROC_GRAMMAR_TAB_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int dve_ppdebug;
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


  bool flag;
  int number;
  char string[MAXLEN];


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


extern YYSTYPE dve_pplval;
extern YYLTYPE dve_pplloc;
int dve_ppparse (void);

#endif /* !YY_DVE_PP_DVE_PROC_GRAMMAR_TAB_HH_INCLUDED  */

/* Copy the second part of user declarations.  */



#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   522

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  75
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  70
/* YYNRULES -- Number of rules.  */
#define YYNRULES  153
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  268

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   318

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      60,    61,     2,     2,    70,     2,    66,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    73,    69,
       2,     2,     2,    74,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    63,     2,    64,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    71,     2,    72,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    62,    65,    67,    68
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   120,   131,   137,   137,   139,   139,   143,   144,   148,
     149,   153,   154,   159,   160,   164,   168,   169,   169,   170,
     174,   175,   179,   180,   184,   188,   189,   204,   209,   209,
     217,   221,   223,   224,   271,   275,   276,   280,   287,   291,
     292,   293,   297,   298,   299,   300,   304,   305,   306,   307,
     308,   309,   313,   314,   318,   319,   323,   324,   328,   329,
     333,   334,   338,   342,   343,   347,   348,   352,   353,   357,
     361,   362,   369,   371,   375,   376,   380,   380,   381,   387,
     389,   393,   394,   403,   405,   409,   413,   417,   418,   422,
     424,   428,   429,   429,   431,   431,   435,   436,   440,   440,
     442,   442,   443,   443,   445,   445,   449,   450,   451,   455,
     456,   460,   464,   465,   465,   465,   467,   474,   475,   490,
     492,   494,   496,   498,   500,   502,   504,   506,   508,   510,
     512,   514,   516,   518,   520,   522,   524,   526,   528,   530,
     532,   534,   536,   538,   540,   544,   546,   548,   555,   557,
     562,   567,   568,   569
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "T_NO_TOKEN", "T_FOREIGN_ID",
  "T_FOREIGN_SQUARE_BRACKETS", "T_EXCLAM", "T_CONST", "T_TRUE", "T_FALSE",
  "T_SYSTEM", "T_PROCESS", "T_STATE", "T_CHANNEL", "T_COMMIT", "T_INIT",
  "T_ACCEPT", "T_ASSERT", "T_TRANS", "T_GUARD", "T_SYNC", "T_ASYNC",
  "T_PROPERTY", "T_EFFECT", "T_BUCHI", "T_GENBUCHI", "T_STREETT",
  "T_RABIN", "T_MULLER", "T_ID", "T_INT", "T_BYTE", "T_NAT",
  "T_WIDE_ARROW", "T_ASSIGNMENT", "T_IMPLY", "T_BOOL_OR", "T_BOOL_AND",
  "T_OR", "T_AND", "T_XOR", "T_EQ", "T_NEQ", "T_LT", "T_LEQ", "T_GEQ",
  "T_GT", "T_LSHIFT", "T_RSHIFT", "T_MINUS", "T_PLUS", "T_MULT", "T_DIV",
  "T_MOD", "T_TILDE", "T_BOOL_NOT", "T_UNARY_MINUS", "T_INCREMENT",
  "T_DECREMENT", "UOPERATOR", "'('", "')'", "T_PARENTHESIS", "'['", "']'",
  "T_SQUARE_BRACKETS", "'.'", "T_DOT", "T_ARROW", "';'", "','", "'{'",
  "'}'", "':'", "'?'", "$accept", "Id", "VariableDecl", "$@1", "$@2",
  "Const", "TypeName", "TypeId", "DeclIdList", "DeclId", "VarInit", "$@3",
  "Initializer", "FieldInitList", "FieldInit", "ArrayDecl",
  "ProcDeclStandalone", "ProcDecl", "$@4", "ProcBody", "ProcLocalDeclList",
  "States", "StateDeclList", "StateDecl", "Init", "InitAndCommitAndAccept",
  "CommitAndAccept", "Accept", "AcceptListBuchi",
  "AcceptListGenBuchiMuller", "AcceptListGenBuchiMullerSet",
  "AcceptListGenBuchiMullerSetNotEmpty", "AcceptListRabinStreett",
  "AcceptListRabinStreettPair", "AcceptListRabinStreettFirst",
  "AcceptListRabinStreettSecond", "AcceptListRabinStreettSet", "Commit",
  "CommitList", "Assertions", "AssertionList", "Assertion", "$@5",
  "Transitions", "TransitionList", "Transition", "ProbTransition",
  "ProbTransitionPart", "ProbList", "TransitionOpt", "Guard", "$@6", "$@7",
  "Sync", "SyncExpr", "$@8", "$@9", "$@10", "$@11", "SyncValue",
  "ExpressionList", "OneExpression", "Effect", "$@12", "$@13", "EffList",
  "Expression", "Assignment", "AssignOp", "UnaryOp", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
      40,    41,   315,    91,    93,   316,    46,   317,   318,    59,
      44,   123,   125,    58,    63
};
# endif

#define YYPACT_NINF -73

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-73)))

#define YYTABLE_NINF -114

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      13,   -17,    37,   -73,   -73,   -73,   -73,    -3,   -73,   -12,
      44,   -73,   -73,    24,   -17,   -73,    41,     9,    43,   -73,
     -73,    38,   -73,   -73,   -73,   -73,   -73,   -17,   -17,   -17,
       6,     1,    59,    90,    97,   108,   -73,   -17,    60,    79,
      62,   -73,    66,   -73,    70,   -17,    89,    93,    93,    89,
      76,   -73,   -73,   -17,   120,   -73,   -73,   -73,   -73,   -73,
     130,   132,   -73,   -17,   -73,   -17,   -73,   -73,   -22,   -73,
      78,   -21,   -73,    88,   -11,   -73,   -73,   -73,   -17,    91,
     122,   -73,   -17,   -73,   104,   170,   -73,   -73,   -73,   -73,
     103,   116,   -73,    89,   -73,   110,   113,   -73,    93,   -73,
     -73,   123,   -73,   189,   -73,   -17,   -27,   125,   -73,   -73,
     -73,   -73,   177,   -17,   -73,   -73,   -17,   -73,   -73,   -73,
     -73,   254,   -73,   118,   -17,   -73,   -25,   -73,   -73,   -73,
     -73,   -73,   -73,   254,   254,   -52,   -73,   424,   254,   -73,
     -73,   424,   -17,   134,   -17,   -73,   -73,   377,    -7,   -73,
     424,   254,   -17,   -17,   254,   254,   254,   254,   254,   254,
     254,   254,   254,   254,   254,   254,   254,   254,   254,   254,
     254,   254,   254,   -73,   137,   -73,     8,   184,   136,   -73,
     254,   -73,   317,   -73,   133,   442,   458,   458,   295,   295,
     295,   469,   469,    69,    69,    69,    69,   -30,   -30,    75,
      75,   -73,   -73,   -73,   180,   -17,   -73,   244,   193,   184,
     -73,   -73,   254,   -73,   -73,   -73,   254,   -17,   191,   193,
     347,   146,   282,    -5,   148,   246,   150,   191,   -73,   -73,
     -73,   101,   105,   -73,   -73,   254,   -73,   151,   -73,   179,
     -73,   179,   131,   405,   -73,   -73,   158,   254,   -73,   424,
     160,   -73,   -73,   254,   -73,   256,   -73,    31,   -73,   424,
     -73,   -73,   -73,   -73,   424,   254,   -73,   -73
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,    27,     2,    28,     1,     0,    31,     0,
       0,    29,    33,     0,     0,    32,     0,     0,     0,    10,
      37,     0,    35,    11,    12,     9,     5,     0,     0,     0,
       0,    39,    72,     0,    44,    45,    34,     0,     0,    25,
       0,    13,     0,    69,     0,     0,     0,     0,     0,     0,
       0,    46,    40,     0,    79,    41,    43,    42,    36,     6,
       0,    16,     4,     0,    70,     0,    38,    47,     0,    48,
       0,     0,    51,     0,     0,    50,    49,    52,     0,     0,
       0,    74,     0,    30,     0,     0,    15,    14,    71,    57,
      58,     0,    54,     0,    63,    67,     0,    60,     0,    65,
      62,     0,    53,     0,    73,     0,     0,     0,    81,    84,
      26,    19,     0,     0,    56,    55,     0,    64,    61,    66,
      78,     0,    75,     0,     0,    80,     0,   120,   119,   121,
     151,   152,   153,     0,     0,   122,    18,    20,     0,    59,
      68,    77,     0,     0,     0,    90,    82,     0,     0,    22,
      24,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   125,     0,    87,     0,    91,     0,   124,
       0,    21,     0,   144,   145,   147,   143,   142,   138,   137,
     139,   128,   129,   126,   127,   131,   130,   140,   141,   133,
     132,   134,   135,   136,     0,     0,    85,     0,    96,    91,
      23,   123,     0,    86,    88,    94,     0,     0,   112,    96,
       0,     0,     0,     0,     0,     0,     0,   112,   146,    95,
      93,     0,     0,    97,   116,     0,    83,     0,   100,   106,
     104,   106,     0,     0,   117,    89,     0,     0,    99,   107,
       0,   103,   114,     0,   150,     0,   101,     0,   109,   111,
     105,   115,   118,   149,   148,     0,   108,   110
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -73,    -1,   -73,   -73,   -73,   -73,   -73,   -73,   -73,   157,
     -73,   -73,   -73,   -73,    55,   -73,   -73,   -73,   -73,   -73,
     -73,   -73,   -73,   199,   207,   -73,   211,   216,   -36,   -47,
     -73,   143,   -45,   -73,   -73,   -73,   -69,   212,   194,   -73,
     -73,   153,   -73,   -73,   -73,   135,   -73,    61,   -73,   -73,
      51,   -73,   -73,    48,   -73,   -73,   -73,   -73,   -73,    27,
     -73,     4,    45,   -73,   -73,   -73,   -72,    17,   -73,   -73
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,   135,    15,    27,    38,    16,    17,    25,    40,    41,
      86,   112,   136,   148,   149,    61,     2,     3,     7,     9,
      10,    18,    21,    22,    31,    32,    33,    34,    51,    69,
      70,    91,    72,    73,    74,   100,    96,    35,    43,    54,
      80,    81,   121,    83,   107,   108,   109,   175,   176,   146,
     208,   216,   221,   218,   224,   239,   246,   241,   250,   248,
     257,   258,   226,   235,   261,   242,   150,   244,   255,   138
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
       5,   231,    76,    75,     4,   101,   123,     4,     4,    67,
      26,   151,     4,    20,   152,    28,   153,    30,     4,   168,
     169,   170,   171,   172,     1,    19,    39,    42,    44,    50,
      45,    46,    47,    48,    49,     4,    20,     6,    -3,    89,
     137,   124,   102,   144,    50,    12,   115,   140,    94,   141,
      99,    13,    79,   118,    -8,    -8,    14,    28,    29,    30,
      11,   147,    39,   180,    42,   181,   173,    90,     8,   232,
      95,    23,    24,    95,    -7,    -7,    53,    50,   205,   182,
     206,   106,   185,   186,   187,   188,   189,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   265,   238,   266,    79,    29,   240,    36,    37,   -98,
     -98,    28,    90,  -102,  -102,    95,   166,   167,   168,   169,
     170,   171,   172,   143,    30,   106,   170,   171,   172,    59,
     -98,    62,    63,   -98,  -102,    64,    65,  -102,    82,    66,
     220,   174,    60,   178,   222,    77,    78,    92,    93,    68,
     -98,   183,   184,    71,  -102,   -98,   -98,    97,    98,  -102,
    -102,   -98,    84,   243,   103,  -102,    85,   249,   110,   249,
     -98,   111,   -98,   113,  -102,   259,  -102,   114,   -17,   -17,
     116,   243,   117,   264,   119,   127,   128,   127,   128,   142,
     120,   104,   105,   259,   125,   126,   212,   -76,   -76,   -17,
     252,   253,   -17,   207,   174,   177,     4,   209,     4,   129,
     204,   129,   213,   217,   225,   229,   223,   233,   -76,   -17,
      87,   -76,   236,   245,   -17,   -17,   130,   256,   130,   260,
     -17,   131,   132,   131,   132,   210,    58,   133,   -76,   133,
      55,   -17,    52,   -76,   -76,   215,    56,   234,   134,   -76,
     247,    57,   -92,   -92,  -113,  -113,   139,   263,   122,    88,
     219,   145,   127,   128,   127,   128,   214,   227,   251,   267,
     262,     0,   237,   -92,     0,  -113,   -92,     0,  -113,     0,
       0,     0,     0,     4,     0,     4,   129,     0,   129,     0,
       0,     0,     0,   -92,     0,  -113,     0,     0,   -92,   -92,
    -113,  -113,     0,   130,   -92,   130,  -113,     0,   131,   132,
     131,   132,     0,     0,   133,     0,   133,   154,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,     0,
       0,   230,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   211,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   228,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,     0,     0,     0,     0,     0,     0,     0,   179,   254,
     154,   155,   156,   157,   158,   159,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172
};

static const yytype_int16 yycheck[] =
{
       1,     6,    49,    48,    29,    74,    33,    29,    29,    45,
       1,    63,    29,    14,    66,    14,    68,    16,    29,    49,
      50,    51,    52,    53,    11,     1,    27,    28,    29,    30,
      24,    25,    26,    27,    28,    29,    37,     0,    29,    61,
     112,    68,    78,    68,    45,     1,    93,   116,    69,   121,
      61,     7,    53,    98,    30,    31,    12,    14,    15,    16,
      72,   133,    63,    70,    65,    72,   138,    68,    71,    74,
      71,    30,    31,    74,    30,    31,    17,    78,    70,   151,
      72,    82,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,    70,     1,    72,   105,    15,     1,    69,    70,     8,
       9,    14,   113,     8,     9,   116,    47,    48,    49,    50,
      51,    52,    53,   124,    16,   126,    51,    52,    53,    69,
      29,    69,    70,    32,    29,    69,    70,    32,    18,    69,
     212,   142,    63,   144,   216,    69,    70,    69,    70,    60,
      49,   152,   153,    60,    49,    54,    55,    69,    70,    54,
      55,    60,    32,   235,    73,    60,    34,   239,    64,   241,
      69,     1,    71,    70,    69,   247,    71,    61,     8,     9,
      70,   253,    69,   255,    61,     8,     9,     8,     9,    71,
       1,    69,    70,   265,    69,    70,    63,     8,     9,    29,
      69,    70,    32,    19,   205,    71,    29,    71,    29,    32,
      73,    32,    32,    20,    23,    69,   217,    69,    29,    49,
      63,    32,    72,    72,    54,    55,    49,    69,    49,    69,
      60,    54,    55,    54,    55,   180,    37,    60,    49,    60,
      33,    71,    31,    54,    55,     1,    34,     1,    71,    60,
      71,    35,     8,     9,     8,     9,   113,     1,   105,    65,
     209,   126,     8,     9,     8,     9,   205,   219,   241,   265,
     253,    -1,   227,    29,    -1,    29,    32,    -1,    32,    -1,
      -1,    -1,    -1,    29,    -1,    29,    32,    -1,    32,    -1,
      -1,    -1,    -1,    49,    -1,    49,    -1,    -1,    54,    55,
      54,    55,    -1,    49,    60,    49,    60,    -1,    54,    55,
      54,    55,    -1,    -1,    60,    -1,    60,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    -1,
      -1,    69,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    64,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    64,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    11,    91,    92,    29,    76,     0,    93,    71,    94,
      95,    72,     1,     7,    12,    77,    80,    81,    96,     1,
      76,    97,    98,    30,    31,    82,     1,    78,    14,    15,
      16,    99,   100,   101,   102,   112,    69,    70,    79,    76,
      83,    84,    76,   113,    76,    24,    25,    26,    27,    28,
      76,   103,   101,    17,   114,    99,   112,   102,    98,    69,
      63,    90,    69,    70,    69,    70,    69,   103,    60,   104,
     105,    60,   107,   108,   109,   107,   104,    69,    70,    76,
     115,   116,    18,   118,    32,    34,    85,    84,   113,    61,
      76,   106,    69,    70,    69,    76,   111,    69,    70,    61,
     110,   111,   103,    73,    69,    70,    76,   119,   120,   121,
      64,     1,    86,    70,    61,   104,    70,    69,   107,    61,
       1,   117,   116,    33,    68,    69,    70,     8,     9,    32,
      49,    54,    55,    60,    71,    76,    87,   141,   144,   106,
     111,   141,    71,    76,    68,   120,   124,   141,    88,    89,
     141,    63,    66,    68,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,   141,    76,   122,   123,    71,    76,    61,
      70,    72,   141,    76,    76,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,    73,    70,    72,    19,   125,    71,
      89,    64,    63,    32,   122,     1,   126,    20,   128,   125,
     141,   127,   141,    76,   129,    23,   137,   128,    64,    69,
      69,     6,    74,    69,     1,   138,    72,   137,     1,   130,
       1,   132,   140,   141,   142,    72,   131,    71,   134,   141,
     133,   134,    69,    70,    34,   143,    69,   135,   136,   141,
      69,   139,   142,     1,   141,    70,    72,   136
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    75,    76,    78,    77,    79,    77,    80,    80,    81,
      81,    82,    82,    83,    83,    84,    85,    86,    85,    85,
      87,    87,    88,    88,    89,    90,    90,    91,    93,    92,
      94,    95,    95,    95,    96,    97,    97,    98,    99,   100,
     100,   100,   101,   101,   101,   101,   102,   102,   102,   102,
     102,   102,   103,   103,   104,   104,   105,   105,   106,   106,
     107,   107,   108,   109,   109,   110,   110,   111,   111,   112,
     113,   113,   114,   114,   115,   115,   117,   116,   116,   118,
     118,   119,   119,   120,   120,   121,   122,   123,   123,   124,
     124,   125,   126,   125,   127,   125,   128,   128,   130,   129,
     131,   129,   132,   129,   133,   129,   134,   134,   134,   135,
     135,   136,   137,   138,   139,   137,   137,   140,   140,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   142,   142,
     143,   144,   144,   144
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     4,     0,     4,     0,     1,     2,
       2,     1,     1,     1,     3,     3,     0,     0,     3,     2,
       1,     3,     1,     3,     1,     0,     3,     1,     0,     6,
       5,     0,     2,     2,     3,     1,     3,     1,     3,     1,
       2,     2,     2,     2,     1,     1,     2,     3,     3,     3,
       3,     3,     2,     3,     2,     3,     3,     2,     1,     3,
       2,     3,     2,     2,     3,     1,     2,     1,     3,     2,
       2,     3,     0,     3,     1,     3,     0,     4,     3,     0,
       3,     1,     3,     8,     1,     5,     3,     1,     3,     7,
       1,     0,     0,     4,     0,     4,     0,     3,     0,     4,
       0,     5,     0,     4,     0,     5,     0,     1,     3,     1,
       3,     1,     0,     0,     0,     5,     2,     1,     3,     1,
       1,     1,     1,     4,     3,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     6,     3,     3,     3,
       1,     1,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static unsigned
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  unsigned res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
 }

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (yylocationp);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
extern YYSTYPE dve_yylval;
/* Location data for the lookahead symbol.  */
extern YYLTYPE dve_yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  yylsp[0] = yylloc;
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yyls1, yysize * sizeof (*yylsp),
                    &yystacksize);

        yyls = yyls1;
        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

    { strncpy((yyval.string), (yyvsp[0].string), MAXLEN); }

    break;

  case 3:

    { CALL((yylsp[0]),(yylsp[0]),var_decl_begin((yyvsp[0].number))); }

    break;

  case 4:

    { CALL((yylsp[-3]),(yylsp[0]),var_decl_done()); }

    break;

  case 5:

    { PERROR(PE_VARDECL); CALL((yylsp[-1]),(yylsp[-1]),var_decl_cancel()); }

    break;

  case 7:

    { (yyval.number) = false; }

    break;

  case 8:

    { (yyval.number) = true; }

    break;

  case 9:

    { CALL((yylsp[-1]),(yylsp[-1]),type_is_const((yyvsp[-1].number))); (yyval.number) = (yyvsp[0].number); }

    break;

  case 10:

    { PERROR(PE_VARDECL); }

    break;

  case 11:

    { (yyval.number) = T_INT; }

    break;

  case 12:

    { (yyval.number) = T_BYTE; }

    break;

  case 15:

    { CALL((yylsp[-2]), (yylsp[0]), var_decl_create((yyvsp[-2].string), (yyvsp[-1].number), (yyvsp[0].flag))); }

    break;

  case 16:

    { (yyval.flag) = false; }

    break;

  case 17:

    { CALL((yylsp[0]),(yylsp[0]),take_expression()); }

    break;

  case 18:

    { (yyval.flag) = true; }

    break;

  case 19:

    { CALL((yylsp[-1]),(yylsp[-1]),take_expression_cancel()); PERROR(PE_VARINIT); }

    break;

  case 20:

    { CALL((yylsp[0]),(yylsp[0]),var_init_is_field(false)); }

    break;

  case 21:

    { CALL((yylsp[-2]),(yylsp[0]),var_init_is_field(true)); }

    break;

  case 22:

    {}

    break;

  case 23:

    {}

    break;

  case 24:

    { CALL((yylsp[0]),(yylsp[0]),var_init_field_part()); }

    break;

  case 25:

    { (yyval.number) = 0; }

    break;

  case 26:

    { (yyval.number) = 1; CALL((yylsp[-2]),(yylsp[0]),var_decl_array_size((yyvsp[-1].number))); }

    break;

  case 27:

    { parser->done(); }

    break;

  case 28:

    { CALL((yylsp[-1]),(yylsp[0]),proc_decl_begin((yyvsp[0].string))); }

    break;

  case 29:

    { CALL((yylsp[-5]),(yylsp[0]),proc_decl_done()); }

    break;

  case 32:

    {}

    break;

  case 33:

    { PERROR(PE_LOCALDECL); }

    break;

  case 34:

    { CALL((yylsp[-1]),(yylsp[-1]),state_list_done()); }

    break;

  case 37:

    { CALL((yylsp[0]),(yylsp[0]),state_decl((yyvsp[0].string))); }

    break;

  case 38:

    { CALL((yylsp[-1]),(yylsp[-1]),state_init((yyvsp[-1].string))); }

    break;

  case 46:

    { CALL((yylsp[-1]),(yylsp[0]),accept_type(T_BUCHI)); }

    break;

  case 47:

    { CALL((yylsp[-2]),(yylsp[-1]),accept_type(T_BUCHI)); }

    break;

  case 48:

    { CALL((yylsp[-2]),(yylsp[-1]),accept_type(T_GENBUCHI)); }

    break;

  case 49:

    { CALL((yylsp[-2]),(yylsp[-1]),accept_type(T_MULLER)); }

    break;

  case 50:

    { CALL((yylsp[-2]),(yylsp[-1]),accept_type(T_RABIN)); }

    break;

  case 51:

    { CALL((yylsp[-2]),(yylsp[-1]),accept_type(T_STREETT)); }

    break;

  case 52:

    { CALL((yylsp[-1]),(yylsp[-1]),state_accept((yyvsp[-1].string))); }

    break;

  case 53:

    { CALL((yylsp[-2]),(yylsp[-2]),state_accept((yyvsp[-2].string))); }

    break;

  case 54:

    { CALL((yylsp[-1]),(yylsp[-1]),accept_genbuchi_muller_set_complete()); }

    break;

  case 55:

    { CALL((yylsp[-2]),(yylsp[-2]),accept_genbuchi_muller_set_complete()); }

    break;

  case 58:

    { CALL((yylsp[0]),(yylsp[0]),state_genbuchi_muller_accept((yyvsp[0].string))); }

    break;

  case 59:

    { CALL((yylsp[-2]),(yylsp[-2]),state_genbuchi_muller_accept((yyvsp[-2].string))); }

    break;

  case 60:

    { CALL((yylsp[-1]),(yylsp[-1]),accept_rabin_streett_pair_complete()); }

    break;

  case 61:

    { CALL((yylsp[-2]),(yylsp[-2]),accept_rabin_streett_pair_complete()); }

    break;

  case 63:

    { CALL((yylsp[-1]),(yylsp[0]),accept_rabin_streett_first_complete()); }

    break;

  case 64:

    { CALL((yylsp[-2]),(yylsp[-1]),accept_rabin_streett_first_complete()); }

    break;

  case 67:

    { CALL((yylsp[0]),(yylsp[0]),state_rabin_streett_accept((yyvsp[0].string))); }

    break;

  case 68:

    { CALL((yylsp[-2]),(yylsp[-2]),state_rabin_streett_accept((yyvsp[-2].string))); }

    break;

  case 70:

    { CALL((yylsp[-1]),(yylsp[-1]),state_commit((yyvsp[-1].string))); }

    break;

  case 71:

    { CALL((yylsp[-2]),(yylsp[-2]),state_commit((yyvsp[-2].string))); }

    break;

  case 76:

    { CALL((yylsp[-1]),(yylsp[-1]),take_expression()); }

    break;

  case 77:

    { CALL((yylsp[-3]),(yylsp[-1]),assertion_create((yyvsp[-3].string))); }

    break;

  case 78:

    { CALL((yylsp[-2]),(yylsp[-2]),take_expression_cancel()); PERROR(PE_ASSERT); }

    break;

  case 83:

    { CALL((yylsp[-7]),(yylsp[0]),trans_create((yyvsp[-7].string),(yyvsp[-5].string),(yyvsp[-3].flag),(yyvsp[-2].number),(yyvsp[-1].number))); }

    break;

  case 85:

    { CALL((yylsp[-4]),(yylsp[0]),prob_trans_create((yyvsp[-4].string))); }

    break;

  case 86:

    { CALL((yylsp[-2]),(yylsp[0]),prob_transition_part((yyvsp[-2].string),(yyvsp[0].number))); }

    break;

  case 89:

    { CALL((yylsp[-6]),(yylsp[0]),trans_create(0,(yyvsp[-5].string),(yyvsp[-3].flag),(yyvsp[-2].number),(yyvsp[-1].number))); }

    break;

  case 91:

    { (yyval.flag) = false; }

    break;

  case 92:

    { CALL((yylsp[0]),(yylsp[0]),take_expression()); }

    break;

  case 93:

    { CALL((yylsp[-2]),(yylsp[-2]),trans_guard_expr()); (yyval.flag) = true; }

    break;

  case 94:

    { CALL((yylsp[-1]),(yylsp[-1]),take_expression_cancel()) }

    break;

  case 95:

    { (yyval.flag) = false; }

    break;

  case 96:

    { (yyval.number) = 0; }

    break;

  case 97:

    { (yyval.number) = (yyvsp[-1].number); }

    break;

  case 98:

    { CALL((yylsp[-1]),(yylsp[0]),take_expression()); }

    break;

  case 99:

    { CALL((yylsp[-3]),(yylsp[0]),trans_sync((yyvsp[-3].string),1,(yyvsp[0].flag))); (yyval.number) = 1; }

    break;

  case 100:

    { CALL((yylsp[-2]),(yylsp[-1]),take_expression_cancel()); }

    break;

  case 101:

    { (yyval.number) = 1; }

    break;

  case 102:

    { CALL((yylsp[-1]),(yylsp[0]),take_expression()); }

    break;

  case 103:

    { CALL((yylsp[-3]),(yylsp[0]),trans_sync((yyvsp[-3].string),2,(yyvsp[0].flag))); (yyval.number) = 2; }

    break;

  case 104:

    { CALL((yylsp[-2]),(yylsp[-1]),take_expression_cancel()); }

    break;

  case 105:

    { (yyval.number) = 1; }

    break;

  case 106:

    { (yyval.flag) = false; }

    break;

  case 107:

    { (yyval.flag) = true; }

    break;

  case 108:

    { (yyval.flag) = true; }

    break;

  case 111:

    { CALL((yylsp[0]),(yylsp[0]), expression_list_store()); }

    break;

  case 112:

    { (yyval.number) = 0; }

    break;

  case 113:

    { CALL((yylsp[0]),(yylsp[0]),trans_effect_list_begin()); }

    break;

  case 114:

    { CALL((yylsp[-3]),(yylsp[-3]),trans_effect_list_end()); }

    break;

  case 115:

    { (yyval.number) = (yyvsp[-2].number); }

    break;

  case 116:

    { CALL((yylsp[-1]),(yylsp[-1]),trans_effect_list_cancel());
	    PERROR(PE_EFFECT_LIST);
	  }

    break;

  case 117:

    { (yyval.number) = 1; CALL((yylsp[0]),(yylsp[0]),trans_effect_part()); }

    break;

  case 118:

    { (yyval.number) = (yyvsp[-2].number) + 1; CALL((yylsp[0]),(yylsp[0]),trans_effect_part()); }

    break;

  case 119:

    { CALL((yylsp[0]),(yylsp[0]),expr_false()); }

    break;

  case 120:

    { CALL((yylsp[0]),(yylsp[0]),expr_true()); }

    break;

  case 121:

    { CALL((yylsp[0]),(yylsp[0]),expr_nat((yyvsp[0].number))); }

    break;

  case 122:

    { CALL((yylsp[0]),(yylsp[0]),expr_id((yyvsp[0].string))); }

    break;

  case 123:

    { CALL((yylsp[-3]),(yylsp[0]),expr_array_mem((yyvsp[-3].string))); }

    break;

  case 124:

    { CALL((yylsp[-2]),(yylsp[0]),expr_parenthesis()); }

    break;

  case 125:

    { CALL((yylsp[-1]),(yylsp[0]),expr_unary((yyvsp[-1].number))); }

    break;

  case 126:

    { CALL((yylsp[-2]),(yylsp[0]),expr_bin(T_LT)); }

    break;

  case 127:

    { CALL((yylsp[-2]),(yylsp[0]),expr_bin(T_LEQ)); }

    break;

  case 128:

    { CALL((yylsp[-2]),(yylsp[0]),expr_bin(T_EQ)); }

    break;

  case 129:

    { CALL((yylsp[-2]),(yylsp[0]),expr_bin(T_NEQ)); }

    break;

  case 130:

    { CALL((yylsp[-2]),(yylsp[0]),expr_bin(T_GT)); }

    break;

  case 131:

    { CALL((yylsp[-2]),(yylsp[0]),expr_bin(T_GEQ)); }

    break;

  case 132:

    { CALL((yylsp[-2]),(yylsp[0]),expr_bin(T_PLUS)); }

    break;

  case 133:

    { CALL((yylsp[-2]),(yylsp[0]),expr_bin(T_MINUS)); }

    break;

  case 134:

    { CALL((yylsp[-2]),(yylsp[0]),expr_bin(T_MULT)); }

    break;

  case 135:

    { CALL((yylsp[-2]),(yylsp[0]),expr_bin(T_DIV)); }

    break;

  case 136:

    { CALL((yylsp[-2]),(yylsp[0]),expr_bin(T_MOD)); }

    break;

  case 137:

    { CALL((yylsp[-2]),(yylsp[0]),expr_bin(T_AND)); }

    break;

  case 138:

    { CALL((yylsp[-2]),(yylsp[0]),expr_bin(T_OR)); }

    break;

  case 139:

    { CALL((yylsp[-2]),(yylsp[0]),expr_bin(T_XOR)); }

    break;

  case 140:

    { CALL((yylsp[-2]),(yylsp[0]),expr_bin(T_LSHIFT)); }

    break;

  case 141:

    { CALL((yylsp[-2]),(yylsp[0]),expr_bin(T_RSHIFT)); }

    break;

  case 142:

    { CALL((yylsp[-2]),(yylsp[0]),expr_bin(T_BOOL_AND)); }

    break;

  case 143:

    { CALL((yylsp[-2]),(yylsp[0]),expr_bin(T_BOOL_OR)); }

    break;

  case 144:

    { CALL((yylsp[-2]),(yylsp[0]),expr_state_of_process((yyvsp[-2].string),(yyvsp[0].string))); }

    break;

  case 145:

    { CALL((yylsp[-2]),(yylsp[0]),expr_var_of_process((yyvsp[-2].string),(yyvsp[0].string))); }

    break;

  case 146:

    { CALL((yylsp[-5]),(yylsp[-3]),expr_var_of_process((yyvsp[-5].string),(yyvsp[-3].string),true)); }

    break;

  case 147:

    { CALL((yylsp[-2]),(yylsp[0]),expr_bin(T_IMPLY)); }

    break;

  case 148:

    { CALL((yylsp[-2]),(yylsp[0]),expr_assign((yyvsp[-1].number))); }

    break;

  case 149:

    { PERROR(PE_EXPR); }

    break;

  case 150:

    { (yyval.number) = T_ASSIGNMENT; }

    break;

  case 151:

    { (yyval.number) = T_UNARY_MINUS; }

    break;

  case 152:

    { (yyval.number) = T_TILDE; }

    break;

  case 153:

    { (yyval.number) = T_BOOL_NOT; }

    break;



      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }

  yyerror_range[1] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[1] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}




/* BEGIN of definitions of functions provided by parser */

void yyerror(const char * msg)
 { (*pterr) << yylloc.first_line << ":" << yylloc.first_column << "-" <<
		yylloc.last_line << ":" << yylloc.last_column << "  " <<
		msg << psh();
  last_loc = yylloc;
  (*pterr) << "Parsing interrupted." << thr();
 }

void divine::dve_proc_init_parsing(dve_parser_t * const pt, error_vector_t * const estack,
                       istream & mystream)
 { parser = pt; pterr = estack; mylexer.yyrestart(&mystream);
   yylloc.first_line = 1; 
   yylloc.first_column = 1; 
   yylloc.last_line = 1; 
   yylloc.last_column = 1;
 }

/* END of definitions of functions provided by parser */

