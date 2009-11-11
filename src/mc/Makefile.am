lib_LIBRARIES =  libctlits.a # libmcctl.a
noinst_PROGRAMS = test_ctl

ITSPATH = /data/thierry/DDD/libddd/romeo/trunk/src/its
LIBDDDPATH = /data/thierry/DDD/libddd/libddd/trunk/src
GLULIBPATH = /home/ythierry/local/lib


AM_CPPFLAGS = -DBDDcu -Wall -g -O2 -I $(top_srcdir)/vis/ctlp/ -I  $(LIBDDDPATH) -I /home/ythierry/local/include -I $(ITSPATH)/src
#
#libmcctl_a_SOURCES = mcCmd.c  mcCover.c  mcDbg.c  mcDnC.c  mcGFP.c  mc.h  mcInt.h  mcMc.c  mcSCC.c  mcUtil.c  mcVacuum.c

libctlits_a_SOURCES = ctlCheck.hh ctlCheck.cpp


test_ctl_SOURCES = test_ctl.cpp
test_ctl_LDADD =  $(top_srcdir)/vis/ctlp/libctlp.a $(top_srcdir)/vis/mc/libctlits.a $(GLULIBPATH)/libcu.a  $(GLULIBPATH)/libglu.a  $(ITSPATH)/lib/libITS.a $(ITSPATH)/lib/libRdP.a $(ITSPATH)/lib/libCAMI.a -lm $(LIBDDDPATH)/libDDD.a 
