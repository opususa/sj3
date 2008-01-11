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
 * $SonyRCSfile: sj_suuji.h,v $  
 * $SonyRevision: 1.1 $ 
 * $SonyDate: 1994/06/03 08:01:19 $
 */





#ifndef	_SJ_SUUJI

#define	_SJ_SUUJI



#define	SJTBLRECLEN	6
#define	SJTBLRECNUM	34
#define	SjTblYPos(p)	(p)
#define	SJTBLYLEN	3
#define	SjTblModeP(p)	((p) + 3)
#define	SjTblNumber(md)	((md) & 0x0f)
#define	SJTBLNARABIAF	0x80
#define	SJTBLNYOMIF	0x40
#define	SJTBLNNARABIF	0x20
#define	SJTBLNNANSUUF	0x10
#define	SjTblNCont(md)	((md) & SJTBLNCONTF)
#define	SjTblCnctP(p)	((p) + 4)

#define	KR1TBLRECLEN	5
#define	KR1TBLRECNUM	10
#define	Kr1TblYPos(p)	(p)
#define	KR1TBLYLEN	3
#define	Kr1TblModeP(p)	((p) + 3)
#define	KR1TBLTOPF	0x80
#define	Kr1TblTop(md)	((md) & KR1TBLTOPF)
#define	Kr1TblKeta(md)	(((md) >> 4) & 7)
#define	Kr1TblCnct(md)	((md) & 0x0f)
#define	Kr1TblCnctP(p)	((p) + 3)

#define	KR2TBLRECLEN	4
#define	KR2TBLRECNUM	3
#define	Kr2TblYPos(p)	(p)
#define	KR2TBLYLEN	3
#define	Kr2TblModeP(p)	((p) + 3)
#define	Kr2TblCnct(md)	((md) & 0x0f)
#define	Kr2TblKeta(md)	(((md) >> 4) & 0x0f)

#define	_NUM0		0x01
#define	_NUM1		0x02
#define	_NUM2		0x03
#define	_NUM3		0x04
#define	_NUM4		0x05
#define	_NUM5		0x06
#define	_NUM6		0x07
#define	_NUM7		0x08
#define	_NUM8		0x09
#define	_NUM9		0x0a
#define	_NUM1S		0x0b
#define	_NUMS		0x0c
#define	_NUMN		0x0d

#define	COMMAHIGH	0xa1
#define	COMMALOW	0xa4

#define	SELARB		1
#define	SELARB0		2
#define	SELARBCMA	3
#define	SELKANSJ0	5
#define	SELKANSJ20	8
#define	SELKANSJYM	9
#define	SELKANSJYM2	10
#define	SELLONGARB	11
#define	SELLONGARBCMA	12
#define	SELLONGSJ	13
#define	SELARBKR	14
#define	SELKANSJKR	15
#define	SELNUMTERM	0xffff

#define	SELKANSJ	4
#define	SELKANSJ2	7
#define	SELKANSJCMA	6

#define	SELNUM0123	(JFLAG_N01 | JFLAG_N23)
#define SELNUM1_23_10_1	(JFLAG_N_1 | JFLAG_N23 | JFLAG_N10 | JFLAG_NS1)
#define	SELNUMMUST	(JFLAG_NK2 | JFLAG_NK4 | JFLAG_NN0 | JFLAG_NSN)
#define	SELNUMALL	(JFLAG_N10 | JFLAG_N_1 | JFLAG_NS1 | JFLAG_N01 \
				| JFLAG_N23 | JFLAG_NK2 | JFLAG_NK4 \
				| JFLAG_NN0 | JFLAG_NSN)



#define	SelNumFunc(c)		((c) & (~SELNUMALL))
#define	SelNumCond(c)		((c) & SELNUMALL)



#define	NUMBER123456789NS	((1<<_NUM1)|(1<<_NUM2)|(1<<_NUM3)|(1<<_NUM4) \
				|(1<<_NUM5)|(1<<_NUM6)|(1<<_NUM7)|(1<<_NUM8) \
				|(1<<_NUM9)|(1<<_NUM1S)\
				|(1<<_NUMN)|(1<<_NUMS))
#define	NUMBER0		(1<<_NUM0)
#define	NUMBER1		((1<<_NUM1)|(1<<_NUM1S))
#define	NUMBERS1	(1<<_NUM1S)
#define	NUMBER01	(NUMBER0|NUMBER1)
#define	NUMBER23	((1<<_NUM2)|(1<<_NUM3))

#endif
