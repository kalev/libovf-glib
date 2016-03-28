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
	ovf-parser.h

C_FILES =					\
	ovf-parser.c

libovf_glib_0_1_la_SOURCES = 	\
	$(H_FILES)		\
	$(C_FILES)

-include $(top_srcdir)/git.mk
