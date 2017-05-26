AC_PREREQ([2.19])
AC_INIT([itsctl],
        [0.1],
        [Yann.Thierry-Mieg@lip6.fr])

# Automake initialisation
AM_INIT_AUTOMAKE([1.10 -Wall foreign])


# link time opt, set AR and RANLIB before LT_INIT call, unless configure overrides them.
# also Options for disabling LTO on Darwin since binutils sucks big time on that platform.
nolto=false

AC_ARG_ENABLE(	[nolto],
        [AC_HELP_STRING([--enable-nolto],[avoid using LTO flags, mostly for Darwin])],
        [  case "${enable_nolto}" in
            yes) nolto=true
                ;;
            no)
                ;;
            *)  AC_MSG_ERROR(Bad value ${enableval})
                ;;
            esac])

if test "x$nolto" = "xfalse"; then
   CXXFLAGS="-flto $CXXFLAGS"
   test -z "$AR" && AR=gcc-ar
   test -z "$RANLIB" && RANLIB=gcc-ranlib
fi



# option to indicate libddd folder
AC_ARG_WITH([libddd],
            [AC_HELP_STRING([--with-libddd=/root/of/libddd],
                            [specify the path of libddd])])

AM_CONDITIONAL([WITH_LIBDDD_PATH], [test "x${with_libddd}" != x])
if test "x${with_libddd}" != x; then
    AC_SUBST([LIBDDD_ROOT],[${with_libddd}])
fi


AC_CHECK_LIB(tcmalloc, malloc, ,AC_MSG_WARN(google perftool not detected : not using tcmalloc))

AC_CHECK_LIB(dl, dlopen, ,AC_MSG_WARN(No operational dynamic link libdl found. This is a real problem.))



#Options for disabling google hash
stdhash=false

AC_ARG_ENABLE(	[stdhash],
        [AC_HELP_STRING([--enable-stdhash],[revert std lib hash map(default uses google sparse hash)])],
        [  case "${enable_stdhash}" in
            yes) stdhash=true
                ;;
            no)
                ;;
            *)  AC_MSG_ERROR(Bad value ${enableval})
                ;;
            esac])

if test "x$stdhash" = "xtrue"; then
    AC_DEFINE([USE_STD_HASH],1,[Define if you want to use std hash instead of google sparse hash.])
fi

CPPFLAGS="-Wno-unused-local-typedefs $CPPFLAGS"
LDFLAGS="-all-static -static-libgcc -static-libstdc++ $LDFLAGS"

AC_ARG_ENABLE([mingw-native],
				[AC_HELP_STRING([--enable-mingw-native],[build native win32 binaries for redistribution])],
				[ case "${enable_mingw_native}" in
					 yes) CXXFLAGS="-DPSAPI_VERSION=1  $CXXFLAGS"
					 LDFLAGS="-Wl,-static,--stack,16777216 $LDFLAGS"
					 LIBS="-lPSAPI -lstdc++ -lpthread $LIBS"
					 ;;
					 no)
					 ;;
					 *) AC_MSG_ERROR(Bad value ${enableval})
					 ;;
				esac])



# M4 macros location
AC_CONFIG_MACRO_DIR([m4])


# option to indicate libits folder
AC_ARG_WITH([libits],
            [AC_HELP_STRING([--with-libits=/root/of/libits],
                            [specify the path of libits])])


AM_CONDITIONAL([WITH_LIBITS_PATH], [test "x${with_libits}" != x])
if test "x${with_libits}" != x; then
    AC_SUBST([LIBITS_ROOT],[${with_libits}])
fi

# option to indicate libexpat folder
AC_ARG_WITH([libexpat],
            [AC_HELP_STRING([--with-libexpat=/root/of/libexpat],
                            [specify the path of libexpat])])


AM_CONDITIONAL([WITH_LIBEXPAT_PATH], [test "x${with_libexpat}" != x])
if test "x${with_libexpat}" != x; then
    AC_SUBST([LIBEXPAT_ROOT],[${with_libexpat}])
fi

# option to indicate the path to the antlr C runtime
AC_ARG_WITH([antlrc],
            [AC_HELP_STRING([--with-antlrc=/root/of/antlrc],
                            [specify the path of antlr C runtime])])

AM_CONDITIONAL([WITH_ANTLRC_PATH], [test "x${with_antlrc}" != x])
if test "x${with_antlrc}" != x; then
    AC_SUBST([ANTLRC_ROOT],[${with_antlrc}])
fi



# M4 macros location
AC_CONFIG_MACRO_DIR([m4])

# programs used to build
# we make static libs (use ranlib)
# we make shared libs (use libtool)
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
LT_INIT([disable-shared])

AC_LIBTOOL_DLOPEN



# If the user didn't supplied a CFLAGS value,
# set an empty one to prevent autoconf to stick -O2 -g here.
test -z "$CFLAGS" && CFLAGS=
test -z "$CXXFLAGS" && CXXFLAGS=

# we compile C++11
CXXFLAGS="-std=c++11 $CXXFLAGS"

# compile in C
AC_PROG_CC
# Compile in C++
AC_PROG_CXX
AC_LANG(C++)

# use yacc to create parsers
AM_PROG_LEX
AC_PROG_YACC

# define HAS_STDC_HEADERS
AC_HEADER_STDC

AC_CHECK_TOOL([STRIP],[strip])


# Configure makefiles
AC_CONFIG_FILES([ Makefile \
		  src/Makefile \
		  src/ctlp/Makefile \
		  src/mc/Makefile \
		  bin/Makefile
                ])

AC_OUTPUT
