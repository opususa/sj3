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
 * $SonyRCSfile: comuni.c,v $  
 * $SonyRevision: 1.4 $ 
 * $SonyDate: 1994/11/16 07:34:05 $
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <setjmp.h>
#include <errno.h>
#include <sys/stat.h>

#include "config.h"

#ifdef USE_LIBWRAP
#include <tcpd.h>
#include <syslog.h>

int allow_severity = LOG_INFO;
int deny_severity = LOG_WARNING;
#endif

#include "sj_kcnv.h"
#include "sj_kanakan.h"
#include "sj3serv.h"

#ifndef SOMAXCONN
# define SOMAXCONN 5
#endif

#define MAX_LISTEN_SOCKS 16

int client_num = 0;
Client *client = NULL;
Client *cur_client;

int maxfds = 0;
fd_set fds_org;
jmp_buf client_dead;
int client_fd;

static int fd_unix = -1;

static char putbuf[BUFSIZ];
static char getbuf[BUFSIZ];
static int putpos = 0;
static int buflen = 0;
static int getpos = 0;


void
socket_init()
{
	FD_ZERO(&fds_org);
	client_num = 0;
	maxfds = 0;
}

static	void
set_fd(int fd)
{
	FD_SET(fd, &fds_org);
	if (fd >= maxfds)
		maxfds = fd + 1;
}

static void
clr_fd(int fd)
{
	FD_CLR(fd, &fds_org);
	if (maxfds == fd + 1) {
		while (--fd >= 0) {
			if (FD_ISSET(fd, &fds_org))
				break;
		}
		maxfds = fd + 1;
	}
}

void
open_af_unix()
{
	struct sockaddr_un sunix;

	unlink(domain_socket_name);

	memset((char *)&sunix, '\0', sizeof(sunix));
	sunix.sun_family = AF_UNIX;
	strlcpy(sunix.sun_path, domain_socket_name, sizeof(sunix.sun_path));

	if ((fd_unix = socket(AF_UNIX, SOCK_STREAM, 0)) == ERROR) {
		unlink(domain_socket_name);
		fprintf(stderr, "Can't create AF_UNIX socket at '%s'\n", domain_socket_name);
		exit(1);
	}

	if (bind(fd_unix, (struct sockaddr *)&sunix, strlen(sunix.sun_path) + 2) == ERROR) {
		if (errno == EADDRINUSE)
			fprintf(stderr, "Port '%s' is already in use\n",
				domain_socket_name);
		else
			fprintf(stderr, "Can't bind AF_UNIX socket at '%s'\n", domain_socket_name);
		shutdown(fd_unix, 2);
		close(fd_unix);
		unlink(domain_socket_name);
		exit(1);
	}

	if (listen(fd_unix, SOMAXCONN) == ERROR) {
		shutdown(fd_unix, 2);
		unlink(domain_socket_name);
		close(fd_unix);
		fprintf(stderr, "Can't listen AF_UNIX socket\n");
		exit(1);
	}
	chmod(domain_socket_name,
	    S_IRUSR | S_IWUSR | S_IXUSR |
	    S_IRGRP | S_IWGRP | S_IXGRP |
	    S_IROTH | S_IWOTH | S_IXOTH);
	set_fd(fd_unix);
}

void
open_socket()
{
	open_af_unix();
}

static int
connect_af_unix()
{
	struct sockaddr_un	sunix;
	int	i = sizeof(sunix);
	int	fd;

	if ((fd = accept(fd_unix, (struct sockaddr *)&sunix, &i)) == ERROR)
		warning_out("Can't accept AF_UNIX socket");
	return fd;
}

static void
close_af_unix()
{
	struct sockaddr_un sunix;
	int true = 1;
	int i;

	ioctl(fd_unix, FIONBIO, &true);
	for ( ; ; ) {
		i = sizeof(sunix);
		if (accept(fd_unix, (struct sockaddr *)&sunix, &i) < 0)
			break;
	}
	shutdown(fd_unix, 2);
	close(fd_unix);
	unlink(domain_socket_name);
	clr_fd(fd_unix);
}

void
close_socket()
{
	close_af_unix();
}

