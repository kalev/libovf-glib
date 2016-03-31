SUBDIRS = libovf-glib tests

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libovf-glib-0.1.pc

EXTRA_DIST=libovf-glib-0.1.pc.in

DISTCHECK_CONFIGURE_FLAGS = --enable-introspection

EXTRA_DIST =			\
	glib-tap.mk		\
	tap-driver.sh		\
	tap-test

-include $(top_srcdir)/git.mk
