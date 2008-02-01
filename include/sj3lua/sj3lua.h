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

#ifndef SJ3LUA_H
#define SJ3LUA_H

#include <stdio.h>
#include <lua.h>

int sj3lua_check_table(lua_State *);
int lua2sj_boolean(lua_State *, char *, int, int, int *);
int lua2sj_integer(lua_State *, char *, int, int, int *);
char *lua2sj_string(lua_State *, char *, int, char *, char *, size_t);
void sj2lua_boolean(lua_State *, char *, int);
void sj2lua_integer(lua_State *, char *, int);
void sj2lua_string(lua_State *, char *, char *);

#endif

