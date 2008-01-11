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
 * $SonyRCSfile: Struct.h,v $  
 * $SonyRevision: 1.3 $ 
 * $SonyDate: 1994/10/21 05:13:34 $
 */

#ifndef STRUCT_H
#define STRUCT_H

typedef struct global {
	JREC	*Jmaxjptr;
	CLREC	*Jmaxclptr;
	JREC	*Jjrt1st;
	CLREC	*Jclt1st;
	JREC	*Jjrt2nd;
	CLREC	*Jclt2nd;
	CLREC	*Jselcl;

	short	Jselectid;
	short	Jtrank;
	short	Jnrank;

	u_char	*Jinputyomi;
	u_char	*Jcnvstart;
	short	Jcnvlen;
	u_char	*Jystart;
	u_char	Jhyomi[MAXPHINPUTLEN + 1];
	u_char	Jorgyomi[MAXCLINPUTLEN*2 + 1];

	u_char	Jkanjibuf[MINPHOUTPUTLEN];
	u_char	*Jkanjipos;
	u_char	*Jkanjitmp;
	short	Jkanjilen;

	u_char		Jdicinl;
	u_char		Jdicsaml;
	TypeDicSeg	Jprevseg;

	u_char	Jfzkcount;
	u_char	Jfzk_ka_flg;
	FREC	Jfzktbl[MAXFREC];

	u_char	Jheadcode;
	u_char	Jheadlen;

	u_char	Jgobilen;

	TypeGram	Jprevclgrm;
	TypeCnct	Jprevclrow;
	u_char		Jnextcllen;

	short	Jkhcount;
	short	Jnkhcount;
	KHREC	Jkouhotbl[MAXKOUHONUMBER];
	STDYIN	*Jstdytop;

	DICT		*Jcurdict;
	DICTL		*Jdictlist;
	u_char		*Jdicbuf;
	u_char		*Jidxbuf;
	TypeIdxOfs	*Jidxofs;
	u_char	*Jaskknj[MAXKNJASKNUMBER];
	u_char	*Jaskknj_k[MAXKNJASKNUMBER];

	STDY	*Jcurstdy;

	u_short		Jsuuji_wbuf[NUMWORDBUF];
	u_char		Jsuuji_ubuf[NUMKETALENGTH];
	TypeClass	Jsuuji_class;
	u_char		Jsuuji_comma;
	u_char		Jsuuji_keta;
	u_char		*Jsuuji_yptr;
	u_char		Jsuuji_len;
	u_char		Jsuuji_exit;
	u_short		*Jsuuji_wkeep;
	u_char		*Jsuuji_ukeep;

	u_char		Jpeepyomi[MAXWDYOMILEN * 2 + 1];
	u_char		Jpeepknj[MAXWDKANJILEN + 1];
	TypeGram	Jpeepgrm;
	u_char		*Jpeepdptr;
	u_char		*Jpeephptr;
	u_char		*Jpeepkptr;
	TypeDicSeg	Jpeepidx;
} Global;


typedef	struct	dictfile {
	DICT		dict;
	int		refcnt;
	fd_set		lock;
	FILE		*fp;
	int		fd;

	u_char		*buffer;
	long		bufsiz;

	long		idxstrt;
	long		segstrt;

	TypeIdxOfs	*ofsptr;

	struct dictfile	*link;
} DictFile;

typedef struct	stdyfile {
	STDY		stdy;
	int		refcnt;
	ino_t		inode;
	FILE		*fp;
	int		fd;

	u_char		*header;

	struct stdyfile	*link;
} StdyFile;

typedef	struct workarea {
	int	refcnt;
	int	lock;

	struct workarea	*link;

	Global		global;
} WorkArea;

typedef	struct client {
	int		fd;
	char		host[MAXHOSTNAMELEN];
	char		user[MAXLOGNAME];
	char		prog[FILENAME_MAX];

	WorkArea	*work;
	StdyFile	*stdy;
	short           version;
	unsigned char   def_char[2];
#if (defined(TLI) && defined(SOCK_UNIX))
	int             unix_flag;
#endif
	uid_t           uid;
	struct client   *next;
} Client;

#endif
