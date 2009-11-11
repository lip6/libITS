AC_PREREQ([2.62])
AC_INIT([ctl],
        [0.7],
        [alexandre.hamez@lip6.fr])

# Automake initialisation
AM_INIT_AUTOMAKE([1.10 -Wall foreign])

# If the user didn't supplied a CFLAGS value,
# set an empty one to prevent autoconf to stick -O2 -g here.
test -z "$CFLAGS" && CFLAGS=
test -z "$CXXFLAGS" && CXXFLAGS=

AC_PROG_RANLIB

# Compile in C++
AC_PROG_CXX
AC_LANG(C++)

# Configure makefiles
AC_CONFIG_FILES([ Makefile \
                  ctl/Makefile \
                  ctl/parser/Makefile \
                  ctl/tools/Makefile \
                  test/Makefile
                ])

AC_OUTPUT
