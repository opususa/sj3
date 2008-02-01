/*
 * Copyright (c) 1991-1994  Sony Corporation
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL SONY CORPORATION BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * Except as contained in this notice, the name of Sony Corporation
 * shall not be used in advertising or otherwise to promote the sale, use
 * or other dealings in this Software without prior written authorization
 * from Sony Corporation.
 *
 */

/*
 * $SonyRCSfile: setup.c,v $  
 * $SonyRevision: 1.3 $ 
 * $SonyDate: 1994/12/09 11:27:07 $
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/socket.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "sj_typedef.h"
#include "sj_const.h"
#include "sj_var.h"
#include "sj_dict.h"
#include "sj_struct.h"
#include "sj_rename.h"
#include "Const.h"
#include "Struct.h"
#include "sj_global.h"
#include "sys-queue.h"

#include "sj3serv.h"

char program_name[MAXPATHLEN];
char runcmd_file[MAXPATHLEN];

int daemon_enable = -1;

char pid_file[MAXPATHLEN];

int inet_enable = -1;
char inet_address_family[256];
int address_family = AF_UNSPEC;
char inet_host_name[MAXHOSTNAMELEN];
char inet_port_name[256];

int domain_enable = -1;
char domain_socket_name[MAXPATHLEN];

char dict_dir[MAXPATHLEN];
char dict_file[FILENAME_MAX];

char chuser_name[BUFSIZ];

int chroot_enable = -1;
char chroot_dir[MAXPATHLEN];

char log_file[MAXPATHLEN];

char debug_file[MAXPATHLEN];

int debug_level = -1;

char error_file[MAXPATHLEN];

int max_client = -1;

TAILQ_HEAD(, dict_entry) open_dict_head, read_dict_head;
struct dict_entry {
	char filename[MAXPATHLEN];
	char passwd[256];
	TAILQ_ENTRY(dict_entry) entries;
} *read_dict, *open_dict;

int dir_mode = -1;
int file_mode = S_IRUSR | S_IWUSR;

TAILQ_HEAD(, allow_user_entry) allow_user_head;
struct allow_user_entry {
	char username[BUFSIZ];
	char hostname[MAXHOSTNAMELEN];
	TAILQ_ENTRY(allow_user_entry) entries;
} *allow_user;

static int opt_daemon_disable = 0;
static int opt_ipv4 = 0;
static int opt_ipv6 = 0;
static lua_State *lua_state;

static int
check_table(lua_State *lstate)
{
	if (!lua_istable(lstate, 1)) {
		fprintf(stderr, "Illegal type of table (type=%s).\n",
		    lua_typename(lstate, lua_type(lstate, 1)));
		return 0;
        }
	return 1;
}

static int
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

static int
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

static char *
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

static void
sj2lua_boolean(lua_State *lstate, char *name, int i)
{
	lua_pushstring(lstate, name);
        lua_pushboolean(lstate, i);
        lua_settable(lstate, -3);
}

static void
sj2lua_integer(lua_State *lstate, char *name, int i)
{
	lua_pushstring(lstate, name);
        lua_pushnumber(lstate, i);
        lua_settable(lstate, -3);
}

static void
sj2lua_string(lua_State *lstate, char *name, char *s)
{
	lua_pushstring(lstate, name);
        lua_pushstring(lstate, s);
        lua_settable(lstate, -3);
}

static int
set_server(lua_State *lstate)
{
	if (!check_table(lstate))
		return 0;
	lua2sj_boolean(lstate, "daemon",      1, FORKFLAG,     &daemon_enable);
	lua2sj_integer(lstate, "max_client",  1, MAXCLIENTNUM, &max_client);
	lua2sj_string(lstate,  "dict_dir",    1, DICTROOTDIR,  dict_dir,    sizeof(dict_dir));
	lua2sj_string(lstate,  "user",        1, SJ3OWNER,     chuser_name, sizeof(chuser_name));
	lua2sj_boolean(lstate, "chroot",      1, CHROOTFLAG,   &chroot_enable);
	lua2sj_string(lstate,  "chroot_dir",  1, CHROOTDIR,    chroot_dir,  sizeof(dict_dir));

	return 0;
}

static int
get_server(lua_State *lstate)
{
	lua_newtable(lstate);
	sj2lua_boolean(lstate, "daemon",      daemon_enable);
	sj2lua_integer(lstate, "max_client",  max_client);
	sj2lua_string(lstate,  "dict_dir",    dict_dir);
	return 1;
}

static int
set_inet(lua_State *lstate)
{
	if (!check_table(lstate))
		return 0;
	lua2sj_boolean(lstate, "enable",         1, 0,             &inet_enable);
	lua2sj_string(lstate,  "address_family", 1, ADDRESSFAMILY, inet_address_family, sizeof(inet_address_family));
	lua2sj_string(lstate,  "host_name",      1, LOCALHOST,     inet_host_name,      sizeof(inet_host_name));
	lua2sj_string(lstate,  "port_name",      1, PORTNUMBER,    inet_port_name,      sizeof(inet_port_name));
	return 0;
}

static int
get_inet(lua_State *lstate)
{
	sj2lua_boolean(lstate, "enable",         inet_enable);
	sj2lua_string(lstate,  "address_family", inet_address_family);
	sj2lua_string(lstate,  "host_name",      inet_host_name);
	sj2lua_string(lstate,  "port_name",      inet_port_name);

	return 1;
}

static int
set_domain(lua_State *lstate)
{
	if (!check_table(lstate))
		return 0;
	lua2sj_boolean(lstate, "enable",      1, 0,          &domain_enable);
	lua2sj_string(lstate,  "socket_name", 1, SOCKETNAME, domain_socket_name, sizeof(domain_socket_name));

	return 0;
}

static int
get_domain(lua_State *lstate)
{
	sj2lua_boolean(lstate, "enable",      domain_enable);
	sj2lua_string(lstate,  "socket_name", domain_socket_name);

	return 1;
}

static int
append_readdict(lua_State *lstate)
{
	char dict[MAXPATHLEN];
	char passwd[BUFSIZ];

	if (!check_table(lstate))
		return 0;
	lua2sj_string(lstate, "file", 1, NULL, dict, sizeof(dict));
	lua2sj_string(lstate, "passwd", 1, NULL, passwd, sizeof(passwd));

	read_dict = malloc(sizeof(struct dict_entry));
	strlcpy(read_dict->filename, dict, sizeof(read_dict->filename));
	strlcpy(read_dict->passwd, passwd, sizeof(read_dict->passwd));
	TAILQ_INSERT_TAIL(&read_dict_head, read_dict, entries);

	return 0;
}

static int
append_opendict(lua_State *lstate)
{
	char dict[MAXPATHLEN];
	char passwd[BUFSIZ];

	if (!check_table(lstate))
		return 0;
	lua2sj_string(lstate, "file", 1, NULL, dict, sizeof(dict));
	lua2sj_string(lstate, "passwd", 1, NULL, passwd, sizeof(passwd));

	open_dict = malloc(sizeof(struct dict_entry));
	strlcpy(open_dict->filename, dict, sizeof(open_dict->filename));
	strlcpy(open_dict->passwd, passwd, sizeof(open_dict->passwd));
	TAILQ_INSERT_TAIL(&open_dict_head, open_dict, entries);

	return 0;
}

static int
set_log(lua_State *lstate)
{
	if (!check_table(lstate))
		return 0;
	lua2sj_string(lstate, "file", 1, LOGOUTFILE, log_file, sizeof(log_file));

	return 0;
}

static int
get_log(lua_State *lstate)
{
	sj2lua_string(lstate, "file", log_file);

	return 1;
}

static int
set_debug(lua_State *lstate)
{
	if (!check_table(lstate))
		return 0;
	lua2sj_string(lstate, "file",   1, DEBUGOUTFILE, debug_file, sizeof(debug_file));
	lua2sj_integer(lstate, "level", 1, DEBUGLEVEL,   &debug_level);

	return 0;
}

static int
get_debug(lua_State *lstate)
{
	sj2lua_string(lstate,  "file",  debug_file);
	sj2lua_integer(lstate, "level", debug_level);

	return 1;
}

static int
set_error(lua_State *lstate)
{
	if (!check_table(lstate))
		return 0;
	lua2sj_string(lstate, "file", 1, ERROROUTFILE, error_file, sizeof(error_file));

	return 0;
}

static int
append_allowuser(lua_State *lstate)
{
	char username[BUFSIZ];
	char hostname[MAXHOSTNAMELEN];

	if (!check_table(lstate))
		return 0;
	lua2sj_string(lstate, "user", 1, NULL, username, sizeof(username));
	lua2sj_string(lstate, "host", 1, NULL, hostname, sizeof(hostname));

	allow_user = malloc(sizeof(struct allow_user_entry));
	strlcpy(allow_user->username, username, sizeof(allow_user->username));
	strlcpy(allow_user->hostname, hostname, sizeof(allow_user->hostname));
	TAILQ_INSERT_TAIL(&allow_user_head, allow_user, entries);

	return 0;
}

static int
get_error(lua_State *lstate)
{
	sj2lua_string(lstate, "file", error_file);

	return 0;
}

void
set_luafunction(lua_State *lstate, char *name, lua_CFunction fun)
{
	lua_pushstring(lstate, name);
        lua_pushcfunction(lstate, fun);
        lua_settable(lstate, -3);
}

void
read_runcmd()
{
	if (lua_state == NULL) {
		exit(1);
	}

	luaL_openlibs(lua_state);

	if (luaL_dofile(lua_state, runcmd_file) != 0) {
		fprintf(stderr, "%s\n", lua_tostring(lua_state, -1));
		exit(255);
	}

	/* overwrite from opt_args */
	if (opt_daemon_disable)
		daemon_enable = 0;
	else
		daemon_enable = 1;

	if (opt_ipv4)
		address_family = AF_INET;
	else if (opt_ipv6)
		address_family = AF_INET6;
	else if (strcmp(inet_address_family, "inet") == 0)
		address_family = AF_INET;
	else if (strcmp(inet_address_family, "inet6") == 0)
		address_family = AF_INET6;

