bin_PROGRAMS = its-ctl


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

AM_CPPFLAGS =  -Wall -g -O2 -I $(top_srcdir)/src $(HAVE_BOOST_FLAG) $(LIBITS_CPP) $(LIBDDD_CPP) $(LIBEXPAT_CPP) $(ANTLRC_CPP)

its_ctl_SOURCES = main.cpp
its_ctl_LDADD =  $(top_srcdir)/src/ctlp/libctlp.a $(top_srcdir)/src/mc/libctlits.a   $(LIBITS_PATH) $(LIBITS_LINK) $(LIBDDD_PATH) $(LIBDDD_LINK) $(LIBEXPAT_PATH) $(LIBEXPAT_LINK) $(ANTLRC_PATH) $(ANTLRC_LINK) -lm
