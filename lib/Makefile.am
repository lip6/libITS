SUBDIRS = bringBack
noinst_LIBRARIES = libITSmain.a libprod.a
lib_LIBRARIES = libITS.a

MYSRCDIR= $(top_srcdir)/its
MYPRODSRCDIR= $(MYSRCDIR)/prod
MYJSONSRCDIR= $(MYSRCDIR)/parser_json
MYPETRISRCDIR = $(MYSRCDIR)/petri
MYCOMPSRCDIR = $(MYSRCDIR)/composite
MYETFSRCDIR = $(MYSRCDIR)/etf
MYSCALARSRCDIR = $(MYSRCDIR)/scalar
MYGALSRCDIR = $(MYSRCDIR)/gal
MYWIBBLESRCDIR = $(MYSRCDIR)/divine/wibble
MYDVESRCDIR = $(MYSRCDIR)/divine
MYGALGRAMMARSRCDIR = $(MYGALSRCDIR)/parser
MYAIGERDIR = $(MYSRCDIR)/aiger

EXTRA_DIST= bringBack/Makefile 


MYANTLRHEADER = $(MYGALGRAMMARSRCDIR)/exprParserLexer.h \
		$(MYGALGRAMMARSRCDIR)/exprParserParser.h \
		$(MYGALGRAMMARSRCDIR)/GALParser.hh

MYANTLRSOURCES = $(MYGALGRAMMARSRCDIR)/exprParserLexer.cpp \
		$(MYGALGRAMMARSRCDIR)/exprParserParser.cpp \
		$(MYGALGRAMMARSRCDIR)/GALParser.cpp



libprod_a_YFLAGS = -d 
libprod_a_LFLAGS = -olex.yy.c

#BUILT_SOURCES =  ./ITSProdLoader_yacc.h

libprod_a_SOURCES = $(MYPRODSRCDIR)/ProdLoader.hh \
		$(MYPRODSRCDIR)/ITSProdLoader_yacc.yxx \
		$(MYPRODSRCDIR)/ITSProdLoader_lex.ll 



JSONSOURCES = 	$(MYJSONSRCDIR)/hierarchie.cpp	 \
		$(MYJSONSRCDIR)/hierarchie.hh   \
		$(MYJSONSRCDIR)/json_model.cpp   \
		$(MYJSONSRCDIR)/parse_json.cpp   \
		$(MYJSONSRCDIR)/parse_json.hh \
		$(MYJSONSRCDIR)/cJSON.h \
		$(MYJSONSRCDIR)/cJSON.c \
		$(MYPETRISRCDIR)/JSON2ITS.cpp \
		$(MYPETRISRCDIR)/JSON2ITS.hh \
		$(MYPETRISRCDIR)/JSONPNetType.hh \
		$(MYPETRISRCDIR)/JSONPNetType.cpp 

# used to be protected by if HAVE_BOOST
JSONSTUFF = $(JSONSOURCES)



XMLLOADERSOURCES =  $(MYCOMPSRCDIR)/CompositeXMLLoader.hh $(MYCOMPSRCDIR)/CompositeXMLLoader.cpp \
	$(MYSRCDIR)/ITSModelXMLLoader.hh $(MYSRCDIR)/ITSModelXMLLoader.cpp \
	$(MYPETRISRCDIR)/XMLLoader.hh $(MYPETRISRCDIR)/XMLLoader.cpp


PETRIHEADERS = 	$(MYPETRISRCDIR)/Arc.hh \
	$(MYPETRISRCDIR)/Hom_PlaceArcs.hh \
	$(MYPETRISRCDIR)/Marking.hh \
	$(MYPETRISRCDIR)/Modular2ITS.hh \
	$(MYPETRISRCDIR)/Place.hh \
	$(MYPETRISRCDIR)/PNet.hh \
	$(MYPETRISRCDIR)/PNetType.hh \
	$(MYPETRISRCDIR)/PTransition.hh \
	$(MYPETRISRCDIR)/TimeConstraint.hh \
	$(MYPETRISRCDIR)/TPNet.hh \
	$(MYPETRISRCDIR)/TPNetType.hh \
	$(MYPETRISRCDIR)/TPNetSemantics.hpp 


