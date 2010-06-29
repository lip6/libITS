SUBDIRS = bringBack
lib_LIBRARIES = libITS.a libprod.a

MYSRCDIR= $(top_srcdir)/src
MYPRODSRCDIR= $(MYSRCDIR)/prod
MYJSONSRCDIR= $(MYSRCDIR)/parser_json
MYJSONSPIRITSRCDIR= $(MYJSONSRCDIR)/json_spirit
MYPETRISRCDIR = $(MYSRCDIR)/petri
MYCOMPSRCDIR = $(MYSRCDIR)/composite
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
		$(MYJSONSRCDIR)/hierarchie.hh   \
		$(MYJSONSRCDIR)/JSException.cpp	 \
		$(MYJSONSRCDIR)/JSException.hh	 \
		$(MYJSONSRCDIR)/parse_json.cpp   \
		$(MYJSONSRCDIR)/parse_json.hh

XMLLOADERSOURCES =  $(MYCOMPSRCDIR)/CompositeXMLLoader.hh $(MYCOMPSRCDIR)/CompositeXMLLoader.cpp \
	$(MYSRCDIR)/ITSModelXMLLoader.hh $(MYSRCDIR)/ITSModelXMLLoader.cpp \
	$(MYPETRISRCDIR)/XMLLoader.hh $(MYPETRISRCDIR)/XMLLoader.cpp


PETRIHEADERS = 	$(MYPETRISRCDIR)/Arc.hh \
	$(MYPETRISRCDIR)/JSON2ITS.hh \
	$(MYPETRISRCDIR)/JSONPNetType.hh \
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
	$(MYPETRISRCDIR)/JSON2ITS.cpp \
	$(MYPETRISRCDIR)/JSONPNetType.cpp \
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
	$(SCALARSOURCES) \
	$(MYSRCDIR)/ColoaneExport.cpp \
	$(MYSRCDIR)/ITSModel.cpp \
	$(MYSRCDIR)/Ordering.cpp \
	$(MYSRCDIR)/TypeBasics.cpp \
	$(MYSRCDIR)/TypeCacher.cpp \
	$(JSONSOURCES) \
	$(XMLLOADERSOURCES)


if WITH_LIBDDD_PATH
LIBDDD_CPP=-I$(LIBDDD_ROOT)
LIBDDD_PATH=$(LIBDDD_ROOT)/libDDD.a
else
LIBDDD_LINK=-lddd
endif

AM_CPPFLAGS = $(LIBDDD_CPP)  -I $(top_srcdir)/src  -I $(MYSRCDIR) -I $(MYPRODSRCDIR) -I ./ -g -O0 -Wall $(BOOST_CPPFLAGS)
AM_CXXFLAGS = -g -O0 -Wall

libITS_a_SOURCES = $(ITSSOURCES)
