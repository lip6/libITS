BASE = its-main

all : 
	latex $(BASE).tex ; bibtex $(BASE) ; latex $(BASE) ; dvipdf $(BASE).dvi $(BASE).pdf

$(BASE).blg : $(BASE).tex $(BASE).bib 
	latex $(BASE).tex ; bibtex $(BASE) ;latex $(BASE).tex ;

$(BASE).bbl : $(BASE).aux $(BASE).bib
	bibtex $(BASE)

$(BASE).aux : $(BASE).tex
	latex $(BASE).tex

$(BASE).pdf : 
	latex $(BASE).tex ; bibtex $(BASE) ; latex $(BASE) ; dvipdf $(BASE).dvi $(BASE).pdf
	@sh -c 'if [ `uname -s` = "Darwin" ] ; then open $(BASE).pdf ; fi'

clean :
	@rm -f *.log *.aux *.out *~
	@rm -f $(BASE).pdf
