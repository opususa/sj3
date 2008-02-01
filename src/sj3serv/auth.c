/*
 * Copyright (c) 2003 Anil Madhavapeddy <anil@recoil.org>
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
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pwd.h>
#include "sj3serv.h"

enum cmd_types {
	AUTH_GETUGID,
	AUTH_EXIT
};

static int auth_fd = -1;

static int may_read(int, void *, size_t);
static void must_read(int, void *, size_t);
static void must_write(int, void *, size_t);


int
exec_auth()
{
	int socks[2];
	int i;
	pid_t child_pid = -1;
	struct sigaction sa;

	if (socketpair(AF_LOCAL, SOCK_STREAM, PF_UNSPEC, socks) == -1) {
		fprintf(stderr, "socketpair() failed");
		exit(255);
	}

	child_pid = fork();
	if (child_pid < 0) {
		fprintf(stderr, "fork() failed\n");
		exit(255);
	}

	if (!child_pid) {
		auth_fd = socks[1];
		return 0;
	}

	memset(&sa, 0, sizeof(sa));
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = SIG_DFL;
	for (i = 1; i < NSIG; i++)
		sigaction(i, &sa, NULL);

	strlcpy(chroot_dir, "/", sizeof(chroot_dir));
	set_priv("[auth]");

	while (1) {
		int cmd;
		size_t username_len;
		char username[BUFSIZ];
		int ret;
		struct passwd *pw;

		if (may_read(socks[0], &cmd, sizeof(int)))
			break;
		switch (cmd) {
		case AUTH_GETUGID:
			/* length, user name */
			must_read(socks[0], &username_len, sizeof(size_t));
			if (username_len == 0 || username_len > sizeof(username))
                                _exit(0);
			must_read(socks[0], username, username_len);
			username[username_len - 1] = '\0';
			pw = getpwnam(username);
			if (pw) {
				ret = 1;
				must_write(socks[0], &ret, sizeof(int));
				must_write(socks[0], &pw->pw_uid, sizeof(uid_t));
				must_write(socks[0], &pw->pw_gid, sizeof(gid_t));
			} else {
				ret = 0;
				must_write(socks[0], &ret, sizeof(int));
			}
			endpwent();
			break;
		case AUTH_EXIT:
			close(socks[0]);
			_exit(1);
		default:
			fprintf(stderr, "auth unknown command %d\n", cmd);
			close(socks[0]);
			_exit(255);
		}
	}
	fprintf(stderr, "auth fatal error\n");

	close(socks[0]);
	_exit(1);
}

/* Read all data or return 1 for error.  */
static int
may_read(int fd, void *buf, size_t n)
{
	char *s = buf;
	ssize_t res, pos = 0;

	while (n > pos) {
		res = read(fd, s + pos, n - pos);
		switch (res) {
		case -1:
			if (errno == EINTR || errno == EAGAIN)
				continue;
		case 0:
			return (1);
		default:
			pos += res;
		}
	}
	return (0);
}

/* Read data with the assertion that it all must come through, or
 * else abort the process.  Based on atomicio() from openssh. */
static void
must_read(int fd, void *buf, size_t n)
{
	char *s = buf;
	ssize_t res, pos = 0;

	while (n > pos) {
		res = read(fd, s + pos, n - pos);
		switch (res) {
		case -1:
			if (errno == EINTR || errno == EAGAIN)
				continue;
		case 0:
			fprintf(stderr, "must_read: internal protocol error\n");
			fflush(stderr);
			_exit(0);
		default:
			pos += res;
		}
	}
}

/* Write data with the assertion that it all has to be written, or
 * else abort the process.  Based on atomicio() from openssh. */
static void
must_write(int fd, void *buf, size_t n)
{
	char *s = buf;
	ssize_t res, pos = 0;

	while (n > pos) {
		res = write(fd, s + pos, n - pos);
		switch (res) {
		case -1:
			if (errno == EINTR || errno == EAGAIN)
				continue;
		case 0:
			fprintf(stderr, "must_write: internal protocol error\n");
			fflush(stderr);
			_exit(0);
		default:
			pos += res;
		}
	}
}

int
auth_getugid(char *username, sj3_ugid *ugid)
{
	size_t len;
	int cmd = AUTH_GETUGID;
	int ret;
	uid_t uid;
	gid_t gid;

	if (auth_fd < 0) {
		fprintf(stderr, "%s called from privileged portion", "auth_getpwnam");
		exit(255);
	}

	len = strlen(username) + 1;
	if (len == 0 || BUFSIZ < len)
		return NULL;

	must_write(auth_fd, &cmd, sizeof(int));
	must_write(auth_fd, &len, sizeof(size_t));
	must_write(auth_fd, username, len);
	must_read(auth_fd, &ret, sizeof(int));
	if (ret) {
		must_read(auth_fd, &uid, sizeof(uid_t));
		must_read(auth_fd, &gid, sizeof(gid_t));
		ugid->uid = uid;
		ugid->gid = gid;
		return 1;
	} else {
		return 0;
	}
}

void
exit_auth()
{
	int cmd = AUTH_EXIT;

	must_write(auth_fd, &cmd, sizeof(int));

	return;
}

