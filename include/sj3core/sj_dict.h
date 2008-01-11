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
 * $SonyRCSfile: sj_dict.h,v $  
 * $SonyRevision: 1.1 $ 
 * $SonyDate: 1994/06/03 08:01:03 $
 */





#ifndef	_SJ_DICT

#define	_SJ_DICT



#define	DIC_MAIN		0
#define	DIC_USER		1

#define	DICSEGBASE		0
#define	MAXDOUONBLK		0xfff
#define	DOUONBLKSIZENUMBER	3
#define	DICSEGTERM		0xff
#define	DOUONBLKTERM		0xff
#define	HINSIBLKTERM		0xff
#define	MAXSEGMENTNUMBER	0xffff



#define NORMALKANJIMASK         0x7f
#define SS3KANJI                0x0080
#define	ZENHIRAASSYUKU		0x80
#define	OFFSETASSYUKU		0x90
#define	ZENKATAASSYUKU		0xa0
#define	AIATTRIBUTE		0xb0
#define	KANJIASSYUKU		0xc0
#define	LEADINGHANKAKU		0xd0
#define	KANJISTREND		0x00
#define ISASSYUKU               0x80
#define ASSUKUUSE2BYTE          0x90
#define KNJASSYUKUMASK          0x0f
#define KANJICODEMASK           0x0f
#define KANJIMODEMASK           0xf0
#define ATRMSKANJIOFS           0xc0c0

#define	MAXKNJASKNUMBER		16
#define	MAXYOMIASKNUMBER	16

#define	MAXWDYOMILEN		31
#define	MAXWDKANJILEN		64

#define	ASSYUKUYOMITOP		0x80
#define	NOKORIYOMITOP		0x40


#define getsize(p)	(((int)(*(p) & 0x0f) * 0x100) + *((p) + 1))

#define	getntag(p)	((*(p) == DICSEGTERM) ? (p) : (p) + getsize(p))

#define	getplen(p)	(((*(p) & ASSYUKUYOMITOP) ? 0x10 : 0) + (*((p) + 2) >> 4))

#define	getnlen(p)	(((*(p) & NOKORIYOMITOP) ? 0x10 : 0) + (*((p) + 2) & 0x0f))

#define	segtop()	(dicbuf + *dicbuf + 1)

#define	segend(p)	(*(p) == DICSEGTERM || (p) >= dicbuf + curdict->seglen)

#endif