COMPOSITEHEADERS = 	$(MYCOMPSRCDIR)/Composite.hh \
	$(MYCOMPSRCDIR)/CompositeType.hh \
	$(MYCOMPSRCDIR)/CompositeVarOrderHeuristic.hh \
	$(MYCOMPSRCDIR)/CState.hh \
	$(MYCOMPSRCDIR)/Synchronization.hh \
	$(MYCOMPSRCDIR)/TComposite.hh 

ETFHEADERS = $(MYETFSRCDIR)/ETFType.hh \
	$(MYETFSRCDIR)/etf-util.h \
	$(MYETFSRCDIR)/lts-type.h \
	$(MYETFSRCDIR)/stream.h \
	$(MYETFSRCDIR)/runtime.h \
	$(MYETFSRCDIR)/etf-objects.h \
	$(MYETFSRCDIR)/chunk_support.h 


SCALARHEADERS = $(MYSCALARSRCDIR)/CircularSet.hh \
	$(MYSCALARSRCDIR)/CircularSetType.hh \
	$(MYSCALARSRCDIR)/CircularSetXMLLoader.hh \
	$(MYSCALARSRCDIR)/CircularSync.hh \
	$(MYSCALARSRCDIR)/Delegator.hh \
	$(MYSCALARSRCDIR)/ScalarSet.hh \
	$(MYSCALARSRCDIR)/ScalarSetXMLLoader.hh \
	$(MYSCALARSRCDIR)/ScalarSetType.hh \
	$(MYSCALARSRCDIR)/ScalarState.hh 

GALHEADERS = $(MYGALSRCDIR)/BoolExpression.hh \
	$(MYGALSRCDIR)/ExprHom.hpp \
	$(MYGALSRCDIR)/force.hh \
	$(MYGALSRCDIR)/GAL.hh \
	$(MYGALSRCDIR)/GAL2C.hh \
	$(MYGALSRCDIR)/GALType.hh \
	$(MYGALSRCDIR)/GALVarOrderHeuristic.hh \
	$(MYGALSRCDIR)/IntExpression.hh \
	$(MYGALSRCDIR)/PBoolExpression.hh \
	$(MYGALSRCDIR)/PIntExpression.hh \
	$(MYGALSRCDIR)/Statements.hh \
	$(MYGALSRCDIR)/Variable.hh

WIBBLEHEADERS = $(MYWIBBLESRCDIR)/empty.h \
	$(MYWIBBLESRCDIR)/exception.h \
	$(MYWIBBLESRCDIR)/operators.h \
	$(MYWIBBLESRCDIR)/sfinae.h \
	$(MYWIBBLESRCDIR)/singleton.h \
	$(MYWIBBLESRCDIR)/string.h \
	$(MYWIBBLESRCDIR)/test.h 

