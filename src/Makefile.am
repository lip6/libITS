noinst_PROGRAMS = sog-its testite


if WITH_LIBDDD_PATH
LIBDDD_CPP=-I$(LIBDDD_ROOT)
LIBDDD_PATH=$(LIBDDD_ROOT)/libDDD.a
else
LIBDDD_LINK=-lddd
endif

if WITH_LIBITS_PATH
LIBITS_CPP=-I$(LIBITS_ROOT)
LIBITS_PATH=$(LIBITS_ROOT)/../lib/libITS.a $(LIBITS_ROOT)/../lib/libprod.a $(LIBITS_ROOT)/../lib/libRdP.a 
else
LIBITS_LINK=-lITS
endif


if WITH_LIBSPOT_PATH
LIBSPOT_CPP=-I$(LIBSPOT_ROOT)
LIBSPOT_PATH=$(LIBSPOT_ROOT)/../../lib/libspot.a $(LIBSPOT_ROOT)/../../lib/libbdd.a
else
LIBSPOT_LINK=-lspot
endif

AM_CPPFLAGS = $(LIBSPOT_CPP) $(LIBITS_CPP) $(LIBDDD_CPP) -I $(top_srcdir)/src/its/src/  -g -O2 -Wall
AM_CXXFLAGS = -g -O2 -Wall

LDADDFLAGS = $(LIBITS_PATH) $(LIBSPOT_PATH)  $(LIBDDD_PATH) -lexpat

MYHEADER =apiterator.hh \
	fsltl.hh \
	slog.hh \
	sogIts.hh \
	sogstate.hh \
	sogsucciter.hh \
	sogtgba.hh \
	sogtgbautils.hh \
	tgbaIts.hh


MYSRC = apiterator.cpp \
	fsltl.cpp \
	main.cpp \
	slog.cpp \
	sogIts.cpp \
	sogstate.cpp \
	sogsucciter.cpp \
	sogtgba.cpp \
	sogtgbautils.cpp \
	tgbaIts.cpp

sog_its_SOURCES = $(MYSRC) $(MYHEADER)

sog_its_LDADD = $(LDADDFLAGS)  

testite_SOURCES = testIterator.cpp apiterator.hh apiterator.cpp 
testite_LDADD =  $(LDADDFLAGS)  




