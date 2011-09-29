 #                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([harmonySEQ], [0.16], [https://bugs.launchpad.net/harmonyseq])
AC_CONFIG_SRCDIR([src/Sequencer.h])
AC_CONFIG_HEADERS([src/config.h])
AM_INIT_AUTOMAKE([harmonySEQ], [0.16])
AM_GNU_GETTEXT_VERSION([0.18.1])
AM_GNU_GETTEXT([external])

# we do not install into /usr/local, since desktop integration wouldn't work
AC_PREFIX_DEFAULT(/usr)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP

CXXFLAGS="-O3"
CXXFLAGS="-g -O0"

osc_enabled=true

# One might want to disable OSC supprort...
AC_ARG_ENABLE(osc,
     AC_HELP_STRING([--disable-osc],
		[disables OSC (Open Sound Control protocol) support (default=enabled)]))

AS_IF([test x$enable_osc = xno],[
					osc_enabled=false
					echo "-- OSC disabled."
					])
AM_CONDITIONAL(DISABLEOSC, test x$enable_osc = xno)

AC_CHECK_LIB([asound], [snd_seq_open], [] , [
					echo "Alsa library missing - install alsa developement headers! [libasound2-dev on Ubuntu]"
					exit -1 ])

# Checks for header files.
AC_CHECK_HEADERS([libintl.h locale.h], [] , [
					echo "Gettext locale header files missing! [libintl.h locale.h]"
					exit -1 ])


PKG_CHECK_MODULES([GTKMM], [gtkmm-2.4], [] , [
					echo "Gtkmm 2.4 library missing - install gtkmm-2.4 developement headers! [gtkmm-2.4-dev on Ubuntu]"
					exit -1 ])

if test "$osc_enabled" = "true"; then
	# Need to check for liblo headers.
	AC_CHECK_HEADERS([lo/lo.h], [] , [
					echo "Liblo header files [lo/lo.h] missing! (either install LIBLO developement headers [liblo7 & liblo-dev on Ubuntu], or disable OSC support by using the --disable-osc flag)"
					exit -1 ])
fi

AC_PATH_PROG(UPDATE_MIME_DB, update-mime-database, no)
if test "x$UPDATE_MIME_DB" = "xno"; then
	AC_MSG_ERROR([You need to install update-mime-database])
fi
#AC_PATH_PROG(UPDATE_ICON_C, gtk-update-icon-cache, no)
#if test "x$UPDATE_ICON_C" = "xno"; then
#	AC_MSG_ERROR([You need to install gtk-update-icon-cache])
#fi

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_ALLOCA
AC_FUNC_ERROR_AT_LINE
AC_CHECK_FUNCS([select])

AC_CONFIG_FILES([Makefile po/Makefile.in examples/Makefile src/Makefile pixmaps/Makefile])
AC_OUTPUT
