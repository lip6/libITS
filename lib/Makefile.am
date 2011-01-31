SUBDIRS = bringBack
lib_LIBRARIES = libITS.a libprod.a

MYSRCDIR= $(top_srcdir)/src
MYPRODSRCDIR= $(MYSRCDIR)/prod
MYJSONSRCDIR= $(MYSRCDIR)/parser_json
MYJSONSPIRITSRCDIR= $(MYJSONSRCDIR)/json_spirit
MYPETRISRCDIR = $(MYSRCDIR)/petri
MYCOMPSRCDIR = $(MYSRCDIR)/composite
MYETFSRCDIR = $(MYSRCDIR)/etf
MYSCALARSRCDIR = $(MYSRCDIR)/scalar


libprod_a_YFLAGS = -d 
libprod_a_LFLAGS = -olex.yy.c

#BUILT_SOURCES =  ./ITSProdLoader_yacc.h

libprod_a_SOURCES = $(MYPRODSRCDIR)/ProdLoader.hh \
		$(MYPRODSRCDIR)/ITSProdLoader_yacc.yxx \
		$(MYPRODSRCDIR)/ITSProdLoader_lex.ll 


JSONSPIRITSOURCES = $(MYJSONSPIRITSRCDIR)/json_spirit.h \
	 		 $(MYJSONSPIRITSRCDIR)/json_spirit_reader.cpp \
			 $(MYJSONSPIRITSRCDIR)/json_spirit_reader.h  \
			 $(MYJSONSPIRITSRCDIR)/json_spirit_utils.h \
			 $(MYJSONSPIRITSRCDIR)/json_spirit_value.cpp \
			 $(MYJSONSPIRITSRCDIR)/json_spirit_value.h \
			 $(MYJSONSPIRITSRCDIR)/json_spirit_writer.cpp \
			 $(MYJSONSPIRITSRCDIR)/json_spirit_writer.h

JSONSOURCES = $(JSONSPIRITSOURCES) \
		$(MYJSONSRCDIR)/hierarchie.cpp	 \
		$(MYJSONSRCDIR)/json_model.cpp	 \
		$(MYJSONSRCDIR)/hierarchie.hh   \
		$(MYJSONSRCDIR)/JSException.cpp	 \
		$(MYJSONSRCDIR)/JSException.hh	 \
		$(MYJSONSRCDIR)/parse_json.cpp   \
		$(MYJSONSRCDIR)/parse_json.hh \
		$(MYPETRISRCDIR)/JSON2ITS.cpp \
		$(MYPETRISRCDIR)/JSON2ITS.hh \
		$(MYPETRISRCDIR)/JSONPNetType.hh \
		$(MYPETRISRCDIR)/JSONPNetType.cpp 

if HAVE_BOOST
JSONSTUFF = $(JSONSOURCES)
endif


XMLLOADERSOURCES =  $(MYCOMPSRCDIR)/CompositeXMLLoader.hh $(MYCOMPSRCDIR)/CompositeXMLLoader.cpp \
	$(MYSRCDIR)/ITSModelXMLLoader.hh $(MYSRCDIR)/ITSModelXMLLoader.cpp \
	$(MYPETRISRCDIR)/XMLLoader.hh $(MYPETRISRCDIR)/XMLLoader.cpp


PETRIHEADERS = 	$(MYPETRISRCDIR)/Arc.hh \
	$(MYPETRISRCDIR)/Hom_PlaceArcs.hh \
	$(MYPETRISRCDIR)/Hom_Basic.hh \
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


ITSHEADERS = $(PETRIHEADERS) \
	$(COMPOSITEHEADERS) \
	$(ETFHEADERS) \
	$(SCALARHEADERS) \
	$(MYSRCDIR)/ColoaneExport.hh \
	$(MYSRCDIR)/Instance.hh \
	$(MYSRCDIR)/ITSModel.hh \
	$(MYSRCDIR)/Naming.hh \
	$(MYSRCDIR)/Ordering.hh \
	$(MYSRCDIR)/Type.hh \
	$(MYSRCDIR)/TypeBasics.hh \
	$(MYSRCDIR)/TypeCacher.hh \
	$(MYSRCDIR)/TypeVisitor.hh 

PETRISOURCES = 	$(MYPETRISRCDIR)/Arc.cpp \
	$(MYPETRISRCDIR)/Hom_Basic.cpp \
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




SCALARSOURCES = $(MYSCALARSRCDIR)/CircularSet.cpp \
	$(MYSCALARSRCDIR)/CircularSetXMLLoader.cpp \
	$(MYSCALARSRCDIR)/CircularSync.cpp \
	$(MYSCALARSRCDIR)/Delegator.cpp \
	$(MYSCALARSRCDIR)/ScalarSet.cpp \
	$(MYSCALARSRCDIR)/ScalarSetXMLLoader.cpp \
	$(MYSCALARSRCDIR)/ScalarSetType.cpp \
	$(MYSCALARSRCDIR)/ScalarState.cpp 


ITSSOURCES = $(ITSHEADERS) \
	$(PETRISOURCES) \
	$(COMPOSITESOURCES) \
	$(ETFSOURCES) \
	$(SCALARSOURCES) \
	$(MYSRCDIR)/ColoaneExport.cpp \
	$(MYSRCDIR)/ITSModel.cpp \
	$(MYSRCDIR)/Ordering.cpp \
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

AM_CPPFLAGS = $(LIBDDD_CPP)  -I $(top_srcdir)/src  -I $(MYSRCDIR) -I $(MYPRODSRCDIR) -I ./ -g -O2 -std=gnu99 -Wall $(BOOST_CPPFLAGS)
AM_CXXFLAGS = -g -O2 -Wall

libITS_a_SOURCES = $(ITSSOURCES)