#if 0 /* DON'T LEAVE */
	lua_close(lua_state);
	lua_state = NULL;
#endif
}

void
set_default()
{
	if (lua_state != NULL) {
		warning_out("lua is Already loaded, why?");
		return;
	}

	if ((lua_state = luaL_newstate()) == NULL) {
		warning_out("Can't setup lua");
		return;
	}

	/* sj3.functions */
	lua_newtable(lua_state);
	set_luafunction(lua_state, "set_server",      set_server);
	set_luafunction(lua_state, "set_inet",        set_inet);
	set_luafunction(lua_state, "set_domain",      set_domain);
	set_luafunction(lua_state, "append_readdict", append_readdict);
	set_luafunction(lua_state, "append_opendict", append_opendict);
	set_luafunction(lua_state, "set_log",         set_log);
	set_luafunction(lua_state, "set_debug",       set_debug);
	set_luafunction(lua_state, "set_error",       set_error);
	set_luafunction(lua_state, "append_allowuser", append_allowuser);

	set_luafunction(lua_state, "get_server",      get_server);
	set_luafunction(lua_state, "get_inet",        get_inet);
	set_luafunction(lua_state, "get_domain",      get_domain);
	set_luafunction(lua_state, "get_log",         get_log);
	set_luafunction(lua_state, "get_debug",       get_debug);
	set_luafunction(lua_state, "get_error",       get_error);

	TAILQ_INIT(&open_dict_head);
	TAILQ_INIT(&read_dict_head);
	TAILQ_INIT(&allow_user_head);

	lua_setglobal(lua_state, "sj3");
}

