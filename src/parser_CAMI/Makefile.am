AM_YFLAGS = -d 
AM_LFLAGS = -olex.yy.c

AM_CPPFLAGS =   -I$(top_srcdir)/src/parser_RdPE

BUILT_SOURCES = Cami_yacc.h

lib_LIBRARIES = libCAMI.a

libCAMI_a_SOURCES   =   Cami.cpp \
                        Cami.hpp \
                        Cami_lex.ll \
                        Cami_yacc.yxx
