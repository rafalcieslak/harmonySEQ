#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([harmonySEQ], [0.10], [https://bugs.launchpad.net/harmonyseq])
AC_CONFIG_SRCDIR([SequencerGUI.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([harmonySEQ], [0.10])
AM_GNU_GETTEXT_VERSION([0.18.1])
AM_GNU_GETTEXT([external])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP

AC_CHECK_LIB([asound], [snd_seq_open], [] , [
					echo "Alsa library missing!"
					exit -1 ])

# Checks for header files.
AC_CHECK_HEADERS([libintl.h locale.h])


PKG_CHECK_MODULES([GTKMM], [gtkmm-2.4])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([select])

AC_CONFIG_FILES([Makefile po/Makefile.in examples/Makefile])
AC_OUTPUT