static void
connect_client(fd_set *readfds)
{
	int fd = -1;
	Client *client_tmp;
	int i;

	if (fd < 0 && FD_ISSET(fd_unix, readfds))
		fd = connect_af_unix();
	if (fd == ERROR)
		return;

	if (client_num >= MAXCLIENTNUM || client_num >= max_client) {
		warning_out("No more client");
	}
	else {
		debug_out(1, "client create(%d)\n", fd);
		logging_out("connect to client on %d", fd);

		if (client_tmp = (Client *)malloc(sizeof(Client))) {
			memset(client_tmp, 0, sizeof(Client));
			client_tmp->fd = fd;
			client_tmp->next = client;
			client = client_tmp;
			client_num++;
			set_fd(fd);
			return;
		}
		warning_out("No more client(Can't alloc)");
	}
	shutdown(fd, 2);
	close(fd);
}



static Client *
disconnect_client(Client *cp)
{
	WorkArea *wp;
	StdyFile *sp;
	int fd;
	Client  *cl_tmp, *cl_before;

	if (sp = cp->stdy) closestdy(sp);
	if (wp = cp->work) free_workarea(wp);

	debug_out(1, "client dead(%d)\n", cp->fd);
	logging_out("disconnect from client on %d", cp->fd);

	shutdown(fd = cp->fd, 2);
	close(fd);
	clr_fd(fd);

	if (cp == client) {
		client = cp->next;
		free(cp);
		cp = client;
	} else {
		cl_before = client;
		cl_tmp = client->next;
		while (cl_tmp) {
			if (cl_tmp == cp) {
				cl_before->next = cp->next;
				free(cp);
				break;
			}
			cl_before = cl_tmp;
			cl_tmp = cl_tmp->next;
		}
		cp = cl_before->next;
	}
  	client_num--;
	return cp;
}

void
communicate()
{
	signal(SIGPIPE, SIG_IGN);

	for ( ; ; ) {
		fd_set readfds;

		readfds = fds_org;

		while (select(maxfds, &readfds, NULL, NULL, NULL) == ERROR) {
			if (errno != EINTR)
				error_out("select");
			errno = 0;
		}

		for (cur_client = client ; cur_client; ) {
			if (setjmp(client_dead)) {
				cur_client = disconnect_client(cur_client);
				continue;
			}
			if (FD_ISSET(cur_client->fd, &readfds)) {
				client_fd = cur_client->fd;
				putpos = buflen = getpos = 0;
				execute_cmd();
			}
			cur_client = cur_client->next;
		}
		connect_client(&readfds);
	}
}


void
put_flush()
{
	int len;
	int i;
	char *p;

	for (len = putpos, p = putbuf ; len > 0 ; ) {
		if ((i = write(client_fd, p, len)) == ERROR) {
			longjmp(client_dead, -1);
		}
		len -= i;
		p += i;
	}
	putpos = 0;
}
void
put_byte(int c)
{
	putbuf[putpos++] = c;
	if (putpos >= sizeof(putbuf))
		put_flush();
}
void
put_work(int c)
{
	put_byte(c >> 8);
	put_byte(c & 0xff);
}
void
put_int(int c)
{
	put_byte(c >> (8 * 3));
	put_byte(c >> (8 * 2));
	put_byte(c >> (8 * 1));
	put_byte(c);
}
u_char *
put_string(u_char *p)
{
	if (p) {
		do {
			put_byte(*p);
		} while (*p++);
	}
	else
		put_byte(0);
	return p;
}
u_char *
put_ndata(u_char *p, int n)
{
	while (n-- > 0)
		put_byte(p ? *p++ : 0);
	return p;
}



void
get_buf()
{
	int i;

	for ( ; ; ) {
		if ((i = read(client_fd, getbuf, sizeof(getbuf))) > 0)
			break;
		if (i == ERROR) {
			if (errno == EINTR)
				continue;
			if (errno == EWOULDBLOCK)
				continue;
		}
		longjmp(client_dead, -1);
	}
	buflen = i;
	getpos = 0;
}
int
get_byte()
{
	if (getpos >= buflen) get_buf();
	return (getbuf[getpos++] & 0xff);
}
int
get_word()
{
	int i;

	i = get_byte();
	return ((i << 8) | get_byte());
}
int
get_int()
{
	int i0;
	int i1;
	int i2;

	i0 = get_byte();
	i1 = get_byte();
	i2 = get_byte();
	return((i0 << (8*3)) | (i1 << (8*2)) | (i2 << (8*1)) | get_byte());
}
int
get_nstring(u_char *p, int n)
{
	int c;

	do {
		c = get_byte();
		if (n-- > 0)
			*p++ = c;
	} while (c);

	return (n < 0) ? ERROR : 0;
}
u_char *
get_ndata(u_char *p, int n)
{
	while (n-- > 0)
		*p++ = get_byte();
	return p;
}
