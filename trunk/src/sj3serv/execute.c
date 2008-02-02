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
 * $SonyRCSfile: execute.c,v $  
 * $SonyRevision: 1.7 $ 
 * $SonyDate: 1998/04/13 11:20:12 $
 */

#include <config.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sj_kcnv.h"
#include "sj_kanakan.h"
#include <pwd.h>
#include <setjmp.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "sj3cmd.h"
#include "sj3err.h"
#include "Dict.h"

#include "sj3serv.h"

#define rmemcpy(a, b, n) memcpy((b), (a), (n))

#define SJIS_PROTO 1
#define EUC_PROTO 2


static	u_char	buf1[YOMIBUFSIZE];
static	u_char	buf2[YOMIBUFSIZE];
static	u_char	buf3[YOMIBUFSIZE];
static	u_char	buf4[YOMIBUFSIZE];
static	u_char	kbuf[KANJIBUFSIZE];
static	u_char	skbuf[KANJIBUFSIZE];

static	jmp_buf	error_ret;
static	WorkArea *worklist = NULL;
static	Client	*cur_cli;

static u_char defaultchar[2] = {(u_char)0x81, (u_char)0x40};
static int  defuse=0, defuse2=0;

int sj3_str_sjistoeuc(unsigned char *, int, unsigned char *, unsigned char *, int *);
int sj3_str_euctosjis(unsigned char *, int, unsigned char *, unsigned char *, int *);

int
make_full_path(char *path, int pathlen)
{
	char	tmp[MAXPATHLEN];
	char    *index;
	int	i;

	if (!path || !*path)
		return ERROR;

	i = strlen(dict_dir) + 1 + strlen(path) + 1;
	if (i > sizeof(tmp))
		return ERROR;

	strlcpy(tmp, path, sizeof(tmp));
	index = strtok(tmp, "/");
	do {
		if (!strcmp(index, ".."))
			return ERROR;
	} while ((index = strtok(NULL, "/")) != NULL );

	if (*path == '/') {
		if (strncmp(path, dict_dir, strlen(dict_dir)) == 0) {
			return 0;
		} else {
			return ERROR;
		}
	}

	strlcpy(tmp, dict_dir, pathlen);
	strlcat(tmp, "/", pathlen);
	strlcat(tmp, path, pathlen);
	strlcpy(path, tmp, pathlen);
	return 0;
}



WorkArea *
alloc_workarea()
{
	WorkArea *p;

	if (!(p = (WorkArea *)malloc(sizeof(WorkArea)))) return NULL;

	memset(p, '\0', sizeof(WorkArea));
	p -> lock   = NOTLOCKED;
	p -> refcnt = 1;

	p -> link = worklist;
	worklist = p;

	work_base = &(p -> global);
	initwork();

	return p;
}

void
free_workarea(WorkArea *p)
{
	if (!p)
		return;
	if (--(p -> refcnt))
		return;

	if (worklist == p)
		worklist = p -> link;
	else {
		WorkArea *q;

		for (q = worklist ; q ; q = q -> link) {
			if (q -> link == p) {
				q -> link = p -> link;
				break;
			}
		}
	}

	if (p -> global.Jdictlist)
		close_dictlist(p -> global.Jdictlist);

	free((char *)p);
}


void
exec_connect()
{
	int	version;
	char	hostname[MAXHOSTNAMELEN];
	char	username[MAXLOGNAME];
	char	progname[MAXPATHLEN];
	WorkArea *wp;
	sj3_ugid ugid;

	version = get_int();
	get_nstring(hostname, sizeof(hostname));
	get_nstring(username, sizeof(username));
	get_nstring(progname, sizeof(progname));

	if (cur_cli -> work)
		longjmp(error_ret, SJ3_AlreadyConnected);

	if (version < 0)
		longjmp(error_ret, SJ3_DifferentVersion);

	if (hostname[0] == '\0')
		longjmp(error_ret, SJ3_NoHostName);

	if (username[0] == '\0')
		longjmp(error_ret, SJ3_NoUserName);

	if (!check_user(username, hostname))
		longjmp(error_ret, SJ3_NotAllowedUser);

	if (!(wp = alloc_workarea()))
		longjmp(error_ret, SJ3_NotEnoughMemory);

	if (auth_getugid(username, &ugid)) {
		struct sockaddr addr;
		uid_t euid;
		gid_t egid;
		int len = sizeof(addr);

		if (getsockname(cur_client->fd, &addr, &len) < 0)
			perror("getsockname");

		if (strict_auth_enable) {
			getpeereid(cur_client->fd, &euid, &egid);
			if (ugid.uid != euid || ugid.gid != egid) {
				longjmp(error_ret, SJ3_NotAllowedUser);
			}
		}
		cur_cli -> uid = ugid.uid;
	} else
		longjmp(error_ret, SJ3_NotAllowedUser);

        cur_cli -> version = version;
	strlcpy(cur_cli -> host, hostname, sizeof(cur_cli -> host));
	strlcpy(cur_cli -> user, username, sizeof(cur_cli -> user));
	strlcpy(cur_cli -> prog, progname, sizeof(cur_cli -> prog));
        rmemcpy(cur_cli -> def_char, defaultchar, 2);
	cur_cli -> work = wp;

        if (version == 1)
	        put_int(SJ3_NormalEnd);
        else
                put_int((-SJ3SERV_VERSION_NO));

	logging_out("connection established on %d (%s, %s, %s)",
			cur_cli -> fd, hostname, username, progname);
}


void
exec_disconnect()
{
	if (cur_cli -> stdy) {
		closestdy(cur_cli -> stdy);
		cur_cli -> stdy = NULL;
	}
	if (cur_cli -> work) {
		free_workarea(cur_cli -> work);
		cur_cli -> work = NULL;
	}
	else
		longjmp(error_ret, SJ3_NotConnected);

	put_int(SJ3_NormalEnd);
}


