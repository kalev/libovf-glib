AC_PREREQ(2.63)

AC_INIT([libovf-glib],
        [0.1],
        [https://github.com/kalev/libovf-glib/issues],
        [libovf-glib])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([libovf-glib/ovf.h])

AM_INIT_AUTOMAKE([1.11 tar-ustar dist-xz no-dist-gzip foreign])
AM_SILENT_RULES([yes])

AM_INIT_AUTOMAKE
AC_PROG_LIBTOOL
AC_PROG_CC

GNOME_COMPILE_WARNINGS([maximum])

GLIB_TESTS

PKG_CHECK_MODULES(LIBOVF_GLIB, [
	gobject-2.0
	glib-2.0
	gio-2.0
	libarchive
	libxml-2.0
])

AC_CONFIG_FILES([
Makefile
libovf-glib/Makefile
tests/Makefile
])

AC_OUTPUT
