noinst_PROGRAMS = philoITS
# ringIPN robinIPN cliservIPN simpleIPN hanoiIPN kanbanIPN

IPNLIBPATH= $(top_builddir)/lib/libITS.a

if WITH_LIBDDD_PATH
LIBDDD_CPP=-I$(LIBDDD_ROOT)
LIBDDD_PATH=$(LIBDDD_ROOT)/libDDD.a
else
LIBDDD_LINK=-lddd
endif


AM_CPPFLAGS = $(LIBDDD_CPP) -I $(top_srcdir)/src/ -g -O2 -Wall
AM_CXXFLAGS = -g -O2 -Wall

LDADDFLAGS = $(IPNLIBPATH)  $(LIBDDD_PATH)

philoITS_SOURCES = philo.cpp
philoITS_LDADD = $(LDADDFLAGS)  

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
