if BUILDABLE_SJ3SERV

EXTRA_DIST = \
	freebsd-sj3.sh.in \
	sj3serv.lua.example.in \
	time_stamp.sh

AM_CFLAGS = $(lua_CFLAGS)

INCLUDES = \
	-I$(top_srcdir)/include/sj3compat \
	-I$(top_srcdir)/include/sj3common \
	-I$(top_srcdir)/include/sj3core

LDADD = \
	$(top_srcdir)/lib/sj3core/libsj3core.la \
	$(top_srcdir)/lib/sj3compat/libsj3compat.la

if SJ3_INTERNAL_LUA
AM_CFLAGS += -I$(top_builddir)/lib/lua/src
LDADD     += $(top_builddir)/lib/lua/src/liblua.la @LIBM@
else
AM_CFLAGS += $(LUA_CFLAGS)
LDADD     += $(LUA_LIBS)
endif

exampledir = $(datadir)/examples/sj3

sj3serv_SOURCES = \
	sj_string.h \
	sj3serv.h \
	auth.c \
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

time_stamp.c: time_stamp.sh
	sh ./time_stamp.sh > time_stamp.c

sj3serv.lua.example: sj3serv.lua.example.in
	sed -e "s,%%SJ3VARDIR%%,$(SJ3VARDIR),g" \
	    -e "s,%%SJ3OWNER%%,$(SJ3OWNER),g" \
	    sj3serv.lua.example.in > sj3serv.lua.example

CLEANFILES = \
	sj3serv.lua.example \
	time_stamp.c

endif
