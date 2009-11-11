lib_LIBRARIES = libEXPRESS.a

AM_YFLAGS = -d 

AM_LFLAGS = -olex.yy.c

if WITH_LIBDDD_PATH
LIBDDD_CPP=-I$(LIBDDD_ROOT)
endif

libEXPRESS_a_CPPFLAGS = -I$(top_srcdir)/src/its/src/parser_RdPE \
                        $(LIBDDD_CPP) 

BUILT_SOURCES = expression_yacc.h

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
