lib_LIBRARIES     = libctl.a

libctl_a_CPPFLAGS = -I$(top_srcdir) -I/usr/local/env/gcc-4.3.2/include

libctl_a_CXXFLAGS  = -g -O0

libctl_a_SOURCES  = $(srcdir)/ctlLexer.cc  \
                    $(srcdir)/ctlParser.cc \
                    $(srcdir)/makers.cc    \
                    $(srcdir)/parser.cc

$(srcdir)/ctlParser.cc:$(srcdir)/ctlLexer.cc

$(srcdir)/ctlLexer.cc: $(srcdir)/ctl.g
	java -cp /Users/hal/lip6/software/ctl/antlr/antlr-3.1.3.jar org.antlr.Tool \
        -o $(srcdir) $(srcdir)/ctl.g                                        ;\
  mv $(srcdir)/ctlLexer.c $(srcdir)/ctlLexer.cc                             ;\
  mv $(srcdir)/ctlParser.c $(srcdir)/ctlParser.cc 
  
CLEANFILES=\
  $(srcdir)/ctlLexer.cc  \
  $(srcdir)/ctlParser.cc \
  $(srcdir)/ctlLexer.h   \
  $(srcdir)/ctlParser.h  \
  $(srcdir)/ctl.tokens
