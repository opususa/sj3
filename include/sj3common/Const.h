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
 * $SonyRCSfile: Const.h,v $  
 * $SonyRevision: 1.4 $ 
 * $SonyDate: 1997/01/23 11:20:52 $
 */

#ifndef CONST_H
#define CONST_H

/*
 * Change some default values.
 * Patched by Hidekazu Kuroki(hidekazu@cs.titech.ac.jp)		1996/8/10
 */

#define	PASSWORDLEN		16

#define	YOMIBUFSIZE		1024
#define	KANJIBUFSIZE		2048

#define	MINIDXLEN		512
#define	MINSEGLEN		512
#define	MINSEGNUM		1
#define	MINSTYNUM		512
#define	MAXSTYNUM		0x7fff
#define	MINCLSTEP		1
#define	MAXCLSTEP		256
#define	MINCLLEN		512
#define	MAXCLLEN		0x7fff

#define	DEBUGOUTFILE		NULL
#define	DEBUGLEVEL		0
#define	FORKFLAG		1
#define	MAXCLIENTNUM		512
#define	LOGOUTFILE		NULL
#define ERROROUTFILE		"/dev/null"
#define ADDRESSFAMILY		"unspec"
#define	PORTNAME		"sj3"
#define	LOCALHOST		"localhost"
#define CHROOTFLAG		1
#define STRICTAUTHFLAG		1
#define MAINDICTIONARY		"sj3main.dic"
#define USERDICTIONARY		"private.dic"
#define STUDYFILE		"study.dat"

/*
 * Change the default Port number of sj3 for FreeBSD.
 * Patched by Hidekazu Kuroki(hidekazu@cs.titech.ac.jp)		1996/8/20
 */
#define	PORTNUMBER		"3086"
#define	DIRECTRYMODE		0755
#define	DICTFILEMODE		0644
#define	MODE_MASK		0777

#define	DEFIDXLEN		2048
#define	DEFSEGLEN		2048
#define	DEFSEGNUM		256
#define	DEFSTYNUM		2048
#define	DEFCLSTEP		1
#define	DEFCLLEN		2048

#define	NOTLOCKED		-1

#define	ERROR			-1

#ifndef	FALSE
#	define	TRUE		-1
#	define	FALSE		(!TRUE)
#endif

#endif /* CONST_H */
