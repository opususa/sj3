if BUILDABLE_SJ3CLIENT

INCLUDES = \
	-I$(top_srcdir)/include/sj3compat \
	-I$(top_srcdir)/include/sj3common \
	-I$(top_srcdir)/include/sj3lib

LDADD = \
	$(top_builddir)/lib/sj3lib/libsj3lib.la \
	$(top_builddir)/lib/sj3compat/libsj3compat.la

sj3stat_SOURCES = sj3stat.c

bin_PROGRAMS = sj3stat

endif
