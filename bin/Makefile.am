noinst_PROGRAMS = romeo-sdd

ITSLIBPATH= $(top_builddir)/src/its/lib/libITS.a  $(top_builddir)/src/its/lib/libRdP.a

if WITH_LIBITS_PATH
LIBITS_CPP=-I$(LIBITS_ROOT)
LIBITS_PATH=$(LIBITS_ROOT)/../lib/libITS.a $(LIBITS_ROOT)/../lib/libprod.a $(LIBITS_ROOT)/../lib/libRdP.a
else
LIBITS_LINK=-lITS
endif

if WITH_LIBDDD_PATH
LIBDDD_CPP=-I$(LIBDDD_ROOT)
LIBDDD_PATH=$(LIBDDD_ROOT)/libDDD.a
else
LIBDDD_LINK=-lddd
endif


AM_CPPFLAGS = $(LIBDDD_CPP) $(LIBITS_CPP)  -g -O2 -Wall
AM_CXXFLAGS = -g -O2 -Wall

LDADDFLAGS = $(LIBITS_PATH)  $(LIBDDD_PATH) -lexpat 


romeo_sdd_SOURCES = main.cpp \
	CSMACD.hh CSMACD.cpp \
	examples.hh examples.cpp \
	fischer.hh fischer.cpp \
	train.hh train.cpp 

romeo_sdd_LDADD = $(LDADDFLAGS)  

