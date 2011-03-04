AC_PREREQ([2.19])
AC_INIT([ctlits],
        [0.1],
        [Yann.Thierry-Mieg@lip6.fr])

# Automake initialisation
AM_INIT_AUTOMAKE([1.10 -Wall foreign])

# If the user didn't supplied a CFLAGS value,
# set an empty one to prevent autoconf to stick -O2 -g here.
test -z "$CFLAGS" && CFLAGS=
test -z "$CXXFLAGS" && CXXFLAGS=

# option to indicate libddd folder
AC_ARG_WITH([libddd],
            [AC_HELP_STRING([--with-libddd=/root/of/libddd],
                            [specify the path of libddd])])

AM_CONDITIONAL([WITH_LIBDDD_PATH], [test "x${with_libddd}" != x])
if test "x${with_libddd}" != x; then
    AC_SUBST([LIBDDD_ROOT],[${with_libddd}])
fi


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


AC_PROG_RANLIB

# Compile in C++
AC_PROG_CXX
AC_LANG(C++)

# compile in C
AC_PROG_CC
# use yacc to create parsers
AM_PROG_LEX
AC_PROG_YACC

# Enable boost if found:
AX_CHECK_BOOST([1.31], [103100])
AM_CONDITIONAL([HAVE_BOOST], [test "${BOOST_DETECTED}" = "true"])

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
