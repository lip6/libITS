noinst_PROGRAMS = its-ltl testite
# A simple program to extract atomic propositions from LTL formulae
# extractAP
# just a small test for AP
# testite


if WITH_LIBDDD_PATH
LIBDDD_CPP=-I$(LIBDDD_ROOT)
LIBDDD_PATH=$(LIBDDD_ROOT)/libDDD_d.a
else
LIBDDD_LINK=-lddd
endif

if WITH_LIBITS_PATH
LIBITS_CPP=-I$(LIBITS_ROOT)
LIBITS_PATH=$(LIBITS_ROOT)/../lib/libITS.a 
else
LIBITS_LINK=-lITS
endif


if WITH_LIBSPOT_PATH
LIBSPOT_CPP=-I$(LIBSPOT_ROOT) -I$(LIBSPOT_ROOT)/..
LIBSPOT_PATH=$(LIBSPOT_ROOT)/../../lib/libspot.a $(LIBSPOT_ROOT)/../../lib/libbdd.a
else
LIBSPOT_LINK=-lspot
endif

if WITH_LIBEXPAT_PATH
LIBEXPAT_CPP=-I$(LIBEXPAT_ROOT)/include
LIBEXPAT_PATH=$(LIBEXPAT_ROOT)/lib/libexpat.a
else
LIBEXPAT_LINK=-lexpat
endif


AM_CPPFLAGS = $(LIBSPOT_CPP) $(LIBITS_CPP) $(LIBDDD_CPP)  -g -O0 -Wall $(BOOST_CPPFLAGS) $(LIBEXPAT_CPP)
AM_CXXFLAGS = -g -O0 -Wall

LDADDFLAGS = $(LIBITS_PATH) $(LIBSPOT_PATH)  $(LIBDDD_PATH)  $(LIBEXPAT_PATH) $(LIBEXPAT_LINK) 

MYHEADER =apiterator.hh \
	bczCSD99.hh \
	dsog.hh \
	fsltl.hh \
	slap.hh \
	sogIts.hh \
	sogstate.hh \
	sogsucciter.hh \
	sogtgba.hh \
	sogtgbautils.hh \
	tgbaIts.hh


MYSRC = apiterator.cpp \
	bczCSD99.cpp \
	dsog.cpp \
	fsltl.cpp \
	main.cpp \
	slap.cpp \
	sogIts.cpp \
	sogstate.cpp \
	sogsucciter.cpp \
	sogtgba.cpp \
	sogtgbautils.cpp \
	tgbaIts.cpp

its_ltl_SOURCES = $(MYSRC) $(MYHEADER)

its_ltl_LDADD = $(LDADDFLAGS) 
its_ltl_LDFLAGS = -rdynamic -ldl

testite_SOURCES = testIterator.cpp apiterator.hh apiterator.cpp
testite_LDADD =  $(LDADDFLAGS)

#extractAP_SOURCES = extractAP.cpp
#extractAP_LDADD =  $(LDADDFLAGS)
