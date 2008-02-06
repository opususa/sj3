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
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <sys/param.h>
#if defined(HAVE_STRVISX) && defined(HAVE_VIS_H)
# include <vis.h>
#endif
#include "sys-queue.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "Const.h"
#include "sj3proxy.h"
#include "sj3lua.h"
#include "atomicio.h"

int chroot_enable = -1;
char address_family_str[256];
int address_family = AF_UNSPEC;
char chuser_name[BUFSIZ];
char chroot_dir[MAXPATHLEN];
char socket_file[MAXPATHLEN];
char host_name[NI_MAXHOST];
char service_name[NI_MAXSERV];

static int trace_enable = 0;

struct listen_addr {
	TAILQ_ENTRY(listen_addr) entry;
	struct sockaddr_storage sa;
	int fd;
} *listen_addrs_ptr;
TAILQ_HEAD(listen_addrs, listen_addr) listen_addrs;

static void
close_listen_socks(void)
{
	struct listen_addr *np = listen_addrs_ptr;

	while ((np = TAILQ_FIRST(&listen_addrs)) != NULL) {
		close(np->fd);
		TAILQ_REMOVE(&listen_addrs, np, entry);
	}
}

/*
 * SIGCHLD handler.  This is called whenever a child dies.  This will then
 * reap any zombies left by exited children.
 */
static void
main_sigchld_handler(int sig)
{
	int save_errno = errno;
	pid_t pid;
	int status;

	while ((pid = waitpid(-1, &status, WNOHANG)) > 0 ||
	    (pid < 0 && errno == EINTR))
                ;

	signal(SIGCHLD, main_sigchld_handler);
	errno = save_errno;
}

