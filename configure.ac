AC_PREREQ(2.63)

AC_INIT([libgovf],
        [0.1],
        [https://github.com/kalev/libgovf/issues],
        [libgovf])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([govf/govf.h])

AM_INIT_AUTOMAKE([1.11 tar-ustar dist-xz no-dist-gzip foreign])
AM_SILENT_RULES([yes])

AM_INIT_AUTOMAKE
AC_PROG_LIBTOOL
AC_PROG_CC

GNOME_COMPILE_WARNINGS([maximum])

GLIB_TESTS

PKG_CHECK_MODULES(LIBGOVF, [
	gobject-2.0
	glib-2.0
	gio-2.0
	libarchive
	libxml-2.0
])

GOBJECT_INTROSPECTION_CHECK([0.9.8])

AC_ARG_ENABLE([vala],
              AS_HELP_STRING([--enable-vala], [enable Vala binding generation]),
              [], [enable_vala=check])
if test "x$enable_introspection" = "xyes" ; then
        if test "x$enable_vala" != "xno" ; then
                AC_PATH_PROG(VAPIGEN, vapigen, no)
                if test "x$VAPIGEN" == "xno"; then
                        if test "x$enable_vala" == "xcheck" ; then
                                enable_vala=no
                        else
                                AC_MSG_ERROR([Cannot find the "vapigen" binary in your PATH])
                        fi
                fi
                AC_SUBST(VAPIGEN)
        fi
        if test "x$enable_vala" != "xno" ; then
                AC_MSG_CHECKING([$VAPIGEN support for --metadatadir])
                $VAPIGEN --help | grep metadatadir 1>/dev/null 2>&1
                if test $? != 0 ; then
                        if test "x$enable_vala" == "xcheck" ; then
                                enable_vala=no
                                AC_MSG_RESULT([no])
                        else
                                AC_MSG_ERROR(["vapigen" binary in your PATH is too old (< 0.13)])
                        fi
                else
                        AC_MSG_RESULT([no])
                fi
        fi
        if test "x$enable_vala" = "xcheck" ; then
                enable_vala=yes
        fi
fi
AM_CONDITIONAL([WITH_VALA], [test "x$enable_vala" = "xyes"])

AC_CONFIG_FILES([
Makefile
govf/Makefile
libgovf.pc
tests/Makefile
])

AC_OUTPUT