extern char *optarg;
extern int optind;

void
parse_arg(int argc, char **argv)
{
	int	c;
	int	errflg = 0;
	char	*p;
	size_t	ret;

	p = (p = strrchr(argv[0], '/')) ? p + 1 : argv[0];
	strlcpy(program_name, p, sizeof(program_name));
	strlcpy(runcmd_file, RUNCMDFILE, sizeof(runcmd_file));

	while ((c = getopt(argc, argv, "46f:d")) != EOF) {
		switch (c) {
		case '4':
			opt_ipv4 = 1;
			break;
		case '6':
			opt_ipv6 = 1;
			break;
		case 'f':
			ret = strlcpy(runcmd_file, optarg, sizeof(runcmd_file));
			if (ret > sizeof(runcmd_file))
				errflg++;
			break;

		case 'd':
			opt_daemon_disable = 1;
			break;
		case '?':
		default:
			errflg++;
			break;
		}
	}

	if (errflg || optind < argc) {
		fprintf(stderr, "Usage: %s [-46] [-d] [-f RunCmdFile]\n", program_name);
		exit(1);
	}
}


void
preload_dict()
{
	struct dict_entry *np;
	char filename[MAXPATHLEN];

	work_base = (Global *)malloc(sizeof(Global));
	if (!work_base) {
		warning_out("can't load default dictionary(not enough memory)");
		return;
	}

	TAILQ_FOREACH(np, &read_dict_head, entries) {
		strlcpy(filename, (char *)np->filename, sizeof(filename));
		if (make_full_path(filename, sizeof(filename)))
			warning_out("too long filename \"%s\"", np->filename);

		else if (opendict(filename, np->passwd))
			logging_out("load dictionary \"%s\"", filename);

		else
			warning_out("can't open dictionary \"%s\"", filename);
	}
	free(work_base);
}

