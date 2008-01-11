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
 * $SonyRCSfile: error.c,v $  
 * $SonyRevision: 1.1 $ 
 * $SonyDate: 1994/06/03 08:02:51 $
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include "sj_typedef.h"
#include "sj_kanakan.h"

#include "sj3serv.h"

static char *tty_name = "/dev/tty";
static FILE *errfp = NULL;
static FILE *logfp = NULL;
static FILE *dbgfp = NULL;

static char *
format_time()
{
#define TBUF_LEN        2048
	static char buf[TBUF_LEN];
	time_t now;

	time(&now);
	strftime(buf, sizeof(buf), "%h %e %T ", localtime(&now));
	return buf;
}

int
open_error()
{
	int flg = 0;

	if (error_file && *error_file) {
		if (!strcmp(error_file, tty_name)) {
			errfp = stderr;
			flg = -1;
		}
		else if (!(errfp = fopen(error_file, "a")))
			fprintf(stderr, "can't open \"%s\"\n", error_file);
	}
	else
		errfp = NULL;

	return flg;
}

void
error_out(const char *fmt, ...)
{
	va_list ap;
	char tmp[BUFSIZ];

	if (errfp) {
		va_start(ap, fmt);
		vsnprintf(tmp, sizeof(tmp), fmt, ap);
		va_end(ap);
		fprintf(errfp, "%s%s: %s\n", format_time(), program_name, tmp);
		fflush(errfp);
	}
	exit(1);
}

void
warning_out(const char *fmt, ...)
{
	va_list ap;
	char tmp[BUFSIZ];

	if (errfp) {
		va_start(ap, fmt);
		vsnprintf(tmp, sizeof(tmp), fmt, ap);
		va_end(ap);
		fprintf(errfp, "%s%s: warning: %s\n", format_time(), program_name, tmp);
		fflush(errfp);
	}
}


int
open_log()
{
	int flg = 0;

	if (log_file && *log_file) {
		if (!strcmp(log_file, tty_name)) {
			logfp = stderr;
			flg = -1;
		}
		else
			logfp = fopen(log_file, "a");
		if (logfp) {
			fprintf(logfp, "%s%s log started\n",
			    format_time(), program_name);
			fflush(logfp);
		}
		else
			fprintf(stderr, "can't open \"%s\"\n", log_file);
	}
	else
		logfp = NULL;

	return flg;
}

void
logging_out(const char *fmt, ...)
{
	va_list ap;
	char tmp[BUFSIZ];

	if (logfp) {
		va_start(ap, fmt);
		vsnprintf(tmp, sizeof(tmp), fmt, ap);
		va_end(ap);
		fprintf(logfp, "%s%s\n", format_time(), tmp);
		fflush(logfp);
	}
}


int
open_debug()
{
	int flg = 0;

	if (debug_file && *debug_file) {
		if (!strcmp(debug_file, tty_name)) {
			dbgfp = stderr;
			flg = -1;
		}
		else  if (!(dbgfp = fopen(debug_file, "a")))
			fprintf(stderr, "can't open \"%s\"\n", debug_file);
	}
	else
		dbgfp = NULL;

	return flg;
}

void
debug_out(int lvl, const char *fmt, ...)
{
	va_list ap;

	if (lvl <= debug_level && dbgfp) {
		va_start(ap, fmt);
		vfprintf(dbgfp, fmt, ap);
		va_end(ap);
		fflush(dbgfp);
	}
}
