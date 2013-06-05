noinst_PROGRAMS = philoITS tokenITS 
# tokenITS2
# ringIPN robinIPN cliservIPN simpleIPN hanoiIPN kanbanIPN
lib_LTLIBRARIES = libtokens.la libphiloSumo.la

IPNLIBPATH= $(top_builddir)/lib/libITS.a

if WITH_LIBDDD_PATH
LIBDDD_CPP=-I$(LIBDDD_ROOT)
LIBDDD_PATH=$(LIBDDD_ROOT)/libDDD.a
else
LIBDDD_LINK=-lddd
endif

if WITH_ANTLRC_PATH
ANTLRC_CPP=-I$(ANTLRC_ROOT)/include
ANTLRC_PATH=$(ANTLRC_ROOT)/lib/libantlr3c.a
else
ANTLRC_LINK=-lantlr3c
endif


AM_CPPFLAGS = $(LIBDDD_CPP)  $(ANTLRC_CPP) -I $(top_srcdir)/src/ -g -O2 -Wall
AM_CXXFLAGS = -g -O2 -Wall

LDADDFLAGS = $(IPNLIBPATH)  $(LIBDDD_PATH) $(ANTLRC_PATH) $(ANTLRC_LINK)

philoITS_SOURCES = philo.cpp
philoITS_LDADD = $(LDADDFLAGS)  


tokenITS_SOURCES = token.cpp
tokenITS_LDADD = $(LDADDFLAGS)  


#tokenITS2_SOURCES = token2.cpp
#tokenITS2_LDADD = $(LDADDFLAGS)  


libtokens_la_SOURCES = token2.cpp 
libtokens_la_LDFLAGS = -module  
# libtokens_la_LDADD = $(LDADDFLAGS)  

libphiloSumo_la_SOURCES = philoSumo.cpp 
libphiloSumo_la_LDFLAGS = -module  


#
#ringIPN_SOURCES = ring.cpp
#ringIPN_LDADD = $(LDADDFLAGS) 
#
#
#robinIPN_SOURCES = robin.cpp
#robinIPN_LDADD = $(LDADDFLAGS) 
#
#cliservIPN_SOURCES = Cliserv.cpp
#cliservIPN_LDADD = $(LDADDFLAGS) 
#
#simpleIPN_SOURCES = simpleSym.cpp
#simpleIPN_LDADD = $(LDADDFLAGS) 
#
#hanoiIPN_SOURCES = hanoi.cpp
#hanoiIPN_LDADD = $(LDADDFLAGS) 
#
#kanbanIPN_SOURCES = kanban.cpp
#kanbanIPN_LDADD = $(LDADDFLAGS) 
#
