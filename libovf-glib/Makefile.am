lib_LTLIBRARIES = libovf-glib-0.1.la

AM_CPPFLAGS =						\
	-I$(top_srcdir)					\
	-I$(top_builddir)				\
	-I$(srcdir)					\
	$(LIBOVF_GLIB_CFLAGS)				\
	$(WARN_CFLAGS)

libovf_glib_0_1_la_LDFLAGS =				\
	-export-dynamic					\
	-no-undefined

libovf_glib_0_1_la_LIBADD = $(LIBOVF_GLIB_LIBS)

H_FILES =					\
	ovf.h					\
	ovf-disk.h				\
	ovf-package.h

C_FILES =					\
	ovf-disk.c				\
	ovf-package.c

libovf_glib_0_1_la_SOURCES = 	\
	$(H_FILES)		\
	$(C_FILES)

# introspection
-include $(INTROSPECTION_MAKEFILE)

if HAVE_INTROSPECTION
INTROSPECTION_GIRS = Ovf-0.1.gir
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

introspection_sources = $(libovf_glib_0_1_la_SOURCES)

Ovf-0.1.gir: libovf-glib-0.1.la
Ovf_0_1_gir_INCLUDES = GObject-2.0 GLib-2.0 Gio-2.0
Ovf_0_1_gir_CFLAGS = $(AM_CPPFLAGS)
Ovf_0_1_gir_SCANNERFLAGS = --warn-all --pkg-export libovf-glib-0.1 --c-include "libovf-glib/ovf.h"
Ovf_0_1_gir_NAMESPACE = Ovf
Ovf_0_1_gir_VERSION = 1.0
Ovf_0_1_gir_LIBS = libovf-glib-0.1.la
Ovf_0_1_gir_FILES = $(introspection_sources)

girdir = $(datadir)/gir-1.0
dist_gir_DATA = Ovf-0.1.gir

typelibdir = $(libdir)/girepository-1.0/
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES = $(dist_gir_DATA) $(typelib_DATA)
endif # HAVE_INTROSPECTION

-include $(top_srcdir)/git.mk
