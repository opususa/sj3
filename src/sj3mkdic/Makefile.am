if BUILDABLE_SJ3SERV

INCLUDES = \
	-I$(top_srcdir)/include/sj3compat \
	-I$(top_srcdir)/include/sj3common \
	-I$(top_srcdir)/include/sj3core

LDADD = $(top_srcdir)/lib/sj3compat/libsj3compat.la

sj3mkdic_SOURCES = \
	GramTable \
	sj3mkdic.h \
	sj3_dict_const.h \
	sj3_dict_struct.h \
	char.c \
	cnvhinsi.c \
	file.c \
	global.c \
	hindo.c \
	knjcvt.c \
	makedict.c \
	makelist.c \
	makeseg.c \
	memory.c \
	offset.c \
	readline.c \
	string.c

bin_PROGRAMS = sj3mkdic

endif
