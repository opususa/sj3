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
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include "sj3proxy.h"

void
set_priv(const char *title)
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
	    setregid(pw->pw_gid, pw->pw_gid) == -1 ||
	    setreuid(pw->pw_uid, pw->pw_uid) == -1
#else
# ifndef SETEUID_BREAKS_SETUID
	    setegid(pw->pw_gid) == -1 ||
	    seteuid(pw->pw_uid) == -1 ||
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