void
exec_opendict()
{
	char	filename[MAXPATHLEN];
	char	password[PASSWORDLEN];
	DICT	*dict;
	DICTL   *dictl, *dltp, *tmp;

	get_nstring(filename, sizeof(filename));
	get_nstring(password, sizeof(password));

	if (!work_base)
		longjmp(error_ret, SJ3_NotConnected);

	if (make_full_path(filename, sizeof(filename)))
		longjmp(error_ret, SJ3_TooLongParameter);

	if (!(dict = (DICT *)opendict(filename, password)))
		longjmp(error_ret, serv_errno);
	if (!dictlist && global_work_base) {
		DICTL *b_dictl = NULL;

		dltp = global_work_base -> Jdictlist;
		dictl = NULL;
		while (dltp) {
			dictl = (DICTL *)malloc(sizeof *dictl);
			if (!dictl) {
				for (tmp = dictlist; tmp; tmp->next)
					free(tmp);
				dictlist = NULL;
			}
			if (!dictlist)
				dictlist = dictl;
			dictl->dict = dltp->dict;
			((DictFile *)(dictl -> dict))->refcnt++;
			dictl->next = NULL;
			if (b_dictl)
				b_dictl->next = dictl;
			b_dictl = dictl;
			dltp = dltp->next;
		}
	}
	for (dltp = dictlist; dltp && dltp->dict != dict; dltp = dltp->next) ;

	if (!dltp) {
		if (!(dictl = (DICTL *)malloc(sizeof *dictl))) {
			closedict((DictFile *)dict);
			longjmp(error_ret, SJ3_NotEnoughMemory);
		}

		dictl -> dict = dict;
		dictl -> next = dictlist;
		dictlist = dictl;
	}

	if (cur_cli -> work -> lock == cur_cli -> fd) {
		if (dict -> maxunit != 0)
			lock_dict((DictFile *)dict, cur_cli -> fd);
	}

	put_int(SJ3_NormalEnd);
	put_int((int)dict->dicid);
}
void
exec_closedict()
{
	TypeDicID	id;
	DICTL	*dictl;

	id = get_int();

	if (!work_base)
		longjmp(error_ret, SJ3_NotConnected);

	for (dictl = dictlist ; dictl ; dictl = dictl -> next)
		if (dictl -> dict -> dicid == id)
			break;
	if (!dictl)
		longjmp(error_ret, SJ3_BadDictID);

	unlock_dict((DictFile *)(dictl -> dict), cur_cli -> fd);

	closedict((DictFile *)(dictl -> dict));
	if (dictlist == dictl) {
		dictlist = dictl -> next;
	} else {
		DICTL	*p;

		for (p = dictlist ; p -> next ; p = p -> next) {
			if (p -> next == dictl) {
				p -> next = dictl -> next;
				break;
			}
		}
	}
	free((char *)dictl);
	put_int(SJ3_NormalEnd);
}
void
close_dictlist(DICTL *dictl)
{
	DICTL	*p;

	while (dictl) {
		unlock_dict((DictFile *)(dictl -> dict), cur_cli -> fd);
		closedict((DictFile *)(dictl -> dict));
		p = dictl -> next;
		free((char *)dictl);
		dictl = p;
	}
}


void
exec_openstdy()
{
	char	filename[MAXPATHLEN];
	char	password[PASSWORDLEN];
	StdyFile *stdy;

	get_nstring(filename, sizeof(filename));
	get_nstring(password, sizeof(password));

	if (cur_cli -> stdy)
		longjmp(error_ret, SJ3_StdyAlreadyOpened);
	if (make_full_path(filename, sizeof(filename)))
		longjmp(error_ret, SJ3_TooLongParameter);

	if (!(stdy = openstdy(filename, password)))
		longjmp(error_ret, serv_errno);
	cur_cli -> stdy = stdy;

	put_int(SJ3_NormalEnd);
}
void
exec_closestdy()
{
	if (!cur_cli -> stdy)
		longjmp(error_ret, SJ3_StdyFileNotOpened);

	closestdy(cur_cli -> stdy);
	cur_cli -> stdy = NULL;

	put_int(SJ3_NormalEnd);
}


void
exec_stdysize()
{
	put_int(SJ3_NormalEnd);
	put_int(sizeof(STDYOUT));
}



static void
exec_lock()
{
	int	lock;
	DICTL	*dl;

	if (!work_base)
		longjmp(error_ret, SJ3_NotConnected);

	lock = cur_cli -> work -> lock;
	if (lock != NOTLOCKED && lock != cur_cli -> fd)
		longjmp(error_ret, SJ3_LockedByOther);
	cur_cli -> work -> lock = cur_cli -> fd;

	for (dl = dictlist ; dl ; dl = dl -> next) {
		if (dl -> dict -> maxunit == 0)
			continue;
		lock_dict((DictFile *)(dl -> dict), cur_cli -> fd);
	}

	put_int(SJ3_NormalEnd);
}
static void
exec_unlock()
{
	int	lock;
	DICTL	*dl;

	if (!work_base)
		longjmp(error_ret, SJ3_NotConnected);

	lock = cur_cli -> work -> lock;
	if (lock == NOTLOCKED)
		longjmp(error_ret, SJ3_NotLocked);
	if (lock != cur_cli -> fd)
		longjmp(error_ret, SJ3_LockedByOther);
	cur_cli -> work -> lock = NOTLOCKED;

	for (dl = dictlist ; dl ; dl = dl -> next) {
		if (dl -> dict -> maxunit == 0)
			continue;
		unlock_dict((DictFile *)(dl -> dict), cur_cli -> fd);
	}

	put_int(SJ3_NormalEnd);
}
static void
lock_check_for_read()
{
	int	lock;

	lock = cur_cli -> work -> lock;
	if (lock == NOTLOCKED)
#ifdef SEVERE_LOCK
		longjmp(error_ret, SJ3_NotLocked);
#else
		;
#endif
	else if (lock != cur_cli -> fd)
		longjmp(error_ret, SJ3_LockedByOther);
}


