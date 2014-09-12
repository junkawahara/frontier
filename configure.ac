#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

#AC_PREREQ([2.68])
AC_INIT([frontier], [1.0.0], [jkawahara@erato.ist.hokudai.ac.jp])
AC_CONFIG_AUX_DIR([build-aux])
#AM_PROG_AR
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([src/frontier/frontier.cpp])
AC_CONFIG_HEADER([config.h])

# Checks for programs.

cxxflags_save="$CXXFLAGS"
AC_PROG_CXX
CXXFLAGS="$cxxflags_save"
test -n "$CXXFLAGS" || CXXFLAGS=""

cflags_save="$CFLAGS"
AC_PROG_CC
CFLAGS="$cflags_save"
test -n "$CFLAGS" || CFLAGS=""

AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC

AC_ARG_ENABLE(32bit,
  [AS_HELP_STRING([--enable-32bit],[build 32bit version])])
AM_CONDITIONAL([ENABLE_32BIT], [test "x$enable_32bit" = "xyes"])

AC_ARG_ENABLE(debug,
  [AS_HELP_STRING([--enable-debug],[build debug version])])
AM_CONDITIONAL([ENABLE_DEBUG], [test "x$enable_debug" = "xyes"])


AC_CHECK_LIB([gmp], [main], [], [])
AC_CHECK_LIB([gmpxx], [main], [], [])
AM_CONDITIONAL([HAVE_LIBGMPXX], [test "x$ac_cv_lib_gmpxx_main" == "xyes"])

AC_CONFIG_FILES([Makefile build-aux/Makefile src/frontier/Makefile src/utility/Makefile])
#AC_CONFIG_FILES([Makefile src/frontier/Makefile src/utility/Makefile])

AC_OUTPUT
