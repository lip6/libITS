SUBDIRS = bringBack
lib_LIBRARIES = libITS.a libprod.a

MYSRCDIR= $(top_srcdir)/src
MYPRODSRCDIR= $(MYSRCDIR)/prod
MYJSONSRCDIR= $(MYSRCDIR)/parser_json
MYJSONSPIRITSRCDIR= $(MYJSONSRCDIR)/json_spirit

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

XMLLOADERSOURCES =  $(MYSRCDIR)/CompositeXMLLoader.hh $(MYSRCDIR)/CompositeXMLLoader.cpp \
	$(MYSRCDIR)/ITSModelXMLLoader.hh $(MYSRCDIR)/ITSModelXMLLoader.cpp \
	$(MYSRCDIR)/XMLLoader.hh $(MYSRCDIR)/XMLLoader.cpp


ITSHEADERS = 	$(MYSRCDIR)/Arc.hh \
	$(MYSRCDIR)/CircularSet.hh \
	$(MYSRCDIR)/CircularSetType.hh \
	$(MYSRCDIR)/CircularSetXMLLoader.hh \
	$(MYSRCDIR)/CircularSync.hh \
	$(MYSRCDIR)/Composite.hh \
	$(MYSRCDIR)/CompositeType.hh \
	$(MYSRCDIR)/CState.hh \
	$(MYSRCDIR)/Delegator.hh \
	$(MYSRCDIR)/JSON2ITS.hh \
	$(MYSRCDIR)/Hom_PlaceArcs.hh \
	$(MYSRCDIR)/Hom_Basic.hh \
	$(MYSRCDIR)/Instance.hh \
	$(MYSRCDIR)/ITSModel.hh \
	$(MYSRCDIR)/Marking.hh \
	$(MYSRCDIR)/Modular2ITS.hh \
	$(MYSRCDIR)/Naming.hh \
	$(MYSRCDIR)/Ordering.hh \
	$(MYSRCDIR)/Place.hh \
	$(MYSRCDIR)/PNet.hh \
	$(MYSRCDIR)/PNetType.hh \
	$(MYSRCDIR)/PTransition.hh \
	$(MYSRCDIR)/ScalarSet.hh \
	$(MYSRCDIR)/ScalarSetXMLLoader.hh \
	$(MYSRCDIR)/ScalarSetType.hh \
	$(MYSRCDIR)/ScalarState.hh \
	$(MYSRCDIR)/Synchronization.hh \
	$(MYSRCDIR)/TimeConstraint.hh \
	$(MYSRCDIR)/TComposite.hh \
	$(MYSRCDIR)/TPNet.hh \
	$(MYSRCDIR)/TPNetType.hh \
	$(MYSRCDIR)/TPNetSemantics.hpp \
	$(MYSRCDIR)/Type.hh \
	$(MYSRCDIR)/TypeBasics.hh \
	$(MYSRCDIR)/TypeCacher.hh 

ITSSOURCES = $(ITSHEADERS) \
	$(MYSRCDIR)/Arc.cpp \
	$(MYSRCDIR)/CircularSet.cpp \
	$(MYSRCDIR)/CircularSetXMLLoader.cpp \
	$(MYSRCDIR)/CircularSync.cpp \
	$(MYSRCDIR)/Composite.cpp \
	$(MYSRCDIR)/CompositeType.cpp \
	$(MYSRCDIR)/Delegator.cpp \
	$(MYSRCDIR)/JSON2ITS.cpp \
	$(MYSRCDIR)/Hom_Basic.cpp \
	$(MYSRCDIR)/ITSModel.cpp \
	$(MYSRCDIR)/Modular2ITS.cpp \
	$(MYSRCDIR)/Ordering.cpp \
	$(MYSRCDIR)/Place.cpp \
	$(MYSRCDIR)/PNet.cpp \
	$(MYSRCDIR)/PTransition.cpp \
	$(MYSRCDIR)/ScalarSet.cpp \
	$(MYSRCDIR)/ScalarSetXMLLoader.cpp \
	$(MYSRCDIR)/ScalarSetType.cpp \
	$(MYSRCDIR)/ScalarState.cpp \
	$(MYSRCDIR)/Synchronization.cpp \
	$(MYSRCDIR)/TComposite.cpp \
	$(MYSRCDIR)/TPNet.cpp \
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

AM_CPPFLAGS = $(LIBDDD_CPP)  -I $(top_srcdir)/src/its/src/lib  -I $(MYSRCDIR) -I $(MYPRODSRCDIR) -I ./ -g -O2 -Wall $(BOOST_CPPFLAGS)
AM_CXXFLAGS = -g -O2 -Wall

libITS_a_SOURCES = $(ITSSOURCES)
