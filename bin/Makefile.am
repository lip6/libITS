noinst_PROGRAMS = its-reach gal-test


LIBITS_CPP=-I$(top_srcdir)/src
LIBITS_PATH=$(top_srcdir)/lib/libITS.a 


if WITH_LIBDDD_PATH
LIBDDD_CPP=-I$(LIBDDD_ROOT)
LIBDDD_PATH=$(LIBDDD_ROOT)/libDDD.a
else
LIBDDD_LINK=-lddd
endif

if WITH_LIBEXPAT_PATH
LIBEXPAT_CPP=-I$(LIBEXPAT_ROOT)/include
LIBEXPAT_PATH=$(LIBEXPAT_ROOT)/lib/libexpat.a
else
LIBEXPAT_LINK=-lexpat
endif


AM_CPPFLAGS = $(LIBDDD_CPP) $(HAVE_BOOST_FLAG) $(LIBITS_CPP) $(BOOST_CPPFLAGS) $(LIBEXPAT_CPP) $(HAVE_BOOST_FLAG) -g -O2 -Wall 
AM_CXXFLAGS = -g -O2 -Wall

LDADDFLAGS = $(LIBITS_PATH)  $(LIBDDD_PATH)  $(LIBEXPAT_PATH) $(LIBEXPAT_LINK) 


its_reach_SOURCES = main.cpp 

its_reach_LDADD = $(LDADDFLAGS)  
its_reach_LDFLAGS = -export-dynamic

gal_test_SOURCES = test-gal.cpp 
# philo.hh philo.cpp

gal_test_LDADD = $(LDADDFLAGS)  
gal_test_LDFLAGS = -export-dynamic
