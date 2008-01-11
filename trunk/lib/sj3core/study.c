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
 * $SonyRCSfile: study.c,v $  
 * $SonyRevision: 1.1 $ 
 * $SonyDate: 1994/06/03 08:02:38 $
 */

#include <string.h>
#include "sj_kcnv.h"

#include "sj_kanakan.h"

int
study(STDYOUT *stdy)
{
	STDYOUT		outp;
	STDYIN		*p1;
	STDYIN		*p2;
	int		wcnt;
	int		ret;
	TypeDicSeg	seg;
	TypeDicOfs	ofs;
	TypeStyNum	nm;
	TypeStyNum	wi;

	if (!StudyExist()) return STUDYNODICT;

	memcpy((u_char *)&outp, (u_char *)stdy, sizeof(outp));

	if (outp.stdy1.offset == 0) return STUDYNOTSTUDY;

	p1 = srchstdy(outp.stdy1.seg, outp.stdy1.offset, outp.stdy1.dicid);
	if (p1) {
		nm = p1 -> styno;

		for (wcnt = STUDYCOUNT, p2 = STUDYDICT ; wcnt-- ; p2++) {
			if (p2 -> styno < nm) p2 -> styno++;
		}

		p1 -> styno  = 1;

		if (outp.sttfg) p1 -> sttkj  = outp.stdy1.sttkj;
		if (outp.ka_fg) p1 -> ka_kj  = outp.stdy1.ka_kj;

		p1 -> nmflg = outp.stdy1.nmflg;

		putstydic();

		return STUDYNORMEND;
	}


	if (STUDYCOUNT >= STUDYMAX) {

		p1 = STUDYDICT; p2 = NULL; wi = 0;
		for (wcnt = STUDYMAX ; wcnt-- ; p1++) {
			if ((nm = p1 -> styno + 1) > wi) {
				wi = nm;
				p2 = p1;
			}

			p1 -> styno = nm;
		}

		if (p2) {
			mvmemi((u_char *)(p2 + 1), (u_char *)p2,
				(STUDYTAIL - p2) * STUDYRECSIZE);
			STUDYCOUNT -= 1;
		}
		else
			return STUDYERROR;

		ret = STUDYFULLAREA;
	}
	else {
		p1 = STUDYDICT;
		for (wcnt = STUDYCOUNT ; wcnt-- ; p1++) p1 -> styno++;

		ret = STUDYNORMEND;
	}

	p1 = STUDYDICT;
	seg = outp.stdy1.seg;
	for (wcnt = STUDYCOUNT ; wcnt && (p1 -> seg < seg) ; ) {
		wcnt--; p1++;
	}

	ofs = outp.stdy1.offset;
	while (wcnt-- && (p1 -> seg == seg)) {
		if (p1 -> offset > ofs) break;
		p1++;
	}

	mvmemd((u_char *)(STUDYTAIL - 1), (u_char *)STUDYTAIL,
			(u_char *)(STUDYTAIL - 1) - (u_char *)p1);
	p1 -> offset = outp.stdy1.offset;
	p1 -> seg    = outp.stdy1.seg;
	p1 -> dicid  = outp.stdy1.dicid;
	p1 -> styno  = 1;
	p1 -> sttkj  = (outp.sttfg) ? outp.stdy1.sttkj : 0;
	p1 -> ka_kj  = (outp.ka_fg) ? outp.stdy1.ka_kj : 0;
	p1 -> nmflg  = outp.stdy1.nmflg;
	STUDYCOUNT += 1;

	putstydic();

	return ret;
}

STDYIN *
srchstdy(TypeDicSeg seg, TypeDicOfs ofs, TypeDicID dicid)
{
	int	high;
	int	mid;
	int	low;
	STDYIN	*ptr;


	if (!StudyExist()) return NULL;

	if (!STUDYCOUNT || !ofs) return NULL;

	low = 0;
	high = STUDYCOUNT - 1;

	for ( ; ; ) {
		mid = ((low + high) >> 1);

		if (STUDYDICT[mid].seg > seg)
			high = mid - 1;

		else if (STUDYDICT[mid].seg < seg)
			low = mid + 1;

		else
			break;

		if (low > high) return NULL;
	}

	low = mid + 1;
	ptr = &STUDYDICT[mid];
	while (ptr -> seg == seg) {
		if (ptr -> offset < ofs) break;
		if (ptr -> dicid == dicid && ptr -> offset == ofs) return ptr;
		if (!mid--) break;
		ptr--;
	}

	ptr = &STUDYDICT[low];
	mid = STUDYCOUNT - low;
	while (mid-- && ptr -> seg == seg) {
		if (ptr -> offset > ofs) break;
		if (ptr -> dicid == dicid && ptr -> offset == ofs) return ptr;
		ptr++;
	}

	return NULL;
}

