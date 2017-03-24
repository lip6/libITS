lib_LIBRARIES = libEXPRESS.a

AM_YFLAGS = -d 
AM_LFLAGS = -olex.yy.c

if WITH_LIBDDD_PATH
LIBDDD_CPP=-I$(LIBDDD_ROOT)/include
endif

libEXPRESS_a_CPPFLAGS = -I$(top_srcdir) \
                        $(LIBDDD_CPP) 

BUILT_SOURCES = expression_yacc.h

# a small hack for automake 1.12 that builds a .hxx from .yxx
# to be removed when compatibility with automake 1.11 will be dropped
SUFFIXES = .h .hxx
.hxx.h:
	cp $< $@

LYFILES =   expression_lex.ll \
            expression_yacc.yxx \
            Moteur_expression.hpp

libEXPRESS_a_SOURCES    =   $(LYFILES) \
                            Word.cpp \
                            ../parser_RdPE/RdPE.h \
                            expression.hpp \
                            strong.hpp \
                            Word.hpp \
                            var_set.hpp \
                            expression.cpp \
                            strong.cpp