void
exec_ph2knj(int mb_flag)
{
	int	i, j, l, stdy_size, buf_size, srchead = 0, srclen;
	u_char	*p,*q;

	i = get_nstring(buf1, sizeof(buf1));

	if (!work_base)
		longjmp(error_ret, SJ3_NotConnected);
	if (i)
		longjmp(error_ret, SJ3_TooLongParameter);
	lock_check_for_read();

	if (mb_flag == SJIS_PROTO) {
		if (sj3_str_sjistoeuc(buf2, sizeof(buf2), buf1,
				       cur_cli->def_char, &defuse) < 0)
		        goto CCONVERR;
		i = ph2knj(buf2, kbuf, sizeof(kbuf));
		p = kbuf;
		q = skbuf;
		stdy_size = sizeof(STDYOUT) + 1;
		buf_size = sizeof(skbuf);

		while (*p) {
			srchead += *p;
			srclen = *p;
			buf_size -= stdy_size;
			if (buf_size <= 0)
				goto CCONVERR;
			(void) memcpy(q, p, stdy_size);
			p += stdy_size;
			q += stdy_size;
			l = sj3_str_euctosjis(q, buf_size, p, cur_cli->def_char, &defuse);
			if (l < 0)
				goto CCONVERR;
			if (defuse) {
				(void) memcpy(buf3, &(buf2[srchead-srclen]), srclen);
				buf3[srclen] = '\0';
                                j = cl2knj(buf3, srclen, buf4);
                                l = sj3_str_euctosjis(q, buf_size, &(buf4[stdy_size]),
						  cur_cli->def_char, &defuse);
                                while(defuse && (j == srclen)) {
					j = nextcl(buf4, 0);
					l = sj3_str_euctosjis(q, buf_size, &(buf4[stdy_size]),
							  cur_cli->def_char, &defuse);
					if (l < 0)
						goto CCONVERR;
				}
                                if (!defuse && (j == srclen)) {
					(void) memcpy(q-stdy_size, buf4, stdy_size);
				} else {
					l = sj3_str_euctosjis(q, buf_size, buf3,
						  cur_cli->def_char, &defuse);
					if ((l < 0) || defuse)
						goto CCONVERR;
				}
                        }
			p += strlen(p) + 1;
			q += l + 1; buf_size -= l + 1;
		}
		*q = '\0';
		p = skbuf;
	} else {
		i = ph2knj(buf1, kbuf, sizeof(kbuf));
		p = kbuf;
	}

	put_int(SJ3_NormalEnd);
	put_int(i);
	while (*p) {
		put_byte(*p); p++;
		p = put_ndata(p, sizeof(STDYOUT));
		p = put_string(p);
	}
	put_byte(0);
	return;
CCONVERR:
	put_int(SJ3_CannotCodeConvert);
	return;
}


void
exec_cl2knj(int mb_flag)
{
	int	len, stdy_size, buf_size;
	int	i, j, l;
	u_char	*ptr, *q;

	len = get_int();
	i = get_nstring(buf1, sizeof(buf1));

	if (!work_base)
		longjmp(error_ret, SJ3_NotConnected);
	if (i)
		longjmp(error_ret, SJ3_TooLongParameter);
	lock_check_for_read();

	if (mb_flag == SJIS_PROTO) {
		if (sj3_str_sjistoeuc(buf2, sizeof(buf2), buf1,
				       cur_cli->def_char, &defuse) < 0)
		        goto CCONVERR;
		i = cl2knj(buf2, strlen(buf2), kbuf);
		ptr = kbuf;
		q = skbuf;
		buf_size = sizeof(skbuf);
		stdy_size = sizeof(STDYOUT);

		(void) memcpy(q, ptr, stdy_size);
		ptr += stdy_size;
		q += stdy_size;
		buf_size -= stdy_size;
		l = sj3_str_euctosjis(q, buf_size, ptr,
				      cur_cli->def_char, &defuse);
		if (l < 0)
			goto CCONVERR;
		if (defuse) {
			j = strlen(buf2);
			while (defuse && (i == j)) {
				i = nextcl(buf4, 0);
				l = sj3_str_euctosjis(q, buf_size, &(buf4[stdy_size]),
						  cur_cli->def_char, &defuse);
				if (l < 0)
					goto CCONVERR;
			}
			if (!defuse && (j == i)) {
				(void) memcpy(q-stdy_size, buf4, stdy_size);
			} else {
				l = sj3_str_euctosjis(q, buf_size, buf2,
						  cur_cli->def_char, &defuse);
				if ((l < 0) || defuse) goto CCONVERR;
				i = l;
			}
		}
		ptr = skbuf;
	} else {
		i = cl2knj(buf1, len, kbuf);
		ptr = kbuf;
	}

	put_int(SJ3_NormalEnd);
	put_int(i);
	ptr = put_ndata(ptr, sizeof(STDYOUT));
	put_string(ptr);
	return;
CCONVERR:
	put_int(SJ3_CannotCodeConvert);
	return;
}


void
exec_nextcl(int mb_flag)
{
	int	mode, stdy_size, buf_size;
	int	i, l;
	u_char	*ptr, *q;

	mode = get_int();

	if (!work_base)
		longjmp(error_ret, SJ3_NotConnected);
	lock_check_for_read();

	i = nextcl(kbuf, mode);
	if (mb_flag == SJIS_PROTO) {
		ptr = kbuf;
		q = skbuf;
		buf_size = sizeof(skbuf);
		stdy_size = sizeof(STDYOUT);

		(void) memcpy(q, ptr, stdy_size);
		ptr += stdy_size;
		q += stdy_size;
		buf_size -= stdy_size;
		l = sj3_str_euctosjis(q, buf_size, ptr, cur_cli->def_char, &defuse);
                if (l < 0)
			goto CCONVERR;
		while (defuse && i) {
			i = nextcl(kbuf, mode);
			l = sj3_str_euctosjis(q, buf_size, &(kbuf[stdy_size]),
					  cur_cli->def_char, &defuse);
			if (l < 0)
				goto CCONVERR;
		}
		if (!defuse && i) {
			(void) memcpy(q-stdy_size, kbuf, stdy_size);
		} else {
			put_int(SJ3_NoMoreDouonWord);
			return;
		}
		ptr = skbuf;
	} else {
		ptr = kbuf;
	}

	put_int(SJ3_NormalEnd);
	put_int(i);
	ptr = put_ndata(ptr, sizeof(STDYOUT));
	put_string(ptr);
	return;
CCONVERR:
        put_int(SJ3_CannotCodeConvert);
        return;
}


void
exec_prevcl(int mb_flag)
{
	int	mode, stdy_size, buf_size;
	int	i, l;
	u_char	*ptr, *q;

	mode = get_int();

	if (!work_base)
		longjmp(error_ret, SJ3_NotConnected);
	lock_check_for_read();

	i = prevcl(kbuf, mode);
	if (mb_flag == SJIS_PROTO) {
		ptr = kbuf;
		q = skbuf;
		buf_size = sizeof(skbuf);
		stdy_size = sizeof(STDYOUT);

		(void) memcpy(q, ptr, stdy_size);
		ptr += stdy_size;
		q += stdy_size;
		buf_size -= stdy_size;
		l = sj3_str_euctosjis(q, buf_size, ptr, cur_cli->def_char, &defuse);
                if (l < 0)
			goto CCONVERR;
		while (defuse && i) {
			i = prevcl(kbuf, mode);
			l = sj3_str_euctosjis(q, buf_size, &(kbuf[stdy_size]),
					  cur_cli->def_char, &defuse);
			if (l < 0)
				goto CCONVERR;
		}
		if (!defuse && i) {
			(void) memcpy(q-stdy_size, kbuf, stdy_size);
		} else {
			put_int(SJ3_NoMoreDouonWord);
			return;
		}
		ptr = skbuf;
	} else {
		ptr = kbuf;
	}

	put_int(SJ3_NormalEnd);
	put_int(i);
	ptr = put_ndata(ptr, sizeof(STDYOUT));
	put_string(ptr);
	return;
CCONVERR:
        put_int(SJ3_CannotCodeConvert);
        return;
}


