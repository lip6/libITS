# this section defines the rules to build .cpp from .g with ANTLR
SUFFIXES = .c .cpp

exprParserLexer.c exprParserParser.c : exprParser.g
	/usr/bin/java -jar $(ANTLRJAR_PATH) $<
	
.c.cpp:
	cp $*.c $*.cpp

MYANTLRHEADER = $(MYGALGRAMMARSRCDIR)/exprParserLexer.h \
		$(MYGALGRAMMARSRCDIR)/exprParserParser.h

MYANTLRSOURCES = $(MYGALGRAMMARSRCDIR)/exprParserLexer.cpp \
		$(MYGALGRAMMARSRCDIR)/exprParserParser.cpp

# clean ANTLR-generated files
CLEANFILES = $MYANTLRSOURCES $MYANTLRHEADER

all: exprParserLexer.cpp exprParserParser.cpp
