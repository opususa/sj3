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

#ifndef SJ3COMPAT_H
#define SJ3COMPAT_H

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/param.h>

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 128
#endif

#ifndef MAXLOGNAME
#define MAXLOGNAME 32
#endif

#ifndef FILENAME_MAX
#define FILENAME_MAX 1024
#endif

/* bsd-asprintf.c */
#ifndef HAVE_VASPRINTF
#define vasprintf	sj3_vasprintf
int sj3_vasprintf(char **ret, const char *format, va_list ap);
#endif
#ifndef HAVE_ASPRINTF
#define asprintf	sj3_asprintf
int sj3_asprintf(char **ret, const char *format, ...);
#endif

/* getopt.c */
#ifndef HAVE_GETOPT_OPTRESET
# undef getopt
# undef opterr
# undef optind
# undef optopt
# undef optreset
# undef optarg
# define getopt(ac, av, o)  BSDgetopt(ac, av, o)
# define opterr             BSDopterr
# define optind             BSDoptind
# define optopt             BSDoptopt
# define optreset           BSDoptreset
# define optarg             BSDoptarg
#endif

/* bsd-getpeereid.c */
#ifndef HAVE_GETPEEREID
#define getpeereid	sj3_getpeereid
int sj3_getpeereid(int s, uid_t *euid, gid_t *egid);
#endif

/* bsd-misc.c */
#if !defined(HAVE_STRERROR) && defined(HAVE_SYS_ERRLIST) && defined(HAVE_SYS_NERR)
#define strerror	sj3_strerror
const char *sj3_strerror(int e);
#endif
#ifndef HAVE_STRDUP
#define strdup	sj3_strdup
char *sj3_strdup(const char *str);
#endif

/* bsd-snprintf.c */
#ifndef HAVE_VSNPRINTF
#define	vsnprintf	sj3_vsnprintf
int sj3_vsnprintf(char *str, size_t size, const char *format, va_list ap);
#endif
#ifndef HAVE_SNPRINTF
#define	snprintf	sj3_snprintf
int sj3_snprintf(char *str, size_t size, const char *format, ...);
#endif

/* daemon.c */
#ifndef HAVE_DAEMON
#define daemon	sj3_daemon
int sj3_daemon(int, int);
#endif

/* #include "fake-rfc2553.h" */

/* inet_ntoa.c */
#if defined(BROKEN_INET_NTOA) || !defined(HAVE_INET_NTOA)
#define inet_ntoa	sj3_inet_ntoa
char* sj3_inet_ntoa(struct in_addr in);
#endif

/* setproctitle.c */
#ifndef HAVE_SETPROCTITLE
#define setproctitle	sj3_setproctitle
void setproctitle(const char *fmt, ...);
#endif

/* strlcat.c */
#ifndef HAVE_STRLCAT
#define	strlcat		sj3_strlcat
size_t sj3_strlcat(char *, const char *, size_t);
#endif

/* strlcpy.c */
#ifndef HAVE_STRLCPY
#define	strlcpy		sj3_strlcpy
size_t sj3_strlcpy(char *, const char *, size_t);
#endif

/* vis.c */
#ifndef HAVE_STRVISX
#define strvisx		sj3strvisx
int strvisx(char *, const char *, size_t, int);
#endif

#endif /* SJ3COMPAT_H */