void
exec_cl2knj_cnt(int mb_flag)
{
	int	len, buf_size, stdy_size;
	int	i;
	int	cnt = 0;

	len = get_int();
	i = get_nstring(buf1, sizeof(buf1));

	if (!work_base)
		longjmp(error_ret, SJ3_NotConnected);
	if (i)
		longjmp(error_ret, SJ3_TooLongParameter);
	lock_check_for_read();

	if (mb_flag == SJIS_PROTO) {
		if (sj3_str_sjistoeuc(buf2, sizeof(buf2), buf1,
				       cur_cli->def_char, &defuse) < 0)
		        goto CCONVERR;
		i = cl2knj(buf2, len, kbuf);
		stdy_size = sizeof(STDYOUT);
		buf_size = sizeof(skbuf) - stdy_size;
	} else {
		i = cl2knj(buf1, len, kbuf);
	}
	if (i) {
		while (i == len) {
			if (mb_flag == SJIS_PROTO) {
				if (sj3_str_euctosjis(skbuf, buf_size,
						   &(kbuf[stdy_size]),
						   cur_cli->def_char, &defuse)
				    < 0)
				    goto CCONVERR;
				if (!defuse)
					cnt++;
			} else
			        cnt++;
			i = nextcl(kbuf, 0);
		}
	}

	put_int(SJ3_NormalEnd);
	put_int(cnt);
	return;
CCONVERR:
	put_int(SJ3_CannotCodeConvert);
	return;
}


void
exec_cl2knj_all(int mb_flag)
{
	int	len, stdy_size, buf_size;
	int	i, l;
	u_char	*ptr, *q;

	len = get_int();
	i = get_nstring(buf1, sizeof(buf1));

	if (!work_base)
		longjmp(error_ret, SJ3_NotConnected);
	if (i)
		longjmp(error_ret, SJ3_TooLongParameter);
	lock_check_for_read();

	if (mb_flag == SJIS_PROTO) {
		if (sj3_str_sjistoeuc(buf2, sizeof(buf2), buf1,
				   cur_cli->def_char, &defuse) < 0)
		        goto CCONVERR;
		len = strlen(buf2);
		i = cl2knj(buf2, len, kbuf);
		ptr = kbuf;
		q = skbuf;
		buf_size = sizeof(skbuf);
		stdy_size = sizeof(STDYOUT);

		(void) memcpy(q, ptr, stdy_size);
		ptr += stdy_size;
		q += stdy_size;
		buf_size -= stdy_size;
		l = sj3_str_euctosjis(q, buf_size, ptr, cur_cli->def_char, &defuse);
		if (l < 0)
			goto CCONVERR;
		if (defuse) {
			while (defuse && (i == len)) {
                                i = nextcl(kbuf, 0);
				l = sj3_str_euctosjis(q, buf_size,
						  &(kbuf[stdy_size]),
						  cur_cli->def_char, &defuse);
                                if (l < 0)
					goto CCONVERR;
			}
			if (!defuse && (i == len)) {
                                (void) memcpy(q-stdy_size, kbuf, stdy_size);
			} else {
				put_int(SJ3_NoMoreDouonWord);
				return;
			}
		}
		ptr = skbuf;
	} else {
		i = cl2knj(buf1, len, kbuf);
		ptr = kbuf;
	}
	put_int(SJ3_NormalEnd);
	if (i) {
		while (i == len) {
			put_int(i);
			ptr = put_ndata(ptr, sizeof(STDYOUT));
			put_string(ptr);
			i = nextcl(kbuf, 0);
			if (mb_flag == SJIS_PROTO) {
				ptr = kbuf;
				q = skbuf;
				buf_size = sizeof(skbuf);
				stdy_size = sizeof(STDYOUT);

				(void) memcpy(q, ptr, stdy_size);
				ptr += stdy_size;
				q += stdy_size;
				buf_size -= stdy_size;
				l = sj3_str_euctosjis(q, buf_size, ptr,
						  cur_cli->def_char, &defuse);
				if (l < 0)
					goto CCONVERR;
				if (defuse) {
					while (defuse && (i == len)) {
						i = nextcl(kbuf, 0);
						l = sj3_str_euctosjis(q, buf_size,
								  &(kbuf[stdy_size]),
								  cur_cli->def_char,
								  &defuse);
						if (l < 0)
							goto CCONVERR;
					}
					if (!defuse && (len == i)) {
						(void) memcpy(q-stdy_size,
							       kbuf, stdy_size);
					}
				}
				ptr = skbuf;
			} else {
				ptr = kbuf;
			}
		}
	}
	put_int(0);
	return;
CCONVERR:
        put_int(SJ3_CannotCodeConvert);
        return;
}


void
exec_study()
{
	int	err;
	STDYOUT	stdy;

	get_ndata(&stdy, sizeof(stdy));

	if (!work_base)
		longjmp(error_ret, SJ3_NotConnected);
	if (!stdy_base)
		longjmp(error_ret, SJ3_StdyFileNotOpened);
	lock_check_for_read();

	err = study(&stdy);

	switch (err) {
	case STUDYNODICT:	err = SJ3_StdyFileNotOpened; break;
	case STUDYNORMEND:
	case STUDYFULLAREA:
	case STUDYNOTSTUDY:	err = SJ3_NormalEnd; break;
	case STUDYERROR:
	default:		err = SJ3_InternalError; break;
	}
	if (err != SJ3_NormalEnd)
		longjmp(error_ret, err);
	put_int(err);
}


