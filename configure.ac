AC_PREREQ([2.19])
AC_INIT([ctlits],
        [0.1],
        [Yann.Thierry-Mieg@lip6.fr])

# Automake initialisation
AM_INIT_AUTOMAKE([1.10 -Wall foreign])

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


AC_ARG_ENABLE([mingw-native],
				[AC_HELP_STRING([--enable-mingw-native],[build native win32 binaries for redistribution])],
				[ case "${enable_mingw_native}" in
					 yes) CXXFLAGS="-DPSAPI_VERSION=1  $CXXFLAGS"
					 LDFLAGS="-all-static -static-libgcc -Wl,-static $LDFLAGS"
					 LIBS="-lPSAPI $LIBS"
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


# M4 macros location
AC_CONFIG_MACRO_DIR([m4])

# programs used to build
# we make static libs (use ranlib)
# we make shared libs (use libtool)
LT_INIT([disable-shared])

AC_LIBTOOL_DLOPEN



# If the user didn't supplied a CFLAGS value,
# set an empty one to prevent autoconf to stick -O2 -g here.
test -z "$CFLAGS" && CFLAGS=
test -z "$CXXFLAGS" && CXXFLAGS=

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
		  src/mc/Makefile
                ])

AC_OUTPUT
