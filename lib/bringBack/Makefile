
all : ../libRdP.a ../libCAMI.a 


../libRdP.a : ../../its/parser_RdPE/libRdP.a
	cp ../../its/parser_RdPE/libRdP.a ../libRdP.a


../libCAMI.a : ../../its/parser_CAMI/libCAMI.a
	cp ../../its/parser_CAMI/libCAMI.a ../libCAMI.a
clean :
	rm -f ../libCAMI.a ../libRdP.a

maintainer-clean : clean

check:

distdir :

install :