void
exec_clstudy(mb_flag)
int mb_flag;
{
	int	err;
	STDYOUT	stdy;

	err  = get_nstring(buf1, sizeof(buf1));
	err |= get_nstring(buf2, sizeof(buf2));
	get_ndata(&stdy, sizeof(stdy));

	if (!work_base)
		longjmp(error_ret, SJ3_NotConnected);
	if (!stdy_base)
		longjmp(error_ret, SJ3_StdyFileNotOpened);
	if (err) longjmp(error_ret, SJ3_TooLongParameter);
	lock_check_for_read();

	if (mb_flag == SJIS_PROTO) {
		if (sj3_str_sjistoeuc(buf3, sizeof(buf3), buf1,
				       cur_cli->def_char, &defuse) < 0)
		        goto CCONVERR;
		if (sj3_str_sjistoeuc(buf4, sizeof(buf4), buf2,
				       cur_cli->def_char, &defuse) < 0)
		        goto CCONVERR;
		err = clstudy(buf3, buf4, &stdy);
	} else {
		err = clstudy(buf1, buf2, &stdy);
	}

	switch (err) {
	case CLSTUDYNODICT:     err = SJ3_StdyFileNotOpened; break;
	case CLSTUDYNORMEND:	err = SJ3_NormalEnd; break;
	case CLSTUDYPARAERR:
	case CLSTUDYYOMIERR:	err = SJ3_IllegalParameter; break;
	case CLSTUDYNOAREA:	err = SJ3_TooSmallStdyArea; break;
	default:		err = SJ3_InternalError; break;
	}
	if (err != SJ3_NormalEnd)
		longjmp(error_ret, err);
	put_int(err);
	return;
CCONVERR:
	put_int(SJ3_CannotCodeConvert);
	return;
}


void
exec_adddict(int mb_flag)
{
	TypeDicID	dicid;
	TypeGram	gram;
	int		err;
	int		err1;
	DICTL		*dl;

	dicid = get_int();
	err  = get_nstring(buf1, sizeof(buf1));
	err1 = get_nstring(buf2, sizeof(buf2));
	gram = get_int();

	if (!work_base)
		longjmp(error_ret, SJ3_NotConnected);
	if (err)
		longjmp(error_ret, SJ3_BadYomiString);
	if (err1)
		longjmp(error_ret, SJ3_BadKanjiString);

	for (dl = dictlist ; dl ; dl = dl -> next)
		if (dl -> dict -> dicid == dicid)
			break;
	if (!dl)
		longjmp(error_ret, SJ3_NoSuchDict);
	if (dl -> dict -> maxunit == 0)
		longjmp(error_ret, SJ3_ReadOnlyDict);
	if (is_dict_locked((DictFile *)(dl -> dict)))
		longjmp(error_ret, SJ3_DictLocked);

	debug_out(2, "adddic %s:%s:%d ", buf1, buf2, gram);
	seldict(dicid);
	if (mb_flag == SJIS_PROTO) {
		if (sj3_str_sjistoeuc(buf3, sizeof(buf3), buf1,
				       cur_cli->def_char, &defuse) < 0)
		        goto CCONVERR;
		if (sj3_str_sjistoeuc(buf4, sizeof(buf4), buf2,
				       cur_cli->def_char, &defuse) < 0)
		        goto CCONVERR;
		err = adddic(buf3, buf4, gram);
	} else {
		err = adddic(buf1, buf2, gram);
	}

	if (err & AD_NOTUSERDICT)	err = SJ3_ReadOnlyDict;
	else if (err & AD_BADYOMI)	err = SJ3_BadYomiString;
	else if (err & AD_BADKANJI)	err = SJ3_BadKanjiString;
	else if (err & AD_BADHINSI)	err = SJ3_BadHinsiCode;
	else if (err & AD_AREXIST)	err = SJ3_AlreadyExistWord;
	else if (err & AD_OVFLWDOUBLK)	err = SJ3_NoMoreDouonWord;
	else if (err & AD_OVFLWDOUNUM)	err = SJ3_NoMoreDouonWord;
	else if (err & AD_OVFLWUSRDIC)	err = SJ3_NoMoreUserDict;
	else if (err & AD_OVFLWINDEX)	err = SJ3_NoMoreIndexBlock;
	else if (err)			err = SJ3_AddDictFailed;
	else				err = SJ3_NormalEnd;
	if (err != SJ3_NormalEnd)
		longjmp(error_ret, err);
	put_int(err);
	return;
CCONVERR:
        put_int(SJ3_CannotCodeConvert);
        return;
}

void
exec_deldict(int mb_flag)
{
	TypeDicID	dicid;
	TypeGram	gram;
	int		err;
	int		err1;
	DICTL		*dl;

	dicid = get_int();
	err  = get_nstring(buf1, sizeof(buf1));
	err1 = get_nstring(buf2, sizeof(buf2));
	gram = get_int();

	if (!work_base)
		longjmp(error_ret, SJ3_NotConnected);
	if (err)
		longjmp(error_ret, SJ3_BadYomiString);
	if (err1)
		longjmp(error_ret, SJ3_BadKanjiString);

	for (dl = dictlist ; dl ; dl = dl -> next)
		if (dl -> dict -> dicid == dicid)
			break;
	if (!dl)
		longjmp(error_ret, SJ3_NoSuchDict);
	if (dl -> dict -> maxunit == 0)
		longjmp(error_ret, SJ3_ReadOnlyDict);
	if (is_dict_locked((DictFile *)(dl -> dict)))
		longjmp(error_ret, SJ3_DictLocked);

	seldict(dicid);
	if (mb_flag == SJIS_PROTO) {
		if (sj3_str_sjistoeuc(buf3, sizeof(buf3), buf1,
				       cur_cli->def_char, &defuse) < 0)
		        goto CCONVERR;
		if (sj3_str_sjistoeuc(buf4, sizeof(buf4), buf2,
				       cur_cli->def_char, &defuse) < 0)
		        goto CCONVERR;
		err = deldic(buf3, buf4, gram);
	} else {
		err = deldic(buf1, buf2, gram);
	}

	if (err & AD_NOTUSERDICT)	err = SJ3_ReadOnlyDict;
	else if (err & AD_BADYOMI)	err = SJ3_BadYomiString;
	else if (err & AD_BADKANJI)	err = SJ3_BadKanjiString;
	else if (err & AD_BADHINSI)	err = SJ3_BadHinsiCode;
	else if (err & AD_NOMIDASI)	err = SJ3_NoSuchWord;
	else if (err & AD_NOHINSI)	err = SJ3_NoSuchWord;
	else if (err & AD_NOKANJI)	err = SJ3_NoSuchWord;
	else if (err)			err = SJ3_DelDictFailed;
	else				err = SJ3_NormalEnd;
	if (err != SJ3_NormalEnd)
		longjmp(error_ret, err);
	put_int(err);
	return;
CCONVERR:
        put_int(SJ3_CannotCodeConvert);
        return;
}


