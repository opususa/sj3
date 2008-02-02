if BUILDABLE_SJ3SERV

EXTRA_DIST = \
	freebsd-sj3proxy.sh.in \
	sj3proxy.lua.example.in


INCLUDES = \
	-I$(top_srcdir)/include/sj3common \
	-I$(top_srcdir)/include/sj3compat \
        -I$(top_srcdir)/include/sj3lua

LDADD = \
        $(top_srcdir)/lib/sj3compat/libsj3compat.la \
	$(top_srcdir)/lib/sj3lua/libsj3lua.la

AM_CFLAGS = $(lua_CFLAGS)

if SJ3_INTERNAL_LUA
AM_CFLAGS += -I$(top_builddir)/lib/lua/src
LDADD     += $(top_builddir)/lib/lua/src/liblua.la @LIBM@
else
AM_CFLAGS += $(LUA_CFLAGS)
LDADD     += $(LUA_LIBS)
endif

exampledir = $(datadir)/examples/sj3

sj3proxy_SOURCES = \
	atomicio.h \
	sj3proxy.h \
	atomicio.c \
	priv.c \
	sj3proxy.c

sbin_PROGRAMS = sj3proxy

example_DATA = \
	freebsd-sj3proxy.sh.in \
	sj3proxy.lua.example

sj3proxy.lua.example: sj3proxy.lua.example.in
	sed -e "s,%%SJ3VARDIR%%,$(SJ3VARDIR),g" \
	    -e "s,%%SJ3OWNER%%,$(SJ3OWNER),g" \
	    sj3proxy.lua.example.in > sj3proxy.lua.example

CLEANFILES = \
	sj3proxy.lua.example

endif
