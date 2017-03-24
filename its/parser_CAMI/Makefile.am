AM_YFLAGS = -d 
AM_LFLAGS = -olex.yy.c

AM_CPPFLAGS =   -I$(top_srcdir)/src/parser_RdPE

BUILT_SOURCES = Cami_yacc.h

# a small hack for automake 1.12 that builds a .hxx from .yxx
# to be removed when compatibility with automake 1.11 will be dropped
SUFFIXES = .h .hxx
.hxx.h:
	cp $< $@

lib_LIBRARIES = libCAMI.a

libCAMI_a_SOURCES   =   Cami.cpp \
                        Cami.hpp \
                        Cami_lex.ll \
                        Cami_yacc.yxx
