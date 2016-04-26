SUBDIRS = govf tests

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libgovf.pc

EXTRA_DIST=libgovf.pc.in

DISTCHECK_CONFIGURE_FLAGS = --enable-introspection

EXTRA_DIST =			\
	glib-tap.mk		\
	tap-driver.sh		\
	tap-test

-include $(top_srcdir)/git.mk
