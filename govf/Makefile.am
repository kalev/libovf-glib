lib_LTLIBRARIES = libgovf.la

CLEANFILES =

AM_CPPFLAGS =						\
	-I$(top_srcdir)					\
	-I$(top_builddir)				\
	-I$(srcdir)					\
	$(LIBGOVF_CFLAGS)				\
	$(WARN_CFLAGS)

libgovf_la_LDFLAGS =					\
	-export-dynamic					\
	-no-undefined

libgovf_la_LIBADD = $(LIBGOVF_LIBS)

H_FILES =					\
	govf.h					\
	govf-disk.h				\
	govf-package.h

C_FILES =					\
	govf-disk.c				\
	govf-package.c

libgovf_la_SOURCES = 		\
	$(H_FILES)		\
	$(C_FILES)

headerdir = $(prefix)/include/libgovf/govf
header_DATA = $(H_FILES)

# introspection
-include $(INTROSPECTION_MAKEFILE)

if HAVE_INTROSPECTION
INTROSPECTION_GIRS = Govf-1.0.gir
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

introspection_sources = $(libgovf_la_SOURCES)

Govf-1.0.gir: libgovf.la
Govf_1_0_gir_INCLUDES = GObject-2.0 GLib-2.0 Gio-2.0
Govf_1_0_gir_CFLAGS = $(AM_CPPFLAGS)
Govf_1_0_gir_SCANNERFLAGS = --warn-all --pkg-export libgovf --c-include "govf/govf.h"
Govf_1_0_gir_NAMESPACE = Govf
Govf_1_0_gir_VERSION = 1.0
Govf_1_0_gir_LIBS = libgovf.la
Govf_1_0_gir_FILES = $(introspection_sources)

girdir = $(datadir)/gir-1.0
dist_gir_DATA = Govf-1.0.gir

typelibdir = $(libdir)/girepository-1.0/
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(dist_gir_DATA) $(typelib_DATA)
endif # HAVE_INTROSPECTION

if WITH_VALA
vapidir = $(datadir)/vala/vapi
vapi_DATA = libgovf.vapi

libgovf.vapi: Govf-1.0.gir
	$(AM_V_GEN)$(VAPIGEN)			 \
		--vapidir=$(builddir)		 \
		--pkg gobject-2.0		 \
		--pkg glib-2.0			 \
		--pkg gio-2.0			 \
		--library libgovf		 \
		--metadatadir=$(srcdir)		 \
		$<

CLEANFILES += libgovf.vapi
endif

-include $(top_srcdir)/git.mk