void
exec_getdict(int mb_flag)
{
	TypeDicID	dicid;
	int		err, buf_size, l;
	DICTL		*dl;
	u_char		*ptr, *q;

	dicid = get_int();

	if (!work_base)
		longjmp(error_ret, SJ3_NotConnected);

	for (dl = dictlist ; dl ; dl = dl -> next)
		if (dl -> dict -> dicid == dicid)
			break;
	if (!dl)
		longjmp(error_ret, SJ3_NoSuchDict);

	lock_check_for_read();

	seldict(dicid);
	err = getusr(kbuf);
	if (err) {
		if (mb_flag == SJIS_PROTO) {
			buf_size = sizeof(skbuf);
			ptr = kbuf;
			q = skbuf;
			l = sj3_str_euctosjis(q, buf_size, ptr,
					      cur_cli->def_char, &defuse);
			if (l < 0)
				goto CCONVERR;
			ptr += strlen(ptr) + 1;
			q += l + 1;
			buf_size -= l + 1;
			l = sj3_str_euctosjis(q, buf_size, ptr,
					      cur_cli->def_char, &defuse2);
			if (l < 0)
				goto CCONVERR;
			if (defuse || defuse2) {
				while ((defuse || defuse2) && err) {
					ptr = kbuf;
					q = skbuf;
					buf_size = sizeof(skbuf);
					err = nextusr(kbuf);
					if (!err)
						break;
					l = sj3_str_euctosjis(q, buf_size, ptr,
							  cur_cli->def_char,
							  &defuse);
					if (l < 0)
						goto CCONVERR;
					ptr += strlen(ptr) + 1;
					q += l + 1;
					buf_size -= l + 1;
					l = sj3_str_euctosjis(q, buf_size, ptr,
							  cur_cli->def_char,
							  &defuse2);
					if (l < 0)
						goto CCONVERR;
				}
				if (!err) {
					longjmp(error_ret, SJ3_NoMoreDictData);
				}
			}
			ptr += strlen(ptr) + 1;
			q += l + 1;
			*q = *ptr;
			ptr = skbuf;
		} else {
			ptr = kbuf;
		}

		put_int(SJ3_NormalEnd);
		ptr = put_string(ptr);
		ptr = put_string(ptr);
		put_int(*ptr);
	}
	else
		longjmp(error_ret, SJ3_NoMoreDictData);
	return;
CCONVERR:
        put_int(SJ3_CannotCodeConvert);
        return;
}

void
exec_nextdict(int mb_flag)
{
	TypeDicID	dicid;
	int		err, buf_size, l;
	DICTL		*dl;
	u_char		*ptr, *q;

	dicid = get_int();

	if (!work_base)
		longjmp(error_ret, SJ3_NotConnected);

	for (dl = dictlist ; dl ; dl = dl -> next)
		if (dl -> dict -> dicid == dicid)
			break;
	if (!dl)
		longjmp(error_ret, SJ3_NoSuchDict);


	lock_check_for_read();

	seldict(dicid);
	err = nextusr(kbuf);
	if (err) {
		if (mb_flag == SJIS_PROTO) {
			buf_size = sizeof(skbuf);
			ptr = kbuf;
			q = skbuf;
			l = sj3_str_euctosjis(q, buf_size, ptr, cur_cli->def_char, 
					  &defuse);
			if (l < 0)
				goto CCONVERR;
			ptr += strlen(ptr) + 1;
			q += l + 1;
			buf_size -= l + 1;
			l = sj3_str_euctosjis(q, buf_size, ptr, cur_cli->def_char, 
					  &defuse2);
			if (l < 0)
				goto CCONVERR;
			if (defuse || defuse2) {
				while ((defuse || defuse2) && err) {
					ptr = kbuf;
					q = skbuf;
					buf_size = sizeof(skbuf);
					err = nextusr(kbuf);
					if (!err)
						break;
					l = sj3_str_euctosjis(q, buf_size, ptr,
							  cur_cli->def_char,
							  &defuse);
					if (l < 0)
						goto CCONVERR;
					ptr += strlen(ptr) + 1;
					q += l + 1;
					buf_size -= l + 1;
					l = sj3_str_euctosjis(q, buf_size, ptr,
							  cur_cli->def_char,
							  &defuse2);
					if (l < 0)
						goto CCONVERR;
				}
				if (!err) {
					longjmp(error_ret, SJ3_NoMoreDictData);
				}
			}
			ptr += strlen(ptr) + 1;
			q += l + 1;
			*q = *ptr;
			ptr = skbuf;
		} else {
			ptr = kbuf;
		}

		put_int(SJ3_NormalEnd);
		ptr = put_string(ptr);
		ptr = put_string(ptr);
		put_int(*ptr);
	}
	else
		longjmp(error_ret, SJ3_NoMoreDictData);
	return;
CCONVERR:
        put_int(SJ3_CannotCodeConvert);
        return;
}

void
exec_prevdict(int mb_flag)
{
	TypeDicID	dicid;
	int		err, buf_size, l;
	DICTL		*dl;
	u_char		*ptr, *q;

	dicid = get_int();

	if (!work_base)
		longjmp(error_ret, SJ3_NotConnected);

	for (dl = dictlist ; dl ; dl = dl -> next)
		if (dl -> dict -> dicid == dicid)
			break;
	if (!dl)
		longjmp(error_ret, SJ3_NoSuchDict);

	lock_check_for_read();

	seldict(dicid);
	err = prevusr(kbuf);
	if (err) {
		if (mb_flag == SJIS_PROTO) {
			buf_size = sizeof(skbuf);
			ptr = kbuf;
			q = skbuf;
			l = sj3_str_euctosjis(q, buf_size, ptr,
					      cur_cli->def_char, &defuse);
			if (l < 0)
				goto CCONVERR;
			ptr += strlen(ptr) + 1;
			q += l + 1;
			buf_size -= l + 1;
			l = sj3_str_euctosjis(q, buf_size, ptr,
					      cur_cli->def_char, &defuse2);
			if (l < 0)
				goto CCONVERR;
			if (defuse || defuse2) {
				while ((defuse || defuse2) && err) {
					ptr = kbuf;
					q = skbuf;
					buf_size = sizeof(skbuf);
					err = prevusr(kbuf);
					if (!err)
						break;
					l = sj3_str_euctosjis(q, buf_size, ptr,
							  cur_cli->def_char,
							  &defuse);
					if (l < 0)
						goto CCONVERR;
					ptr += strlen(ptr) + 1;
					q += l + 1;
					buf_size -= l + 1;
					l = sj3_str_euctosjis(q, buf_size, ptr,
							  cur_cli->def_char,
							  &defuse2);
					if (l < 0)
						goto CCONVERR;
				}
				if (!err) {
					longjmp(error_ret, SJ3_NoMoreDictData);
				}
			}
			ptr += strlen(ptr) + 1;
			q += l + 1;
			*q = *ptr;
			ptr = skbuf;
		} else {
			ptr = kbuf;
		}

		put_int(SJ3_NormalEnd);
		ptr = put_string(ptr);
		ptr = put_string(ptr);
		put_int(*ptr);
	}
	else
		longjmp(error_ret, SJ3_NoMoreDictData);
	return;
CCONVERR:
        put_int(SJ3_CannotCodeConvert);
        return;
}


