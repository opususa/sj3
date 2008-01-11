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
 * $SonyRCSfile: sj_study.h,v $  
 * $SonyRevision: 1.1 $ 
 * $SonyDate: 1994/06/03 08:01:18 $
 */

#ifndef	_SJ_STUDY

#define	_SJ_STUDY

#define	StudyExist()	(stdy_base)

#define	STUDYCOUNT	(stdy_base -> stdycnt)
#define	STUDYMAX	(stdy_base -> stdymax)
#define	STUDYDICT	(stdy_base -> stdydic)

#define	CLSTUDYIDX	(stdy_base -> clstdyidx)
#define	CLSTUDYSTEP	(stdy_base -> clstdystep)
#define	CLSTUDYLEN 	(stdy_base -> clstdylen)
#define	CLSTUDYDICT	(stdy_base -> clstdydic)

#define	STUDYTAIL	(STUDYDICT + STUDYMAX)
#define	STUDYRECSIZE	(sizeof(STDYIN))

#define	CLSTUDYTAIL	(CLSTUDYDICT + CLSTUDYLEN)

#define	STUDYNORMEND	0
#define	STUDYNODICT	1
#define	STUDYFULLAREA	2
#define	STUDYNOTSTUDY	3
#define	STUDYERROR	4

#define	CLSTDY_TMPLEN	256
#define	CLSTUDYNORMEND	0
#define	CLSTUDYNODICT	1
#define	CLSTUDYPARAERR	2
#define	CLSTUDYYOMIERR	3
#define	CLSTUDYNOAREA	4

#define	CL_YLEN		0
#define	CL_GRAM2	1
#define	CL_Y1LEN	2
#define	CL_NUMH		3
#define	CL_NUML		4
#define	CL_YDATA	5
#define	CL_HDLEN	5



#define	iseocl(p)	(!ClYomiLen(p) || (u_char *)(p) >= CLSTUDYTAIL)

#define	ClGramCode(p)	*((p) + CL_GRAM2)
#define	ClYomiLen(p)	*((p) + CL_YLEN)
#define	ClYomi1Len(p)	*((p) + CL_Y1LEN)
#define	ClHighNum(p)	*(p + CL_NUMH)
#define	ClLowNum(p)	*(p + CL_NUML)
#define	ClGetNum(p)	(ClHighNum(p) * 0x100 + ClLowNum(p))
#define	ClSetNum(p, n)	(ClHighNum(p)=(u_char)((n) >> 8), ClLowNum(p)=(u_char)(n))
#define	ClYomiPos(p)	((p) + CL_YDATA)
#define	ClBlkLen(p)	(ClYomiLen(p) + CL_HDLEN)
#define	ClNextTag(p)	((p) + ClBlkLen(p))

#endif
