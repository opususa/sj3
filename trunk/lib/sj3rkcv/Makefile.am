if BUILDABLE_SJ3DEMO

EXTRA_DIST = sjrk

INCLUDES = \
	-I$(top_srcdir)/include/sj3compat \
	-I$(top_srcdir)/include/sj3comon \
	-I$(top_srcdir)/include/sj3rkcv

noinst_LTLIBRARIES = libsj3rkcv.la

libsj3rkcv_la_SOURCES = \
	rk_conv.c \
	sj3_rkcv.c \
	wc16_str.c

rkdir = $(SJ3SYSCONFDIR)

rk_DATA = sjrk

endif
