if BUILDABLE_SJ3CLIENT

INCLUDES = \
	-I$(top_srcdir)/include/sj3compat \
	-I$(top_srcdir)/include/sj3common \
	-I$(top_srcdir)/include/sj3lib

LDADD = \
	$(top_srcdir)/lib/sj3lib/libsj3lib.la \
	$(top_srcdir)/lib/sj3compat/libsj3compat.la

sj3dic_SOURCES = \
	sj3dic.h \
	sjctype.h \
	sjtool.h \
	codecnv.c \
	dictdisp.c \
	dictmake.c \
	hinsi.c \
	sj3dic.c \
	sj3err.c \
	sjrc.c

bin_PROGRAMS = sj3dic

endif
