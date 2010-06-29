
all : ../libRdP.a ../libCAMI.a 


../libRdP.a : ../../src/parser_RdPE/libRdP.a
	cp ../../src/parser_RdPE/libRdP.a ../libRdP.a


../libCAMI.a : ../../src/parser_CAMI/libCAMI.a
	cp ../../src/parser_CAMI/libCAMI.a ../libCAMI.a
clean :
	rm -f ../libCAMI.a ../libRdP.a

maintainer-clean : clean

