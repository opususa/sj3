/*
 * Copyright (c) 2008 Iwata <iwata@quasiquote.org>
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

enum priv_state {
	STATE_INIT,		/* just started up */
	STATE_QUIT		/* shutting down */
};

enum cmd_types {
	PRIV_GETUGID
};

static int auth_fd = -1;
static volatile pid_t child_pid = -1;
static volatile sig_atomic_t cur_state = STATE_INIT;

static int may_read(int, void *, size_t);
static void must_read(int, void *, size_t);
static void must_write(int, void *, size_t);
static void sig_pass_to_chld(int);
static void sig_got_chld(int);

void
priv_set(const char *title)
{
	struct passwd *pw;

	if (!chroot_enable)
		return;

	if ((pw = getpwnam(chuser_name)) == NULL) {
		fprintf(stderr, "failed to get user \"%s\"\n", chuser_name);
		fflush(stderr);
		exit(255);
	}

	if (chroot(chroot_dir) != 0) {
		fprintf(stderr, "can't chroot\n");
		fflush(stderr);
		exit(255);
	}
	if (chdir("/") != 0) {
		fprintf(stderr, "can't change directory '/'\n");
		fflush(stderr);
		exit(255);
	}

	if (title != NULL)
		setproctitle("%s %s", pw->pw_name, title);

	/*
	 * Drop privileges and clear the group access list
	 */
	if (setgroups(1, &pw->pw_gid) == -1 ||
#if defined(HAVE_SETRESUID) && !defined(BROKEN_SETRESUID)
	    setresgid(pw->pw_gid, pw->pw_gid, pw->pw_gid) == -1 ||
	    setresuid(pw->pw_uid, pw->pw_uid, pw->pw_uid) == -1
#elif defined(HAVE_SETREUID) && !defined(BROKEN_SETREUID)
	    setregid(pw->pw_gid, pw->pw_gid, pw->pw_gid) == -1 ||
	    setreuid(pw->pw_uid, pw->pw_uid, pw->pw_uid) == -1
#else
# ifndef SETEUID_BREAKS_SETUID
	    setegid(pw->pw_gid) == -1 ||
	    seteuid(pw->pw_uid) == -1
# endif
	    setgid(pw->pw_gid) == -1 ||
	    setuid(pw->pw_uid) == -1
#endif
	   ) {
		fprintf(stderr, "can't drop privileges\n");
		fflush(stderr);
		exit(255);
	}
}

int
priv_init()
{
	int socks[2];
	int i;
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

	/* Father */
	/* Pass TERM/HUP/INT/QUIT through to child, and accept CHLD */
	sa.sa_handler = sig_pass_to_chld;
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGHUP, &sa, NULL);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	sa.sa_handler = sig_got_chld;
	sa.sa_flags |= SA_NOCLDSTOP;
	sigaction(SIGCHLD, &sa, NULL);

	strlcpy(chroot_dir, "/", sizeof(chroot_dir));
	priv_set("[priv]");

	while (cur_state < STATE_QUIT) {
		int cmd;
		size_t username_len;
		char username[BUFSIZ];
		int ret;
		struct passwd *pw;

		if (may_read(socks[0], &cmd, sizeof(int)))
			break;
		switch (cmd) {
		case PRIV_GETUGID:
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
		default:
			fprintf(stderr, "auth unknown command %d\n", cmd);
			close(socks[0]);
			_exit(255);
		}
	}
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

/* Pass the signal through to child */
static void
sig_pass_to_chld(int sig)
{
	int save_errno = errno;

	if (child_pid != -1)
		kill(child_pid, sig);
	errno = save_errno;
}

/* When child dies, move into the shutdown state */
/* ARGSUSED */
static void
sig_got_chld(int sig)
{
	int save_errno = errno;
	pid_t   pid;

	do {
		pid = waitpid(WAIT_ANY, NULL, WNOHANG);
		if (pid == child_pid && cur_state < STATE_QUIT)
			cur_state = STATE_QUIT;
	} while (pid > 0 || (pid == -1 && errno == EINTR));
	errno = save_errno;
}

int
priv_getugid(char *username, sj3_ugid *ugid)
{
	size_t len;
	int cmd = PRIV_GETUGID;
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


