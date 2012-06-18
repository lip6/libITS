

# this section defines the rules to build .cpp from .g with ANTLR
SUFFIXES = .g .cpp
.g.cpp:
	/usr/bin/java -jar $(ANTLRJAR_PATH) $< && mv $*.c $*.cpp

MYANTLRHEADER = $(MYGALGRAMMARSRCDIR)/exprLexer.h \
		$(MYGALGRAMMARSRCDIR)/exprParser.h

MYANTLRSOURCES = $(MYGALGRAMMARSRCDIR)/exprLexer.cpp \
		$(MYGALGRAMMARSRCDIR)/exprParser.cpp


CLEANFILES = $MYANTLRSOURCES $MYANTLRHEADER

all : exprParser.g
	/usr/bin/java -jar $(ANTLRJAR_PATH)  exprParser.g &&  mv exprParserParser.c exprParserParser.cpp  && mv exprParserLexer.c exprParserLexer.cpp 


