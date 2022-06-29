noinst_LIBRARIES = libctlp.a
noinst_PROGRAMS = test_ctl

AM_YFLAGS = -pCtlpYy -d -t 
AM_LFLAGS = -PCtlpYy -o lex.yy.c
AM_CPPFLAGS = -Wall -g -O2 
BUILT_SOURCES =  ctlp_yacc.h

UTILSOURCES = util.h  util/cpu_stats.c util/cpu_time.c util/datalimit.c util/getopt.c util/pathsearch.c util/prtime.c util/qsort.c util/random.c util/safe_mem.c util/strsav.c util/texpand.c util/tmpfile.c

ARRAYSOURCES = array.h array/array.c

STSOURCES = st.h st/st.c

libctlp_a_SOURCES = ctlpCmd.c \
	ctlp.h  \
	ctlpInt.h  \
	ctlp_flex.l \
	ctlpUtil.c  \
	ctlp_yacc.y \
	visfd.h \
	visfd.c \
	$(UTILSOURCES) $(ARRAYSOURCES) $(STSOURCES)


test_ctl_SOURCES = test_ctl.c
test_ctl_LDADD =  libctlp.a -lm 