DVEHEADERS = $(MYDVESRCDIR)/blob.h  \
	$(MYDVESRCDIR)/common.h  \
	$(MYDVESRCDIR)/hash.h  \
	$(MYDVESRCDIR)/pool.h  \
	$(MYDVESRCDIR)/stateallocator.h  \
	$(MYDVESRCDIR)/dve2GAL.hh  \
	$(MYDVESRCDIR)/dveLoader.hh \
	$(MYDVESRCDIR)/legacy/common/array.hh \
	$(MYDVESRCDIR)/legacy/common/array_of_abstract.hh \
	$(MYDVESRCDIR)/legacy/common/bit_string.hh \
	$(MYDVESRCDIR)/legacy/common/deb.hh \
	$(MYDVESRCDIR)/legacy/common/error.hh \
	$(MYDVESRCDIR)/legacy/common/hash_function.hh \
	$(MYDVESRCDIR)/legacy/common/process_decomposition.hh \
	$(MYDVESRCDIR)/legacy/common/reporter.hh \
	$(MYDVESRCDIR)/legacy/common/stateallocator.hh \
	$(MYDVESRCDIR)/legacy/common/sysinfo.hh \
	$(MYDVESRCDIR)/legacy/common/types.hh \
	$(MYDVESRCDIR)/legacy/common/undeb.hh \
	$(MYDVESRCDIR)/legacy/system/data.hh \
	$(MYDVESRCDIR)/legacy/system/dve/dve_explicit_system.hh \
	$(MYDVESRCDIR)/legacy/system/dve/dve_expression.hh \
	$(MYDVESRCDIR)/legacy/system/dve/dve_process.hh \
	$(MYDVESRCDIR)/legacy/system/dve/dve_process_decomposition.hh \
	$(MYDVESRCDIR)/legacy/system/dve/dve_source_position.hh \
	$(MYDVESRCDIR)/legacy/system/dve/dve_system.hh \
	$(MYDVESRCDIR)/legacy/system/dve/dve_system_trans.hh \
	$(MYDVESRCDIR)/legacy/system/dve/dve_transition.hh \
	$(MYDVESRCDIR)/legacy/system/explicit_system.hh \
	$(MYDVESRCDIR)/legacy/system/expression.hh \
	$(MYDVESRCDIR)/legacy/system/path.hh \
	$(MYDVESRCDIR)/legacy/system/process.hh \
	$(MYDVESRCDIR)/legacy/system/state.hh \
	$(MYDVESRCDIR)/legacy/system/system.hh \
	$(MYDVESRCDIR)/legacy/system/system_abilities.hh \
	$(MYDVESRCDIR)/legacy/system/system_trans.hh \
	$(MYDVESRCDIR)/legacy/system/transition.hh \
	$(MYDVESRCDIR)/legacy/system/dve/syntax_analysis/dve_commonparse.hh \
	$(MYDVESRCDIR)/legacy/system/dve/syntax_analysis/dve_flex_lexer.hh \
	$(MYDVESRCDIR)/legacy/system/dve/syntax_analysis/dve_grammar.hh \
	$(MYDVESRCDIR)/legacy/system/dve/syntax_analysis/dve_gramsymb.hh \
	$(MYDVESRCDIR)/legacy/system/dve/syntax_analysis/dve_parser.hh \
	$(MYDVESRCDIR)/legacy/system/dve/syntax_analysis/dve_symbol.hh \
	$(MYDVESRCDIR)/legacy/system/dve/syntax_analysis/dve_symbol_table.hh \
	$(MYDVESRCDIR)/legacy/system/dve/syntax_analysis/dve_token_vector.hh

AIGERHEADERS = $(MYAIGERDIR)/aiger.h \
  $(MYAIGERDIR)/aigerModel.h

ITSHEADERS = $(PETRIHEADERS) \
	$(COMPOSITEHEADERS) \
	$(ETFHEADERS) \
	$(GALHEADERS) \
	$(SCALARHEADERS) \
	$(WIBBLEHEADERS) \
	$(DVEHEADERS) \
	$(MYANTLRHEADER) \
	$(AIGERHEADERS) \
	$(MYSRCDIR)/ColoaneExport.hh \
	$(MYSRCDIR)/Instance.hh \
	$(MYSRCDIR)/ITSModel.hh \
	$(MYSRCDIR)/Naming.hh \
	$(MYSRCDIR)/Observe_Hom.hh \
	$(MYSRCDIR)/Ordering.hh \
	$(MYSRCDIR)/Options.hh \
	$(MYSRCDIR)/Type.hh \
	$(MYSRCDIR)/TypeBasics.hh \
	$(MYSRCDIR)/TypeCacher.hh \
	$(MYSRCDIR)/TypeDecorator.hh \
	$(MYSRCDIR)/TypeVisitor.hh 

PETRISOURCES = 	$(MYPETRISRCDIR)/Arc.cpp \
	$(MYPETRISRCDIR)/Modular2ITS.cpp \
	$(MYPETRISRCDIR)/Place.cpp \
	$(MYPETRISRCDIR)/PNet.cpp \
	$(MYPETRISRCDIR)/PTransition.cpp \
	$(MYPETRISRCDIR)/TPNet.cpp 


COMPOSITESOURCES = $(MYCOMPSRCDIR)/Composite.cpp \
	$(MYCOMPSRCDIR)/CompositeType.cpp \
	$(MYCOMPSRCDIR)/CompositeVarOrderHeuristic.cpp \
	$(MYCOMPSRCDIR)/Synchronization.cpp \
	$(MYCOMPSRCDIR)/TComposite.cpp 

