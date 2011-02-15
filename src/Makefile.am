noinst_PROGRAMS = its-ltl 
# A simple program to extract atomic propositions from LTL formulae
# extractAP
# just a small test for AP
# testite


if WITH_LIBDDD_PATH
LIBDDD_CPP=-I$(LIBDDD_ROOT)
LIBDDD_PATH=$(LIBDDD_ROOT)/libDDD.a
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

AM_CPPFLAGS = $(LIBSPOT_CPP) $(LIBITS_CPP) $(LIBDDD_CPP)  -g -O2 -Wall $(BOOST_CPPFLAGS)
AM_CXXFLAGS = -g -O2 -Wall

LDADDFLAGS = $(LIBITS_PATH) $(LIBSPOT_PATH)  $(LIBDDD_PATH) -lexpat

MYHEADER =apiterator.hh \
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


#testite_SOURCES = testIterator.cpp apiterator.hh apiterator.cpp
#testite_LDADD =  $(LDADDFLAGS)

#extractAP_SOURCES = extractAP.cpp
#extractAP_LDADD =  $(LDADDFLAGS)
