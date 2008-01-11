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
 * $SonyRCSfile: Dict.h,v $  
 * $SonyRevision: 1.1 $ 
 * $SonyDate: 1994/06/03 08:00:55 $
 */




#define	VERSIONPOS		0

#define	PASSWDPOS		16
#define	PASSWDLEN		16


#define	STDYNORMPOS		32
#define	STDYNORMLEN		36
#define	STDYNORMNUM		40
#define	STDYNORMCNT		44

#define	STDYCLIDXPOS		48
#define	STDYCLIDXLEN		52
#define	STDYCLIDXSTEP		56

#define	STDYCLSEGPOS		64
#define	STDYCLSEGLEN		68


#define	DICTIDXPOS		32
#define	DICTIDXLEN		36

#define	DICTSEGPOS		48
#define	DICTSEGLEN		52
#define	DICTSEGNUM		56
#define	DICTSEGMAX		60

#define	DICTAIDXPOS		64
#define	DICTAIDXLEN		68

#define	DICTASEGPOS		80
#define	DICTASEGLEN		84


#define	HEADERPOS		0
#define	HEADERLENGTH		128
#define	COMMENTPOS		128
#define	COMMENTLENGTH		128


#define	Byte2Long(c3,c2,c1,c0)	(((c3) << (8*3)) | ((c2) << (8*2)) | \
				 ((c1) << (8*1)) | ((c0) << (8*0)))
#define	STDYVERSION		Byte2Long('S', 2, 0, 0)
#define	DICTVERSION		Byte2Long('D', 2, 0, 0)
