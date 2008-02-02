/*
 * Copyright (c) 2004 Iwata <iwata@quasiquote.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <config.h>

#include <stdio.h>
#include <string.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "sj3lua.h"

int
sj3lua_check_table(lua_State *lstate)
{
	if (!lua_istable(lstate, 1)) {
		fprintf(stderr, "Illegal type of table (type=%s).\n",
		    lua_typename(lstate, lua_type(lstate, 1)));
		return 0;
        }
	return 1;
}

int
lua2sj_boolean(lua_State *lstate, char *name, int idx, int default_val, int *i)
{
	lua_getfield(lstate, idx, name);
	lua_pop(lstate, idx);

	if (lua_isboolean(lstate, idx - 1)) {
		*i = (int)lua_toboolean(lstate, idx - 1);
		return *i;
	} else if (lua_isnil(lstate, idx - 1)) {
		*i = default_val;
		return *i;
	} else {
		fprintf(stderr, "Illegal type of '%s'(%s), must be boolean.\n",
		    name, lua_typename(lstate, lua_type(lstate, idx - 1)));
		exit(1);
	}
}

int
lua2sj_integer(lua_State *lstate, char *name, int idx, int default_val, int *i)
{
	lua_getfield(lstate, idx, name);
	lua_pop(lstate, idx);

	if (lua_isnumber(lstate, idx - 1)) {
		*i = (int)lua_tonumber(lstate, idx - 1);
		return *i;
	} else if (lua_isnil(lstate, idx - 1)) {
		*i = default_val;
		return *i;
	} else {
		fprintf(stderr, "Illegal type of '%s'(%s), must be integer.\n",
		    name, lua_typename(lstate, lua_type(lstate, idx - 1)));
		exit(1);
	}
}

char *
lua2sj_string(lua_State *lstate, char *name, int idx, char *default_val, char *str, size_t len)
{
	lua_getfield(lstate, idx, name);
	lua_pop(lstate, idx);

	if (lua_isstring(lstate, idx - 1)) {
		strlcpy(str, lua_tostring(lstate, idx - 1), len);
		return str;
	} else if (lua_isnil(lstate, idx - 1)) {
		if (default_val == NULL)
			str[0] = '\0';
		else
			strlcpy(str, default_val, len);
		return str;
	} else {
		fprintf(stderr, "Illegal type of '%s'(%s), must be string.\n",
		    name, lua_typename(lstate, lua_type(lstate, idx - 1)));
		exit(1);
	}
}

void
sj2lua_boolean(lua_State *lstate, char *name, int i)
{
	lua_pushstring(lstate, name);
        lua_pushboolean(lstate, i);
        lua_settable(lstate, -3);
}

void
sj2lua_integer(lua_State *lstate, char *name, int i)
{
	lua_pushstring(lstate, name);
        lua_pushnumber(lstate, i);
        lua_settable(lstate, -3);
}

void
sj2lua_string(lua_State *lstate, char *name, char *s)
{
	lua_pushstring(lstate, name);
        lua_pushstring(lstate, s);
        lua_settable(lstate, -3);
}

void
set_luafunction(lua_State *lstate, char *name, lua_CFunction fun)
{
	lua_pushstring(lstate, name);
        lua_pushcfunction(lstate, fun);
        lua_settable(lstate, -3);
}
