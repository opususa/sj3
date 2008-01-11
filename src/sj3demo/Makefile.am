if BUILDABLE_SJ3DEMO

INCLUDES = \
	-I$(top_srcdir)/include/sj3compat \
	-I$(top_srcdir)/include/sj3common \
	-I$(top_srcdir)/include/sj3lib \
	-I$(top_srcdir)/include/sj3rkcv

LDADD = \
	$(top_srcdir)/lib/sj3lib/libsj3lib.la \
	$(top_srcdir)/lib/sj3rkcv/libsj3rkcv.la \
	$(top_srcdir)/lib/sj3compat/libsj3compat.la

AM_CPPFLAGS= -DORG

sj3demo_SOURCES = sj3demo.c

bin_PROGRAMS = sj3demo

endif
