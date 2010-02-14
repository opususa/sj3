if BUILDABLE_SJ3SERV

EXTRA_DIST = \
	freebsd-sj3.sh.in \
	sj3serv.lua.example.in \
	time_stamp.sh

AM_CFLAGS = $(lua_CFLAGS)

INCLUDES = \
	-I$(top_srcdir)/include/sj3compat \
	-I$(top_srcdir)/include/sj3common \
	-I$(top_srcdir)/include/sj3core \
	-I$(top_srcdir)/include/sj3lua

LDADD = \
	$(top_builddir)/lib/sj3core/libsj3core.la \
	$(top_builddir)/lib/sj3lua/libsj3lua.la

if SJ3_INTERNAL_LUA
AM_CFLAGS += -I$(top_builddir)/lib/lua/src
LDADD     += $(top_builddir)/lib/lua/src/liblua.la @LIBM@
else
AM_CFLAGS += $(LUA_CFLAGS)
LDADD     += $(LUA_LIBS)
endif

LDADD	+= $(top_builddir)/lib/sj3compat/libsj3compat.la

exampledir = $(datadir)/examples/sj3

sj3serv_SOURCES = \
	sj_string.h \
	sj3serv.h \
	comuni.c \
	error.c \
	execute.c \
	main.c \
	priv.c \
	setup.c \
	time_stamp.c \
	version.c \
	sj_string.c

sbin_PROGRAMS = sj3serv

example_DATA = \
	freebsd-sj3.sh.in \
	sj3serv.lua.example

time_stamp.c: $(srcdir)/time_stamp.sh
	sh $(srcdir)/time_stamp.sh > time_stamp.c

CLEANFILES = \
	sj3serv.lua.example \
	time_stamp.c

endif

