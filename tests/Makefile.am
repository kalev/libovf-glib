include $(top_srcdir)/glib-tap.mk

LDADD = $(top_builddir)/libovf-glib/libovf-glib-0.1.la

AM_CPPFLAGS =							\
	-I$(top_srcdir)						\
	$(LIBOVF_GLIB_CFLAGS)					\
	$(NULL)

AM_CFLAGS = -g $(WARN_CFLAGS)

test_programs =	\
	parser			\
	$(NULL)

-include $(top_srcdir)/git.mk
