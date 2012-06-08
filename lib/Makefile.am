SUBDIRS = bringBack
lib_LIBRARIES = libITSmain.a libprod.a

MYSRCDIR= $(abs_top_srcdir)/src
MYPRODSRCDIR= $(MYSRCDIR)/prod
MYJSONSRCDIR= $(MYSRCDIR)/parser_json
MYPETRISRCDIR = $(MYSRCDIR)/petri
MYCOMPSRCDIR = $(MYSRCDIR)/composite
MYETFSRCDIR = $(MYSRCDIR)/etf
MYSCALARSRCDIR = $(MYSRCDIR)/scalar
MYGALSRCDIR = $(MYSRCDIR)/gal
MYWIBBLESRCDIR = $(MYSRCDIR)/divine/wibble
MYDVESRCDIR = $(MYSRCDIR)/divine

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
	$(MYCOMPSRCDIR)/CState.hh \
	$(MYCOMPSRCDIR)/Synchronization.hh \
	$(MYCOMPSRCDIR)/TComposite.hh 

ETFHEADERS = $(MYETFSRCDIR)/ETFType.hh \
	$(MYETFSRCDIR)/etf-util.h



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
	$(MYGALSRCDIR)/GAL.hh \
	$(MYGALSRCDIR)/GALType.hh \
	$(MYGALSRCDIR)/IntExpression.hh \
	$(MYGALSRCDIR)/PBoolExpression.hh \
	$(MYGALSRCDIR)/PIntExpression.hh \
	$(MYGALSRCDIR)/Variable.hh

WIBBLEHEADERS = $(MYWIBBLESRCDIR)/empty.h \
	$(MYWIBBLESRCDIR)/exception.h \
	$(MYWIBBLESRCDIR)/operators.h \
	$(MYWIBBLESRCDIR)/sfinae.h \
	$(MYWIBBLESRCDIR)/singleton.h \
	$(MYWIBBLESRCDIR)/string.h \
	$(MYWIBBLESRCDIR)/test.h 

DVEHEADERS = $(MYDVESRCDIR)/dve2GAL.hh \
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



ITSHEADERS = $(PETRIHEADERS) \
	$(COMPOSITEHEADERS) \
	$(ETFHEADERS) \
	$(GALHEADERS) \
	$(SCALARHEADERS) \
	$(WIBBLEHEADERS) \
	$(DVEHEADERS) \
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
	$(MYGALSRCDIR)/GALType.cpp \
	$(MYGALSRCDIR)/GAL.cpp \
	$(MYGALSRCDIR)/IntExpression.cpp \
	$(MYGALSRCDIR)/PBoolExpression.cpp \
	$(MYGALSRCDIR)/PIntExpression.cpp 


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

DVESOURCES = 	 $(MYDVESRCDIR)/dve2GAL.cpp \
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

ITSSOURCES = $(ITSHEADERS) \
	$(PETRISOURCES) \
	$(COMPOSITESOURCES) \
	$(ETFSOURCES) \
	$(GALSOURCES) \
	$(SCALARSOURCES) \
	$(WIBBLESOURCES) \
	$(DVESOURCES) \
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
LIBDDD_CPP=-I$(LIBDDD_ROOT)
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

AM_CFLAGS = $(LIBDDD_CPP) $(LIBEXPAT_CPP) -I $(top_srcdir)/src  -I $(MYSRCDIR) -I ./ -g -O2 -std=gnu99 -Wall
AM_CPPFLAGS = $(LIBDDD_CPP) $(LIBEXPAT_CPP) -I $(top_srcdir)/src  -I $(MYSRCDIR) -I $(MYPRODSRCDIR) -I ./ -g -O2 -Wall $(BOOST_CPPFLAGS) -DLIBDDD=\"$(LIBDDD_ROOT)\" -DLIBITS=\"$(MYSRCDIR)\"
AM_CXXFLAGS = -g -O2 -Wall

libITSmain_a_SOURCES = $(ITSSOURCES)


libITS.a : libITSmain.a libprod.a libCAMI.a libprod.a libRdP.a
	mkdir .tmp-libs ; cd .tmp-libs ; ar x ../libITSmain.a ; ar x ../libprod.a ; ar x ../libRdP.a ; ar x ../libCAMI.a ; ar x ../libprod.a ; ar cru ../libITS.a *.o ; cd .. ; ranlib libITS.a ; \rm -r .tmp-libs ;


all-local : libITS.a
