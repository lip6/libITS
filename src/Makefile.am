noinst_PROGRAMS = its-ltl testite extractAP

if WITH_LIBDDD_PATH
LIBDDD_CPP=-I$(LIBDDD_ROOT)/include
LIBDDD_PATH=$(LIBDDD_ROOT)/lib/libDDD.a
else
LIBDDD_LINK=-lDDD
endif

if WITH_LIBITS_PATH
LIBITS_CPP=-I$(LIBITS_ROOT)/include
LIBITS_PATH=$(LIBITS_ROOT)/lib/libITS.a
else
LIBITS_LINK=-lITS
endif


if WITH_LIBSPOT_PATH
LIBSPOT_CPP=-I$(LIBSPOT_ROOT) -I$(LIBSPOT_ROOT)/..
LIBSPOT_PATH=$(LIBSPOT_ROOT)/../../lib/libspot.a $(LIBSPOT_ROOT)/../../lib/libbddx.a
else
LIBSPOT_LINK=-lspot -lbddx
endif

if WITH_LIBEXPAT_PATH
LIBEXPAT_CPP=-I$(LIBEXPAT_ROOT)/include
LIBEXPAT_PATH=$(LIBEXPAT_ROOT)/lib/libexpat.a
else
LIBEXPAT_LINK=-lexpat
endif



if WITH_ANTLRC_PATH
ANTLRC_CPP=-I$(ANTLRC_ROOT)/include
ANTLRC_PATH=$(ANTLRC_ROOT)/lib/libantlr3c.a
else
ANTLRC_LINK=-lantlr3c
endif


AM_CPPFLAGS = $(LIBSPOT_CPP) $(LIBITS_CPP) $(LIBDDD_CPP)  -g -O2 -Wall $(BOOST_CPPFLAGS) $(LIBEXPAT_CPP) $(ANTLRC_CPP)
AM_CXXFLAGS = -g -O2 -Wall

LDADDFLAGS = $(LIBITS_PATH) $(LIBSPOT_PATH) $(LIBSPOT_LINK) $(LIBDDD_PATH)  $(LIBEXPAT_PATH) $(LIBEXPAT_LINK) $(ANTLRC_PATH) $(ANTLRC_LINK)

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
	slaptgta.hh \
	etf/ETFTestingType.hh \
	fsltltesting.hh \
	sogkripke.hh \
	tgbaIts.hh \
	dsogtgta.hh



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
	slaptgta.ccp \
	etf/ETFTestingType.cpp \
	fsltltesting.cpp \
	sogkripke.ccp \
	tgbaIts.cpp \
    dsogtgta.cpp

its_ltl_SOURCES = $(MYSRC) $(MYHEADER)

its_ltl_LDADD = $(LDADDFLAGS)
# its_ltl_LDFLAGS =

testite_SOURCES = testIterator.cpp apiterator.hh apiterator.cpp
testite_LDADD =  $(LDADDFLAGS)

extractAP_SOURCES = extractAP.cpp
extractAP_LDADD =  $(LDADDFLAGS)
