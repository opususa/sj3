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
 * $SonyRCSfile: main.c,v $  
 * $SonyRevision: 1.3 $ 
 * $SonyDate: 1995/02/03 07:38:44 $
 */

#include <config.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <pwd.h>
#include <unistd.h>

#include "sj3serv.h"

#ifndef lint
static char rcsid_sony[] = "$Header: /export/work/contrib/sj3/server/RCS/main.c,v 1.12 1994/06/03 07:41:30 notanaka Exp $ SONY;";
#endif

static void
opening()
{
	printf("Kana-Kanji Conversion Server Version %s\r\n", version_number);
	printf("Copyright (c) 1990-1995 Sony Corporation\r\n");
	printf("Build at %s\r\n", time_stamp);
}

static int
signal_handler(int sig)
{
	static int wantwarn = 0;

	if (wantwarn)
		return 0;
	wantwarn = 1;

	warning_out("signal %d catched.", sig);

	wantwarn = 0;
	return 0;
}

static int
terminate_handler(int sig)
{
	static int wantdie = 0;

	if (wantdie)
		return 0;
	wantdie = 1;

	warning_out("terminate signal %d catched.", sig);

	server_terminate();
}

void
server_terminate()
{
	close_socket();
	sj_closeall();

	exit(0);
}

static void
set_signals()
{
	signal(SIGHUP,  (void (*)(int))signal_handler);

	signal(SIGINT,  (void (*)(int))terminate_handler);
	signal(SIGQUIT, (void (*)(int))signal_handler);
	signal(SIGTERM, (void (*)(int))terminate_handler);
}

static void
exec_daemon()
{
	open_error();
	open_log();
        open_debug();

	if (daemon_enable) {
		daemon(0, 1);
	}
}

int
main(int argc, char **argv)
{
	setlocale(LC_ALL, "");

	parse_arg(argc, argv);

	opening();

	set_default();
	read_runcmd();

	exec_daemon();

	priv_init();

	set_signals();

	socket_init();
	open_socket();

	if (chroot_enable == 1)
		priv_set(NULL);

	preload_dict();
	preopen_dict();

	communicate();

	close_socket();

	sj_closeall();

	exit(0);
}