void
preopen_dict()
{
	struct dict_entry *np;
	char filename[MAXPATHLEN];
	DICT *dict;
	DICTL *dictl;

	work_base = global_work_base = (Global *)malloc(sizeof(Global));
	if (!work_base) {
		warning_out("can't open default dictionary(not enough memory)");
		return;
	}
	memset(work_base, 0, sizeof(Global));

	TAILQ_FOREACH(np, &open_dict_head, entries) {
		strlcpy(filename, np->filename, sizeof(filename));
		if (make_full_path(filename, sizeof(filename)))
			warning_out("too long filename \"%s\"", np->filename);
		else if ((dict = (DICT *)opendict(filename, np->passwd)) != NULL) {
			logging_out("open dictionary \"%s\"", filename);
		        if (!(dictl = (DICTL *)malloc(sizeof *dictl))) {
				closedict((DictFile *)dict);
				warning_out("can't open dictionary \"%s\"", filename);
				continue;
			}
			dictl->dict = dict;
			dictl->next = dictlist;
			dictlist = dictl;
		} else
			warning_out("can't open dictionary \"%s\"", filename);
	}
}



static int
str_match(char *s, char *d)
{
	while (*d) {
		if (*s == '*') {
			while (*s == '*') s++;
			if (!*s)
				return TRUE;
			while (*d) {
				if (str_match(s, d))
					return TRUE;
				d++;
			}
			return FALSE;
		}
		if (*s != '?' && *s != *d)
			break;
		s++;
		d++;
	}
	return (!*s) ? TRUE : FALSE;
}

int
check_user(char *user, char *host)
{
	struct allow_user_entry *np;

	if (TAILQ_EMPTY(&allow_user_head))
		return TRUE;

	TAILQ_FOREACH(np, &allow_user_head, entries) {
		if (!str_match(np->username, user))
			continue;

		if (np->hostname)
			if (!str_match(np->hostname, host))
				continue;

		return TRUE;
	}

	return FALSE;
}
