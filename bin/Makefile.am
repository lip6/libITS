bin_PROGRAMS = its-reach
noinst_PROGRAMS = dve2gal gal2c
#gal-test

LIBITS_CPP=-I$(top_srcdir)
LIBITS_PATH=$(top_srcdir)/lib/libITS.a 


if WITH_LIBDDD_PATH
LIBDDD_CPP=-I$(LIBDDD_ROOT)/include
LIBDDD_PATH=$(LIBDDD_ROOT)/lib/libDDD.a
else
LIBDDD_LINK=-lDDD
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


if WITH_GMP_PATH
GMP_CPP=-I$(GMP_ROOT)/include
GMP_PATH=$(GMP_ROOT)/lib/libgmpxx.a $(GMP_ROOT)/lib/libgmp.a 
else
GMP_LINK=-lgmpxx -lgmp
endif


AM_CPPFLAGS = $(LIBDDD_CPP) $(LIBITS_CPP) $(LIBEXPAT_CPP) $(ANTLRC_CPP) $(GMP_CPP)
AM_CXXFLAGS = -g -O3 -Wall 

LDADDFLAGS = $(LIBITS_PATH)  $(LIBDDD_PATH)  $(LIBDDD_LINK) $(LIBEXPAT_PATH) $(LIBEXPAT_LINK) $(ANTLRC_PATH) $(ANTLRC_LINK) $(GMP_PATH) $(GMP_LINK)
AM_LDFLAGS = $(LDFLAGS) -fwhole-program 
AM_LDFLAGS += $(STATICFLAGS)

its_reach_SOURCES = main.cpp EarlyBreakObserver.hh MaxComputer.hh MaxComputer.cpp SMTExporter.hh SMTExporter.cpp ExactStateCounter.cpp ExactStateCounter.hh ToTransRel.hh ToTransRel.cpp Graph.hh Graph.cpp

its_reach_LDADD = $(LDADDFLAGS) 

dve2gal_SOURCES = dve2galmain.cpp

dve2gal_LDADD = $(LDADDFLAGS)  

gal2c_SOURCES = gal2cmain.cpp

gal2c_LDADD = $(LDADDFLAGS)

#gal_test_SOURCES = test-gal.cpp 
# philo.hh philo.cpp

#gal_test_LDADD = $(LDADDFLAGS)  
#gal_test_LDFLAGS = -export-dynamic
