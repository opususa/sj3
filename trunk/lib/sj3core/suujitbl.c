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
 * $SonyRCSfile: suujitbl.c,v $  
 * $SonyRevision: 1.1 $ 
 * $SonyDate: 1994/06/03 08:02:39 $
 */




#include "sj_const.h"
#include "sj_rename.h"
#include "sj_typedef.h"
#include "sj_yomi.h"
#include "sj_suuji.h"

#include "sj_kanakan.h"

#define	NA	SJTBLNARABIAF
#define	NY	SJTBLNYOMIF
#define	NN	SJTBLNNARABIF
#define	NS	SJTBLNNANSUUF



u_char	suuji_tbl[] = {
	N_0,_NIL,_NIL,	_NUM0  | NA,		0x00, 0x00,
	N_1,_NIL,_NIL,	_NUM1  | NA,		0xec, 0x07,
	N_2,_NIL,_NIL,	_NUM2  | NA,		0xec, 0x07,
	N_3,_NIL,_NIL,	_NUM3  | NA,		0xff, 0x07,
	N_4,_NIL,_NIL,	_NUM4  | NA,		0xec, 0x07,
	N_5,_NIL,_NIL,	_NUM5  | NA,		0xec, 0x07,
	N_6,_NIL,_NIL,	_NUM6  | NA,		0xec, 0xc7,
	N_7,_NIL,_NIL,	_NUM7  | NA,		0xec, 0x07,
	N_8,_NIL,_NIL,	_NUM8  | NA,		0xec, 0xc7,
	N_9,_NIL,_NIL,	_NUM9  | NA,		0xec, 0x07,
	_I, _TI, _NIL,	_NUM1,			0x00, 0x05,
	_I, _XTU,_NIL,	_NUM1S | NY,		0x20, 0x02,
	_KI,_XYU,_U,  	_NUM9,			0xec, 0x07,
	_KU,_NIL,_NIL,	_NUM9  | NY,		0x00, 0x00,
	_GO,_NIL,_NIL,	_NUM5,			0xec, 0x07,
	_GO,_U,  _NIL,	_NUM5  | NN,		0x00, 0x00,
	_SA,_NN, _NIL,	_NUM3,			0xd3, 0x07,
	_SI,_NIL,_NIL,	_NUM4  | NY,		0x80, 0x00,
	_SI,_I,  _NIL,	_NUM4  | NN,		0x00, 0x00,
	_SI,_TI, _NIL,	_NUM7,			0xec, 0x07,
	_SU,_U,  _NIL,  _NUMS  | NY | NS,	0xec, 0x07,
	_ZE,_RO, _NIL,	_NUM0  | NN,		0x00, 0x00,
	_NA,_NA, _NIL,	_NUM7,			0xec, 0x07,
	_NA,_NN, _NIL,	_NUMN  | NY | NS,	0xf3, 0x07,
	_NI,_NIL,_NIL,	_NUM2,			0xec, 0x07,
	_NI,_I,  _NIL,	_NUM2  | NN,		0x00, 0x00,
	_HA,_TI, _NIL,	_NUM8,			0xc0, 0x05,
	_HA,_XTU,_NIL,	_NUM8  | NY,		0x20, 0xc2,
	_MA,_RU, _NIL,	_NUM0  | NN,		0x00, 0x00,
	_YO,_NIL,_NIL,	_NUM4  | NY,		0x00, 0x00,
	_YO,_NN, _NIL,	_NUM4,			0xec, 0x07,
	_RE,_I,  _NIL,	_NUM0  | NN,		0xec, 0x00,
	_RO,_KU, _NIL,	_NUM6,			0xe0, 0x07,
	_RO,_XTU,_NIL,	_NUM6  | NY,		0x00, 0xc2,
		0
};



u_char	kurai1_tbl[] = {
	_ZI,_XYU,_U,  	(1<<4)+0+KR1TBLTOPF,	0x05,
	_ZI,_XYU,_XTU,	(1<<4)+1+KR1TBLTOPF,	0x02,
	_SE,_NN, _NIL,	(3<<4)+2+KR1TBLTOPF,	0x07,
	_ZE,_NN, _NIL,	(3<<4)+3,		0x05,
	_HI,_XYA,_KU, 	(2<<4)+4+KR1TBLTOPF,	0x07,
	_HI,_XYA,_XTU,	(2<<4)+5+KR1TBLTOPF,	0x02,
	_BI,_XYA,_KU, 	(2<<4)+6,		0x07,
	_BI,_XYA,_XTU,	(2<<4)+7,		0x00,
	_PI,_XYA,_KU, 	(2<<4)+8,		0x07,
	_PI,_XYA,_XTU,	(2<<4)+9,		0x00,
		0
};



u_char	kurai2_tbl[] = {
	_O, _KU, _NIL,	 (2<<4)+13,
	_TI,_XYO,_U,  	 (3<<4)+14,
	_MA,_NN, _NIL,	 (1<<4)+15,
		0
};



u_char	num1tbl[]  = {
	0xa3, 0xb0,
	0xa3, 0xb1,
	0xa3, 0xb2,
	0xa3, 0xb3,
	0xa3, 0xb4,
	0xa3, 0xb5,
	0xa3, 0xb6,
	0xa3, 0xb7,
	0xa3, 0xb8,
	0xa3, 0xb9,
	0xa3, 0xb1,
	0xbf, 0xf4,
	0xb2, 0xbf
};



u_char	num2tbl[]  = {
	0xa1, 0xbb,
	0xb0, 0xec,
	0xc6, 0xf3,
	0xbb, 0xb0,
	0xbb, 0xcd,
	0xb8, 0xde,
	0xcf, 0xbb,
	0xbc, 0xb7,
	0xc8, 0xac,
	0xb6, 0xe5,
	0xb0, 0xec,
	0xbf, 0xf4,
	0xb2, 0xbf
};



u_char	num3tbl[]  = {
	0xce, 0xed,
	0xb0, 0xed,
	0xc6, 0xf5,
	0xbb, 0xb2,
	0xbb, 0xcd,
	0xb8, 0xde,
	0xcf, 0xbb,
	0xbc, 0xb7,
	0xc8, 0xac,
	0xb6, 0xe5,
	0xb0, 0xed,
	0xbf, 0xf4,
	0xb2, 0xbf
};



u_char	num4tbl[] = {
	0xc0, 0xe9,
	0xc9, 0xb4,
	0xbd, 0xbd,
	0
};



u_char	num5tbl[] = {
	0xc0, 0xe9,
	0xc9, 0xb4,
	0xbd, 0xa6,
	0
};



u_char	num6tbl[] = {
	0xc3, 0xfb,
	0xb2, 0xaf,
	0xcb, 0xfc,
	0
};