void
exec_makedict()
{
	struct stat    sbuf;
	char	path[MAXPATHLEN];
	int	err;
	int	idxlen;
	int	seglen;
	int	segnum;

	err = get_nstring(path, sizeof(path));
	idxlen = get_int();
	seglen = get_int();
	segnum = get_int();
	if (err)
		longjmp(error_ret, SJ3_TooLongParameter);
	if (make_full_path(path, sizeof(path)))
		longjmp(error_ret, SJ3_TooLongParameter);

	if ((stat(path, &sbuf) != ERROR) || (errno != ENOENT)){
		longjmp(error_ret, SJ3_IllegalDictFile);
	}
	err = makedict(path, idxlen, seglen, segnum);
	if (err != SJ3_NormalEnd)
		longjmp(error_ret, err);
	chmod(path, (file_mode & MODE_MASK));

	put_int(err);
}


void
exec_makestdy()
{
	struct stat    sbuf;
	char	path[MAXPATHLEN];
	int	err;
	int	stynum;
	int	clstep;
	int	cllen;

	err = get_nstring(path, sizeof(path));
	stynum = get_int();
	clstep = get_int();
	cllen = get_int();
	if (err)
		longjmp(error_ret, SJ3_TooLongParameter);
	if (make_full_path(path, sizeof(path)))
		longjmp(error_ret, SJ3_TooLongParameter);

	if ((stat(path, &sbuf) != ERROR) || (errno != ENOENT)){
		longjmp(error_ret, SJ3_IllegalStdyFile);
	}
	err = makestdy(path, stynum, clstep, cllen);
	if (err != SJ3_NormalEnd)
		longjmp(error_ret, err);
	chmod(path, (file_mode & MODE_MASK));

	put_int(err);
}


void
exec_access()
{
	char	path[MAXPATHLEN];
	int	mode;
	int	i;

	i = get_nstring(path, sizeof(path));
	mode = get_int();
	if (i)
		longjmp(error_ret, SJ3_TooLongParameter);
	if (make_full_path(path, sizeof(path)))
		longjmp(error_ret, SJ3_TooLongParameter);

	put_int(access(path, mode));
}


void
exec_makedir()
{
	char	path[MAXPATHLEN];
	int	i;

	i = get_nstring(path, sizeof(path));
	if (i)
		longjmp(error_ret, SJ3_TooLongParameter);
	if (make_full_path(path, sizeof(path)))
		longjmp(error_ret, SJ3_TooLongParameter);

	if (mkdir(path, dir_mode)) {
		if (errno == EEXIST)
			longjmp(error_ret, SJ3_DirAlreadyExist);
		else
			longjmp(error_ret, SJ3_CannotCreateDir);
	}
	chmod(path, (dir_mode & MODE_MASK));

	put_int(SJ3_NormalEnd);
}


void
exec_who()
{
	int	i;
	Client	*cli;

	i = 0;
	cli = client;
	while ((i <= client_num) && cli) {
		i++;
		cli = cli->next;
	}
	if (i < client_num) {
		put_int(i);
	} else {
		put_int(client_num);
	}
	for (i = 0, cli = client ; (i < client_num) && cli ; i++, cli = cli->next) {
		put_int(cli -> fd);
		put_string(cli -> host);
		put_string(cli -> user);
		put_string(cli -> prog);
	}
}


void
exec_kill()
{
	put_int(SJ3_NormalEnd);
	put_flush();

	server_terminate();
}
void
exec_quit()
{
	if (client_num > 1)
		longjmp(error_ret, SJ3_UserConnected);
	exec_kill();
}


void
exec_version()
{
	static	char	*Version = "version : ";
	static	char	*TimeStamp = "times-stamp : ";

	put_int(SJ3_NormalEnd);
	put_ndata(Version, strlen(Version));
	put_string(version_number);
	put_ndata(TimeStamp, strlen(TimeStamp));
	put_string(time_stamp);
	put_string(NULL);
}


void
exec_dictpass()
{
	TypeDicID	dicid;
	char		buf[PASSWDLEN + 1];
	int		err;
	DICTL		*dl;

	dicid = get_int();
	err  = get_nstring(buf, sizeof(buf));

	if (!work_base)
		longjmp(error_ret, SJ3_NotConnected);
	if (err)
		longjmp(error_ret, SJ3_TooLongPasswd);

	for (dl = dictlist ; dl ; dl = dl -> next)
		if (dl -> dict -> dicid == dicid)
			break;
	if (!dl)
		longjmp(error_ret, SJ3_NoSuchDict);

	if (set_dictpass(dl -> dict, buf))
		longjmp(error_ret, serv_errno);
	put_int(SJ3_NormalEnd);
}
void
exec_dictcmnt()
{
	TypeDicID	dicid;
	char		buf[COMMENTLENGTH + 1];
	int		err;
	DICTL		*dl;

	dicid = get_int();
	err  = get_nstring(buf, sizeof(buf));

	if (!work_base)
		longjmp(error_ret, SJ3_NotConnected);
	if (err)
		longjmp(error_ret, SJ3_TooLongComment);

	for (dl = dictlist ; dl ; dl = dl -> next)
		if (dl -> dict -> dicid == dicid)
			break;
	if (!dl)
		longjmp(error_ret, SJ3_NoSuchDict);

	if (set_dictcmnt(dl -> dict, buf))
		longjmp(error_ret, serv_errno);
	put_int(SJ3_NormalEnd);
}
void
exec_stdypass()
{
	char		buf[PASSWDLEN + 1];
	int		err;

	err  = get_nstring(buf, sizeof(buf));

	if (!work_base)
		longjmp(error_ret, SJ3_NotConnected);
	if (!stdy_base)
		longjmp(error_ret, SJ3_StdyFileNotOpened);
	if (err)
		longjmp(error_ret, SJ3_TooLongPasswd);

	if (set_stdypass(buf))
		longjmp(error_ret, serv_errno);
	put_int(SJ3_NormalEnd);
}
void
exec_stdycmnt()
{
	char		buf[COMMENTLENGTH + 1];
	int		err;

	err  = get_nstring(buf, sizeof(buf));

	if (!work_base)
		longjmp(error_ret, SJ3_NotConnected);
	if (!stdy_base)
		longjmp(error_ret, SJ3_StdyFileNotOpened);
	if (err)
		longjmp(error_ret, SJ3_TooLongComment);

	if (set_stdycmnt(buf))
		longjmp(error_ret, serv_errno);
	put_int(SJ3_NormalEnd);
}


