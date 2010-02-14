if BUILDABLE_SJ3CLIENT

INCLUDES = \
	-I$(top_srcdir)/include/sj3compat \
	-I$(top_srcdir)/include/sj3common \
	-I$(top_srcdir)/include/sj3lib

lib_LTLIBRARIES = libsj3lib.la

libsj3lib_la_LDFLAGS = $(top_builddir)/lib/sj3compat/libsj3compat.la -version-info @LIBSJ3LIB_VERSION_INFO@

libsj3lib_la_SOURCES = \
	level1.c \
	sj.c \
	sj_string.c

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = sj3lib.pc

EXTRA_DIST = sj3lib.pc.in

endif
