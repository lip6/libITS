# this section defines the rules to build .cpp from .g with ANTLR
SUFFIXES = .c .cpp

EXTRA_DIST = exprParser.g

exprParserLexer.c : exprParserParser.c

exprParserParser.c : exprParser.g
	java -jar $(ANTLRJAR_PATH) $<

.c.cpp:
	cp $*.c $*.cpp

MYGALGRAMMARSRCDIR= .

MYANTLRHEADER = $(MYGALGRAMMARSRCDIR)/exprParserLexer.h \
		$(MYGALGRAMMARSRCDIR)/exprParserParser.h

MYANTLRSOURCES = $(MYGALGRAMMARSRCDIR)/exprParserLexer.cpp \
		$(MYGALGRAMMARSRCDIR)/exprParserParser.cpp \
		 $(MYGALGRAMMARSRCDIR)/exprParserLexer.c \
		$(MYGALGRAMMARSRCDIR)/exprParserParser.c


# clean ANTLR-generated files
CLEANFILES = $(MYANTLRSOURCES) $(MYANTLRHEADER) 

all: exprParserLexer.cpp exprParserParser.cpp
