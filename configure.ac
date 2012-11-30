#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([frontier], [0.9.1], [jkawahara@erato.ist.hokudai.ac.jp])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([src/frontier/frontier.cpp])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
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

AC_CONFIG_FILES([Makefile build-aux/Makefile src/frontier/Makefile src/utility/Makefile])
AC_OUTPUT