ETFSOURCES = $(MYETFSRCDIR)/ETFType.cpp \
	$(MYETFSRCDIR)/etf-util.c \
	$(MYETFSRCDIR)/dynamic-array.c \
	$(MYETFSRCDIR)/stringindex.c \
	$(MYETFSRCDIR)/chunk_support.c \
	$(MYETFSRCDIR)/fast_hash.c \
	$(MYETFSRCDIR)/etf-parser.c \
	$(MYETFSRCDIR)/ltsmin-syntax.c \
	$(MYETFSRCDIR)/ltsmin-grammar.c \
	$(MYETFSRCDIR)/ltsmin-lexer.c \
	$(MYETFSRCDIR)/lts-type.c \
	$(MYETFSRCDIR)/etf-objects.c \
	$(MYETFSRCDIR)/runtime.c \
	$(MYETFSRCDIR)/stream.c \
	$(MYETFSRCDIR)/treedbs.c \
	$(MYETFSRCDIR)/stream_buffer.c \
	$(MYETFSRCDIR)/stream_diff32.c \
	$(MYETFSRCDIR)/stream_fd.c \
	$(MYETFSRCDIR)/stream_mem.c

GALSOURCES = $(MYGALSRCDIR)/ExprHom.cpp \
	$(MYGALSRCDIR)/force.cpp \
	$(MYGALSRCDIR)/GALType.cpp \
	$(MYGALSRCDIR)/GALVarOrderHeuristic.cpp \
	$(MYGALSRCDIR)/GAL.cpp \
	$(MYGALSRCDIR)/GAL2C.cpp \
	$(MYGALSRCDIR)/IntExpression.cpp \
	$(MYGALSRCDIR)/PBoolExpression.cpp \
	$(MYGALSRCDIR)/PIntExpression.cpp \
	$(MYGALSRCDIR)/Statements.cpp \
	$(MYGALSRCDIR)/Variable.cpp 


SCALARSOURCES = $(MYSCALARSRCDIR)/CircularSet.cpp \
	$(MYSCALARSRCDIR)/CircularSetXMLLoader.cpp \
	$(MYSCALARSRCDIR)/CircularSync.cpp \
	$(MYSCALARSRCDIR)/Delegator.cpp \
	$(MYSCALARSRCDIR)/ScalarSet.cpp \
	$(MYSCALARSRCDIR)/ScalarSetXMLLoader.cpp \
	$(MYSCALARSRCDIR)/ScalarSetType.cpp 

WIBBLESOURCES = $(MYWIBBLESRCDIR)/exception.cpp \
	$(MYWIBBLESRCDIR)/string.cpp \
	$(MYWIBBLESRCDIR)/test.cpp

DVESOURCES =	$(MYDVESRCDIR)/pool.cpp  \
	$(MYDVESRCDIR)/dve2GAL.cpp \
	$(MYDVESRCDIR)/dveLoader.cpp \
	$(MYDVESRCDIR)/legacy/common/error.cc \
	$(MYDVESRCDIR)/legacy/common/hash_function.cc \
	$(MYDVESRCDIR)/legacy/common/reporter.cc \
	$(MYDVESRCDIR)/legacy/common/sysinfo.cc \
	$(MYDVESRCDIR)/legacy/system/dve/dve_explicit_system.cc \
	$(MYDVESRCDIR)/legacy/system/dve/dve_expression.cc \
	$(MYDVESRCDIR)/legacy/system/dve/dve_process.cc \
	$(MYDVESRCDIR)/legacy/system/dve/dve_process_decomposition.cc \
	$(MYDVESRCDIR)/legacy/system/dve/dve_source_position.cc \
	$(MYDVESRCDIR)/legacy/system/dve/dve_system.cc \
	$(MYDVESRCDIR)/legacy/system/dve/dve_system_trans.cc \
	$(MYDVESRCDIR)/legacy/system/dve/dve_transition.cc \
	$(MYDVESRCDIR)/legacy/system/path.cc \
	$(MYDVESRCDIR)/legacy/system/process.cc \
	$(MYDVESRCDIR)/legacy/system/state.cc \
	$(MYDVESRCDIR)/legacy/system/system.cc \
	$(MYDVESRCDIR)/legacy/system/system_trans.cc \
	$(MYDVESRCDIR)/legacy/system/transition.cc \
	$(MYDVESRCDIR)/legacy/system/dve/syntax_analysis/dve_expr_grammar.cc \
	$(MYDVESRCDIR)/legacy/system/dve/syntax_analysis/dve_grammar.cc \
	$(MYDVESRCDIR)/legacy/system/dve/syntax_analysis/dve_lexer.cc \
	$(MYDVESRCDIR)/legacy/system/dve/syntax_analysis/dve_parser.cc \
	$(MYDVESRCDIR)/legacy/system/dve/syntax_analysis/dve_proc_grammar.cc \
	$(MYDVESRCDIR)/legacy/system/dve/syntax_analysis/dve_symbol.cc \
	$(MYDVESRCDIR)/legacy/system/dve/syntax_analysis/dve_symbol_table.cc \
	$(MYDVESRCDIR)/legacy/system/dve/syntax_analysis/dve_trans_grammar.cc 

