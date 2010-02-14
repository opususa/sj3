if BUILDABLE_SJ3SERV

INCLUDES = \
	-I$(top_srcdir)/include/sj3compat \
	-I$(top_srcdir)/include/sj3lua

AM_CFLAGS = $(lua_CFLAGS)

if SJ3_INTERNAL_LUA
AM_CFLAGS += -I$(top_builddir)/lib/lua/src
else
AM_CFLAGS += $(LUA_CFLAGS)
endif

noinst_LTLIBRARIES = libsj3lua.la

libsj3lua_la_SOURCES = \
	sj3lua.c

endif