static void
readwrite(int inet_fd)
{
	struct sockaddr_un sunix;
	int sj3_fd;
	char buf[BUFSIZ];
	struct pollfd pfd[2];
	int nfds;
	ssize_t readlen;

	if (access(socket_file, R_OK | W_OK) == -1) {
		fprintf(stderr, "Can't find AF_UNIX socket\n");
		return;
	}

	memset((char *)&sunix, 0, sizeof(sunix));
        sunix.sun_family = AF_UNIX;
	strlcpy(sunix.sun_path, socket_file, sizeof(sunix.sun_path));

	if ((sj3_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		fprintf(stderr, "Can't create AF_UNIX socket\n");
		return;
	}
	fcntl(sj3_fd, F_SETFD, 1);
	if (connect(sj3_fd, (struct sockaddr *)&sunix, SUN_LEN(&sunix)) < 0) {
		fprintf(stderr, "Can't connect AF_UNIX socket\n");
		close(sj3_fd);
		return;
	}

	pfd[0].fd = inet_fd;
	pfd[0].events = POLLIN;

	pfd[1].fd = sj3_fd;
	pfd[1].events = POLLIN;

	for ( ; ; ){
		if ((nfds = poll(pfd, 2, -1)) < 0) {
			close(inet_fd);
			close(sj3_fd);
			fprintf(stderr, "Polling Error");
			return;
		}

		if (nfds == 0)
			return;

		if (pfd[0].revents & POLLIN) {
			memset(buf, 0, sizeof(buf));
			if ((readlen = read(inet_fd, buf, sizeof(buf))) == -1) {
				close(sj3_fd);
				return;
			} else if (readlen == 0) {
				shutdown(inet_fd, SHUT_RD);
				return;
			} else {
				if (trace_enable) {
					char fmtbuf[BUFSIZ * 4 + 1];
					strvisx(fmtbuf, buf, readlen, VIS_CSTYLE|VIS_NL|VIS_TAB|VIS_OCTAL);
					fmtbuf[BUFSIZ * 4] = 0;
					printf("i< %s\n", fmtbuf);
				}
				if (atomicio(vwrite, sj3_fd, buf, readlen) != readlen)
					return;
			}
		}
		if (pfd[1].revents & POLLIN) {
			memset(buf, 0, sizeof(buf));
			if ((readlen = read(sj3_fd, buf, BUFSIZ)) == -1) {
				close(sj3_fd);
				return;
			} else if (readlen == 0) {
				shutdown(inet_fd, SHUT_WR);
				return;
			} else {
				if (trace_enable) {
					char fmtbuf[BUFSIZ * 4 + 1];
					strvisx(fmtbuf, buf, readlen, VIS_CSTYLE|VIS_NL|VIS_TAB|VIS_OCTAL);
					fmtbuf[BUFSIZ * 4] = 0;
					printf("o> %s\n", fmtbuf);
				}
				if (atomicio(vwrite, inet_fd, buf, readlen) != readlen)
                                        return;
			}
		}
	}
}

static int
set_server(lua_State *lstate)
{
	if (!sj3lua_check_table(lstate))
		return 0;
	lua2sj_string(lstate,  "user",          1, SJ3OWNER,      chuser_name,  sizeof(chuser_name));
	lua2sj_boolean(lstate, "chroot",        1, CHROOTFLAG,    &chroot_enable);
	lua2sj_string(lstate,  "chroot_dir",    1, SOCKDIR,       chroot_dir,   sizeof(chroot_dir));
	lua2sj_string(lstate,  "socket_file",   1, SOCKETFILE,    socket_file,  sizeof(socket_file));
	lua2sj_string(lstate,  "host_name",     1, LOCALHOST,     host_name,    sizeof(host_name));
	lua2sj_string(lstate,  "address_famiy", 1, ADDRESSFAMILY, address_family_str, sizeof(address_family_str));
	lua2sj_string(lstate,  "service_name",  1, PORTNUMBER,    service_name, sizeof(service_name));

	return 0;
}

static int
get_server(lua_State *lstate)
{
	sj2lua_string(lstate,  "user",          chuser_name);
	sj2lua_boolean(lstate, "chroot",        chroot_enable);
	sj2lua_string(lstate,  "chroot_dir",    chroot_dir);
	sj2lua_string(lstate,  "socket_file",   socket_file);
	sj2lua_string(lstate,  "host_name",     host_name);
	sj2lua_string(lstate,  "address_famiy", address_family_str);
	sj2lua_string(lstate,  "service_name",  service_name);

        return 1;
}

int
main(int argc, char *argv[])
{
	struct sockaddr *sa;
	struct addrinfo hints;
	struct addrinfo *res, *res0;
	int listen_sock, maxfd;
	socklen_t fromlen;
	int inet_fd = -1;
	int error;
	u_int8_t salen;
	char ntop[NI_MAXHOST], strport[NI_MAXSERV];
	int fdsetsz;
	int on = 1;
	fd_set *fdset;
	struct sockaddr_storage from;
	int pid;
	int errflg = 0;
	int c;
	int daemon_disable = 0;
	lua_State *lua_state = NULL;
	int opt_inet4 = 0, opt_inet6 = 0;
	char *config_file = SJ3PROXYCFG;

	while ((c = getopt(argc, argv, "46c:dt")) != EOF) {
		switch (c) {
		case '4':
			opt_inet4 = 1;
			break;
		case '6':
			opt_inet6 = 1;
			break;
		case 'c':
			config_file = optarg;
			break;
		case 'd':
			daemon_disable = 1;
			break;
		case 't':
			trace_enable = 1;
			break;
		case '?':
		default:
			errflg++;
			break;
		}
	}

	if (errflg || optind < argc) {
		fprintf(stderr, "Usage: sj3_proxy [-46d] [-c configfile]\n");
		exit(1);
	}

	if ((lua_state = luaL_newstate()) == NULL) {
		fprintf(stderr, "Can't setup lua");
		exit(1);
	}

	luaL_openlibs(lua_state);

	/* sj3.functions */
	lua_newtable(lua_state);
	set_luafunction(lua_state, "set_server", set_server);
	set_luafunction(lua_state, "get_server", get_server);
	lua_setglobal(lua_state, "sj3proxy");

	if (luaL_dofile(lua_state, config_file) != 0) {
		fprintf(stderr, "%s\n", lua_tostring(lua_state, -1));
		exit(255);
	}


	if (strcmp(address_family_str, "unspec") == 0)
		address_family = AF_UNSPEC;
	if (opt_inet4 || strcmp(address_family_str, "inet") == 0)
		address_family = AF_INET;
	else if (opt_inet6 || strcmp(address_family_str, "inet6") == 0)
		address_family = AF_INET6;

	if (!daemon_disable)
		daemon(0, 1);


	TAILQ_INIT(&listen_addrs);

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = address_family;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	error = getaddrinfo(host_name, service_name, &hints, &res);
	if (error) {
		perror(gai_strerror(error));
		exit(1);
	}

	res0 = res;

	for ( ; res; res = res->ai_next) {
		sa = res->ai_addr;
		salen = res->ai_addrlen;

		if (res->ai_family != AF_INET && res->ai_family != AF_INET6)
			continue;

		if (getnameinfo(sa, salen,
			ntop, sizeof(ntop), strport, sizeof(strport),
			NI_NUMERICHOST|NI_NUMERICSERV) != 0) {
			fprintf(stderr, "getnameinfo failed\n");
			continue;
		}
		/* Create socket for listening. */
		listen_sock = socket(res->ai_family, res->ai_socktype,
		    res->ai_protocol);
		if (listen_sock < 0) {
			/* kernel may not support ipv6 */
			fprintf(stderr, "socket: %.100s\n", strerror(errno));
			continue;
		}

		/*
		 * Set socket options.
		 * Allow local port reuse in TIME_WAIT.
		 */
		if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR,
			&on, sizeof(on)) == -1)
			fprintf(stderr, "setsockopt SO_REUSEADDR: %s\n", strerror(errno));

		if (bind(listen_sock, sa, salen) < 0) {
			fprintf(stderr, "Bind to port %s on %s failed: %.200s.\n",
			    strport, ntop, strerror(errno));
			close(listen_sock);
			continue;
		}

		/* Start listening on the port. */
		if (listen(listen_sock, 127) < 0) {
			fprintf(stderr, "listen: %.100s\n", strerror(errno));
			exit(1);
		}

		if ((listen_addrs_ptr = calloc(1, sizeof(struct listen_addr))) ==
		    NULL) {
			fprintf(stderr, "setup_listeners calloc");
			exit(1);
		}

		memcpy(&listen_addrs_ptr->sa, sa, SA_LEN(sa));
		listen_addrs_ptr->fd = listen_sock;
		TAILQ_INSERT_TAIL(&listen_addrs, listen_addrs_ptr, entry);
	}

	freeaddrinfo(res0);

	/* drop privilege */
	set_priv(NULL);

	/* Arrange SIGCHLD to be caught. */
	signal(SIGCHLD, main_sigchld_handler);

	/* setup fd set for listen */
	fdset = NULL;
	maxfd = 0;
	TAILQ_FOREACH(listen_addrs_ptr, &listen_addrs, entry) {
		if (listen_addrs_ptr->fd > maxfd)
			maxfd = listen_addrs_ptr->fd;
	}

	/*
	 * Stay listening for connections until the system crashes or
	 * the daemon is killed with a signal.
	 */
	for ( ; ; ) {
		int ret;

		if (fdset != NULL)
			free(fdset);
		fdsetsz = howmany(maxfd + 1, NFDBITS) * sizeof(fd_mask);
		fdset = (fd_set *)malloc(fdsetsz);
		memset(fdset, 0, fdsetsz);

		TAILQ_FOREACH(listen_addrs_ptr, &listen_addrs, entry)
		    FD_SET(listen_addrs_ptr->fd, fdset);

		/* Wait in select until there is a connection. */
		ret = select(maxfd + 1, fdset, NULL, NULL, NULL);
		if (ret < 0 && errno != EINTR)
			fprintf(stderr, "select: %.100s\n", strerror(errno));
		if (ret < 0)
			continue;
		TAILQ_FOREACH(listen_addrs_ptr, &listen_addrs, entry) {
			if (!FD_ISSET(listen_addrs_ptr->fd, fdset))
				continue;
			fromlen = sizeof(from);
			inet_fd = accept(listen_addrs_ptr->fd, (struct sockaddr *)&from,
					 &fromlen);
			if (inet_fd < 0) {
				if (errno != EINTR && errno != EWOULDBLOCK)
					fprintf(stderr, "accept: %.100s\n", strerror(errno));
				continue;
			}
			/*
			 * Normal production daemon.  Fork, and have
			 * the child process the connection. The
			 * parent continues listening.
			 */
			if ((pid = fork()) == 0) {
				/*
				 * Child.  Close the listening and max_startup
				 * sockets.  Start using the accepted socket.
				 * Reinitialize logging (since our pid has
				 * changed).  We break out of the loop to handle
				 * the connection.
				 */
				close_listen_socks();
				break;
			}
			/* Parent.  Stay in the loop. */
			if (pid < 0) {
				fprintf(stderr, "fork: %.100s\n", strerror(errno));
				exit(1);
			}

			/* Close the new socket (the child is now taking care of it). */
			close(inet_fd);
		}
		if (TAILQ_EMPTY(&listen_addrs))
			break;
	}

	readwrite(inet_fd);

	close(inet_fd);
	close_listen_socks();

	_exit(0);
}