AIGERSOURCES = $(MYAIGERDIR)/aiger.c \
  $(MYAIGERDIR)/aigerModel.cpp

ITSSOURCES = $(ITSHEADERS) \
	$(PETRISOURCES) \
	$(COMPOSITESOURCES) \
	$(ETFSOURCES) \
	$(GALSOURCES) \
	$(SCALARSOURCES) \
	$(WIBBLESOURCES) \
	$(DVESOURCES) \
	$(MYANTLRSOURCES) \
	$(AIGERSOURCES) \
	$(MYSRCDIR)/ColoaneExport.cpp \
	$(MYSRCDIR)/ITSModel.cpp \
	$(MYSRCDIR)/Observe_Hom.cpp \
	$(MYSRCDIR)/Ordering.cpp \
	$(MYSRCDIR)/Options.cpp \
	$(MYSRCDIR)/TypeBasics.cpp \
	$(MYSRCDIR)/TypeCacher.cpp \
	$(JSONSTUFF) \
	$(XMLLOADERSOURCES)

if WITH_LIBDDD_PATH
LIBDDD_CPP=-I$(LIBDDD_ROOT)/include
LIBDDD_PATH=$(LIBDDD_ROOT)/libDDD.a
else
LIBDDD_LINK=-lddd
endif

if WITH_LIBEXPAT_PATH
LIBEXPAT_CPP=-I$(LIBEXPAT_ROOT)/include
LIBEXPAT_PATH=$(LIBEXPAT_ROOT)/lib/libexpat.la
else
LIBEXPAT_LINK=-lexpat
endif

if WITH_ANTLRC_PATH
ANTLRC_CPP=-I$(ANTLRC_ROOT)/include
ANTLRC_PATH=$(ANTLRC_ROOT)/lib/libantlr3c.a
else
ANTLRC_LINK=-lantlr3c
endif


AM_CFLAGS = -g -O3 -Wall
AM_CPPFLAGS = $(LIBDDD_CPP) $(LIBEXPAT_CPP) $(ANTLRC_CPP) -I $(abs_top_srcdir)  -I $(MYSRCDIR) -I $(MYPRODSRCDIR) -I ./ -DLIBDDD=\"$(LIBDDD_ROOT)\" -DLIBITS=\"$(MYSRCDIR)\"
AM_CXXFLAGS = -g -O3 -Wall

libITSmain_a_SOURCES = $(ITSSOURCES)

otherincludedir = $(includedir)/include
nobase_otherinclude_HEADERS = $(ITSHEADERS)

libITS_a_SOURCES = $(ITSSOURCES)

libITS.a : libITSmain.a libprod.a libCAMI.a libprod.a libRdP.a
	mkdir .tmp-libs ; cd .tmp-libs ; $(AR) x ../libITSmain.a ; $(AR) x ../libprod.a ; $(AR) x ../libRdP.a ; $(AR) x ../libCAMI.a ; $(AR) x ../libprod.a ; $(AR) cru ../libITS.a *.o ; cd .. ; $(RANLIB) libITS.a ; \rm -r .tmp-libs ;


all-local : libITS.a
