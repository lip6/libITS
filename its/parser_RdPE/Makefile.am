AM_YFLAGS = -d 
AM_LFLAGS = -olex.yy.c
AM_CPPFLAGS = -O3 -I$(abs_top_srcdir)

BUILT_SOURCES = RdPMonteur_yacc.h

# a small hack for automake 1.12 that builds a .hxx from .yxx
# to be removed when compatibility with automake 1.11 will be dropped
SUFFIXES = .h .hxx
.hxx.h:
	cp $< $@

noinst_LIBRARIES = libRdP.a

libRdP_a_SOURCES = RdPE.cpp  RdPE.h  RdPMonteur.h  RdPMonteur_lex.ll  RdPMonteur_yacc.yxx

