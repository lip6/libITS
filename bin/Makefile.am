noinst_PROGRAMS = its-reach


LIBITS_CPP=-I$(top_srcdir)/src
LIBITS_PATH=$(top_srcdir)/lib/libITS.a 


if HAVE_BOOST
HAVE_BOOST_FLAG = -DHAVE_BOOST
endif

if WITH_LIBDDD_PATH
LIBDDD_CPP=-I$(LIBDDD_ROOT)
LIBDDD_PATH=$(LIBDDD_ROOT)/libDDD_d.a
else
LIBDDD_LINK=-lddd
endif


AM_CPPFLAGS = $(LIBDDD_CPP) $(LIBITS_CPP) $(BOOST_CPPFLAGS) $(HAVE_BOOST_FLAG) -g -O2 -Wall 
AM_CXXFLAGS = -g -O2 -Wall

LDADDFLAGS = $(LIBITS_PATH)  $(LIBDDD_PATH) -lexpat 


its_reach_SOURCES = main.cpp 

its_reach_LDADD = $(LDADDFLAGS)  