void
exec_stdypara()
{
	int	stynum, clstep, cllen;

	if (!work_base)
		longjmp(error_ret, SJ3_NotConnected);
	if (!stdy_base)
		longjmp(error_ret, SJ3_StdyFileNotOpened);

	get_stdysize(&stynum, &clstep, &cllen);
	put_int(SJ3_NormalEnd);
	put_int(stynum);
	put_int(clstep);
	put_int(cllen);
}


void
execute_cmd()
{
	int	i;

	if (i = setjmp(error_ret)) {
		debug_out(1, "%d: error code = %d", client_fd, i);
		put_int(i);
	} else {
		cur_cli = cur_client;
		work_base = cur_cli->work ? &(cur_cli->work->global) : NULL;
		stdy_base = cur_cli->stdy ? &(cur_cli->stdy->stdy) : NULL;

#define	CASE(X)	case (X): { int j=X; debug_out(1, "%d: %d\t", client_fd, j); }
		switch (i = get_int()) {

		CASE(SJ3_CONNECT)	exec_connect(); break;
		CASE(SJ3_DISCONNECT)	exec_disconnect(); break;

		CASE(SJ3_OPENDICT)	exec_opendict(); break;
		CASE(SJ3_CLOSEDICT)	exec_closedict(); break;

		CASE(SJ3_OPENSTDY)	exec_openstdy(); break;
		CASE(SJ3_CLOSESTDY)	exec_closestdy(); break;
		CASE(SJ3_STDYSIZE)	exec_stdysize(); break;

		CASE(SJ3_LOCK)		exec_lock(); break;
		CASE(SJ3_UNLOCK)	exec_unlock(); break;

		CASE(SJ3_PH2KNJ)	exec_ph2knj(SJIS_PROTO); break;
		CASE(SJ3_PH2KNJ_EUC)	exec_ph2knj(EUC_PROTO); break;

		CASE(SJ3_CL2KNJ)	exec_cl2knj(SJIS_PROTO); break;
		CASE(SJ3_CL2KNJ_EUC)	exec_cl2knj(EUC_PROTO); break;
		CASE(SJ3_NEXTCL)	exec_nextcl(SJIS_PROTO); break;
		CASE(SJ3_NEXTCL_EUC)	exec_nextcl(EUC_PROTO); break;
		CASE(SJ3_PREVCL)	exec_prevcl(SJIS_PROTO); break;
		CASE(SJ3_PREVCL_EUC)	exec_prevcl(EUC_PROTO); break;
		CASE(SJ3_CL2KNJ_CNT)	exec_cl2knj_cnt(SJIS_PROTO); break;
		CASE(SJ3_CL2KNJ_CNT_EUC)	exec_cl2knj_cnt(EUC_PROTO);
			                        break;
		CASE(SJ3_CL2KNJ_ALL)	exec_cl2knj_all(SJIS_PROTO); break;
		CASE(SJ3_CL2KNJ_ALL_EUC)	exec_cl2knj_all(EUC_PROTO);
                                                break;

		CASE(SJ3_STUDY)		exec_study(); break;
		CASE(SJ3_CLSTUDY)	exec_clstudy(SJIS_PROTO); break;
		CASE(SJ3_CLSTUDY_EUC)	exec_clstudy(EUC_PROTO); break;

		CASE(SJ3_ADDDICT)	exec_adddict(SJIS_PROTO); break;
		CASE(SJ3_ADDDICT_EUC)	exec_adddict(EUC_PROTO); break;
		CASE(SJ3_DELDICT)	exec_deldict(SJIS_PROTO); break;
		CASE(SJ3_DELDICT_EUC)	exec_deldict(EUC_PROTO); break;

		CASE(SJ3_GETDICT)	exec_getdict(SJIS_PROTO); break;
		CASE(SJ3_GETDICT_EUC)	exec_getdict(EUC_PROTO); break;
		CASE(SJ3_NEXTDICT)	exec_nextdict(SJIS_PROTO); break;
		CASE(SJ3_NEXTDICT_EUC)	exec_nextdict(EUC_PROTO); break;
		CASE(SJ3_PREVDICT)	exec_prevdict(SJIS_PROTO); break;
		CASE(SJ3_PREVDICT_EUC)	exec_prevdict(EUC_PROTO); break;

		CASE(SJ3_MAKEDICT)	exec_makedict(); break;
		CASE(SJ3_MAKESTDY)	exec_makestdy(); break;
		CASE(SJ3_ACCESS)	exec_access(); break;
		CASE(SJ3_MAKEDIR)	exec_makedir(); break;

		CASE(SJ3_WHO)		exec_who(); break;
		CASE(SJ3_QUIT)		exec_quit(); break;
		CASE(SJ3_KILL)		exec_kill(); break;
		CASE(SJ3_VERSION)	exec_version(); break;

		CASE(SJ3_DICTPASS)	exec_dictpass(); break;
		CASE(SJ3_DICTCMNT)	exec_dictcmnt(); break;
		CASE(SJ3_STDYPASS)	exec_stdypass(); break;
		CASE(SJ3_STDYCMNT)	exec_stdycmnt(); break;
		default:		warning_out("Illegal command");
					longjmp(error_ret, SJ3_IllegalCommand);
					break;
		}
#undef	CASE
		debug_out(1, "cmd end");
	}

	put_flush();
}
