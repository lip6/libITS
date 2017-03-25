AM_YFLAGS = -d 
AM_LFLAGS = -olex.yy.c

AM_CPPFLAGS =   -I$(top_srcdir)

BUILT_SOURCES = Cami_yacc.h

# a small hack for automake 1.12 that builds a .hxx from .yxx
# to be removed when compatibility with automake 1.11 will be dropped
SUFFIXES = .h .hxx
.hxx.h:
	cp $< $@

noinst_LIBRARIES = libCAMI.a

libCAMI_a_SOURCES   =   Cami.cpp \
                        Cami.hpp \
                        Cami_lex.ll \
                        Cami_yacc.yxx
