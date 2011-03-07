lib_LIBRARIES =  libctlits.a 
noinst_PROGRAMS = its-ctl

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


if WITH_LIBITS_PATH
LIBITS_CPP=-I$(LIBITS_ROOT) 
LIBITS_PATH=$(LIBITS_ROOT)/../lib/libITS.a 
else
LIBITS_LINK=-lITS
endif

AM_CPPFLAGS =  -Wall -g -O2 -I $(top_srcdir)/src/ctlp/ $(HAVE_BOOST_FLAG) $(LIBITS_CPP) $(LIBDDD_CPP) $(LIBEXPAT_CPP)
#
#libmcctl_a_SOURCES = mcCmd.c  mcCover.c  mcDbg.c  mcDnC.c  mcGFP.c  mc.h  mcInt.h  mcMc.c  mcSCC.c  mcUtil.c  mcVacuum.c

libctlits_a_SOURCES = ctlCheck.hh ctlCheck.cpp

its_ctl_SOURCES = test_ctl.cpp
its_ctl_LDADD =  $(top_srcdir)/src/ctlp/libctlp.a $(top_srcdir)/src/mc/libctlits.a   $(LIBITS_PATH)   $(LIBDDD_PATH) $(LIBEXPAT_PATH) $(LIBEXPAT_LINK) -lm

$(top_srcdir)/src/ctlp/libctlp.a :
	cd ../ctlp ; make ;

