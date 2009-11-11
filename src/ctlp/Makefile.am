lib_LIBRARIES = libctlp.a
noinst_PROGRAMS = test_ctl

AM_YFLAGS = -pCtlpYy -d -t 
AM_LFLAGS = -PCtlpYy -o lex.yy.c
AM_CPPFLAGS = -DBDDcu -Wall -g -O2 -I  $(libddd_prefix)/src -I /home/ythierry/local/include 
BUILT_SOURCES =  ctlp_yacc.h

GLULIBPATH = /home/ythierry/local/lib

libctlp_a_SOURCES = ctlpCmd.c  ctlp.h  ctlpInt.h  ctlp_flex.l ctlpUtil.c  ctlp_yacc.y visfd.h visfd.c

test_ctl_SOURCES = test_ctl.c
test_ctl_LDADD =  libctlp.a $(GLULIBPATH)/libcu.a  $(GLULIBPATH)/libglu.a -lm 