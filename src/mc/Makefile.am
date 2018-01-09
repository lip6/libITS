noinst_LIBRARIES =  libctlits.a 

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



AM_CPPFLAGS =  -Wall -g -O2 -I $(top_srcdir)/src $(HAVE_BOOST_FLAG) $(LIBITS_CPP) $(LIBDDD_CPP) 
#
#libmcctl_a_SOURCES = mcCmd.c  mcCover.c  mcDbg.c  mcDnC.c  mcGFP.c  mc.h  mcInt.h  mcMc.c  mcSCC.c  mcUtil.c  mcVacuum.c

libctlits_a_SOURCES = ctlCheck.hh ctlCheck.cpp operators.hh operators.cpp


# LDFLAGS used to have '-rdynamic' for DLL mode

$(top_srcdir)/src/ctlp/libctlp.a :
	cd ../ctlp ; make ;

