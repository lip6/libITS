noinst_PROGRAMS = main

main_SOURCES    = main.cc

main_CPPFLAGS   = -I$(top_srcdir) -I/usr/local/env/gcc-4.3.2/include

main_CXXFLAGS   = -g -O0

main_LDADD      = $(top_builddir)/ctl/parser/libctl.a

main_LDFLAGS    = -lantlr3c

check_PROGRAMS  = main

TESTS = $(check_PROGRAMS)
