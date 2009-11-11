CSRC += ctlpCmd.c ctlpUtil.c
HEADERS += ctlp.h ctlpInt.h
LEXSRC += ctlp.l
YACCSRC += ctlp.y
GENERATEDCSRC += ctlpRead.c

$(objectdir)/ctlpRead.c: ctlp.y ctlp.h ctlpInt.h ctlpLex.c
	$(YACC) -pCtlpYy -t -o $(objectdir)/ctlpRead.c $<
	-@chmod 0664 $(objectdir)/ctlpRead.c

ctlpLex.c: ctlp.l
	$(LEX) -PCtlpYy -o$(objectdir)/ctlpLex.c $< 
	-@chmod 0664 $(objectdir)/ctlpLex.c

$(objectdir)/ctlpRead.o: $(objectdir)/ctlpRead.c

DEPENDENCYFILES = $(CSRC)
