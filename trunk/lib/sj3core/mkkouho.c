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
 * $SonyRCSfile: mkkouho.c,v $  
 * $SonyRevision: 1.1 $ 
 * $SonyDate: 1994/06/03 08:02:04 $
 */




#include "sj_euc.h"
#include "sj_kcnv.h"
#include "sj_hinsi.h"
#include "sj_suuji.h"
#include "sj_dict.h"

#include "sj_kanakan.h"

static  void	getkhtbl(), cl_kanji();
static  int	diffknj();
static  void	cl_numcmn();
static  u_char	*makekan();

void
mkkouho()
{
	CLREC	*clrec;
	int	keeplen;

	khcount = nkhcount = 0;

	keeplen = clt1st -> cllen;

	clrec = clt1st;

	do {
		getkhtbl(clrec);
		clrec = clrec -> clsort;
	} while (clrec && (keeplen == clrec -> cllen));
}



static void
getkhtbl(CLREC *clrec)
{
	JREC	*jrec;

	jrec = clrec -> jnode;

	switch (jrec -> class) {
	case C_DICT:
		cl_kanji(jrec, clrec);
		break;
	case C_N_ARABIA:
	case C_N_ARACMA:
	case C_N_KAZU:
	case C_N_SUUJI:
	case C_N_NANSUU:
	case C_N_KANKURA:
	case C_N_ARAKURA:
	case C_N_KAZULONG:
	case C_N_KAZULCMA:
	case C_N_SUUJILONG:
		cl_numcmn(jrec, clrec);
		break;
	case C_BUNSETU:
		break;
	case C_MINASI:
	case C_WAKACHI:
		setkouho(clrec, (TypeDicOfs)0, 0);
		break;
	}
}



static void
cl_kanji(JREC *jrec, CLREC *clrec)
{
	u_char	*ptr;
	int	kcount = khcount;

	if (seldict(jrec -> dicid)) {
		(*curdict->getdic)(curdict, jrec -> jseg);
		ptr = dicbuf + jrec->jofsst + 1;

		get_askknj();

		while (*ptr != HINSIBLKTERM) {
			if (diffknj(jrec, ptr, kcount)) {
				setkouho(clrec, (TypeDicOfs)(ptr - dicbuf), 0);
			}
			ptr = skipkstr(ptr);
		}
	}
	else {
		setkouho(clrec, (TypeDicOfs)1, 0);
	}
}



u_char *
makekan_none(u_char *s, u_char *d, int flg)
{
	return d;
}



u_char *
makekan_1byte(u_char *s, u_char *d, int flg)
{
	if ((*s & KANJIMODEMASK) == ZENHIRAASSYUKU)
	  *d++ = 0x10 | (*s & KNJASSYUKUMASK);
	else
	  *d++ = 0x90 | (*s & KNJASSYUKUMASK);

	if (flg) *d++ = KANJISTREND;
	return d;
}



u_char *
makekan_knj(u_char *s, u_char *d, int flg)
{
	return makekan(askknj[*s & KNJASSYUKUMASK], d, flg);
}



u_char	*makekan_ofs(u_char *s, u_char *d, int flg)
{
        return makekan(dicbuf + ((*s & KANJICODEMASK) << 8) + *(s + 1),
                                d, flg);
}



u_char *
makekan_norm(u_char *s, u_char *d, int flg)
{
	u_char c;

	if (*s != 0) {
		c = s[1];
		if (c & 0x80)
		  *d++ = SS3;
		*d++ = *s | 0x80;
		*d++ =  c | 0x80;
	} else {
		*d++ = *s;
	}
	if (flg) *d++ = KANJISTREND;
	return d;
}



u_char *
makekan_ascii(u_char *s, u_char *d, int flg)
{
	u_char c;

	c = s[1];
	if (c & 0x80)
	  *d++ = SS2;
	*d++ = c;

	if (flg) *d++ = KANJISTREND;

	return d;
}



static u_char *
makekan(u_char *s, u_char *d, int flg)
{
	int csize;

	for ( ; ; ) {
		csize = codesize(*s);
		if (s[csize] == KANJISTREND) {
			switch(*s & KANJIMODEMASK) {
			      case ZENHIRAASSYUKU:
                                d = makekan_1byte(s, d, flg); break;

			      case ZENKATAASSYUKU:
                                d = makekan_1byte(s, d, flg); break;

			      case OFFSETASSYUKU:
                                d = makekan_ofs(s, d, flg); break;

			      case AIATTRIBUTE:
                                d = makekan_none(s, d, flg); break;

			      case LEADINGHANKAKU:
				d = makekan_ascii(s, d, flg); break;

			      case KANJIASSYUKU:
				d = makekan_knj(s, d, flg); break;

			      default:
				d = makekan_norm(s, d, flg);
			}
			break;
		}
		else
			switch(*s & KANJIMODEMASK) {
			      case ZENHIRAASSYUKU:
                                d = makekan_1byte(s, d, FALSE); break;

			      case ZENKATAASSYUKU:
                                d = makekan_1byte(s, d, FALSE); break;

			      case OFFSETASSYUKU:
                                d = makekan_ofs(s, d, FALSE); break;

			      case AIATTRIBUTE:
                                d = makekan_none(s, d, FALSE); break;

			      case LEADINGHANKAKU:
				d = makekan_ascii(s, d, FALSE); break;

			      case KANJIASSYUKU:
				d = makekan_knj(s, d, FALSE); break;

			      default:
				d = makekan_norm(s, d, FALSE);
			}
		s += codesize(*s);
	}

	return d;
}



#define	PEND	2
#define	QEND	1
static int
sameknj(u_char *p, u_char plen, u_char *q, u_char qlen)
{
	int	i;
	int	j;
	int	endf = 0;
	int     brkf;

	do {
		if ((i = euc_codesize(*p)) != euc_codesize(*q)) return FALSE;

		brkf = 0;
		for (j = 0; j < i; j++) {
			if (p[j] != q[j] ) {
				brkf = 1;
				break;
			}
		}
		if (brkf) break;

		if (p[i] == KANJISTREND) endf += PEND;
		if (q[i] == KANJISTREND) endf += QEND;

		p += i;
		q += i;
	} while (!endf);

	switch (endf) {
	case (PEND | QEND):
		if (plen == qlen) return TRUE;
		break;

	case PEND:
		i = plen;
		plen = qlen;
		qlen = i;
		p = q;

	case QEND:
		if (p[euc_codesize(*p)] != KANJISTREND) break;
		if ((*p & KANJIMODEMASK) != 0x10 ) break;
		if (qlen != plen - (*p & KANJICODEMASK) - 1) break;
		return TRUE;

	default:
		if (q[euc_codesize(*q)] != KANJISTREND) break;
		if (p[euc_codesize(*p)] != KANJISTREND) break;
		if ((*q & KANJIMODEMASK) != 0x10 ) break;
		if ((*p & KANJIMODEMASK) != 0x10 ) break;
		if (plen - (*p & KANJICODEMASK) != qlen - (*q & KANJICODEMASK))
			break;
		return TRUE;
	}

	return FALSE;
}
#undef	PEND
#undef	QEND



static int
diffknj(JREC *jrec, u_char *ptr, int num)
{
	KHREC	*kptr;
	JREC	*jptr;
	int	i;
	u_char	kbuf1[MAXWDKANJILEN];
	u_char	kbuf2[MAXWDKANJILEN];

	if (jrec -> hinsi == TANKANJI) return TRUE;

	makekan(ptr, kbuf1, TRUE);

	for (i = 0, kptr = kouhotbl ; i < num ; i++, kptr++) {
		if ((jptr = kptr -> clrec -> jnode) == jrec)
			return FALSE;

		if (jptr -> hinsi == TANKANJI) continue;

		if (jptr -> class != jrec -> class) continue;

		if (jptr -> dicid != jrec -> dicid) continue;

		if (jptr -> jseg != jrec -> jseg) continue;

		if (jptr -> sttofs != jrec -> sttofs) continue;

		if (jptr -> stbofs != jrec -> stbofs) continue;

		makekan(dicbuf + kptr -> offs, kbuf2, TRUE);

		if (sameknj(kbuf1, jrec -> jlen, kbuf2, jptr -> jlen))
			return FALSE;
	}

	return TRUE;
}




static int
chrck_numtbl(u_short flg, u_short cond)
{
	u_short	must;

	if (cond = SelNumCond(cond)) {
		if (must = (cond & SELNUMMUST)) {
			if ((flg & must) != must) return FALSE;
			cond &= ~must;
		}
		if (cond && !(cond & flg)) return FALSE;
	}

	return TRUE;
}



int
sel_sjmode(JREC *jrec)
{
	u_short	i;
	u_short	*p;
	u_short	*q;

	switch (jrec -> class) {
	case C_N_ARABIA:
	case C_N_ARACMA:
	case C_N_KAZU:
	case C_N_SUUJI:
	case C_N_NANSUU:
	case C_N_KANKURA:
	case C_N_ARAKURA:
	case C_N_KAZULONG:
	case C_N_KAZULCMA:
	case C_N_SUUJILONG:
		p = q = Selsjadrs(jrec->class - C_N_ARABIA);
		break;

	default:
		return SELARB;
	}

	while ((i = *p++) != SELNUMTERM) {
		if (chrck_numtbl(jrec -> flags, i)) break;
	}

	if (i == SELNUMTERM) i = *q;

	return SelNumFunc(i);
}



static void
cl_numcmn(JREC *jrec, CLREC *clrec)
{
	u_char 	*p;
	int	i;
	u_short	j;
	u_short	*tbl;

	p = cnvstart;
	if ((i = jrec -> sttofs) == SETTOU_DAI)
		p += SttYomiLen(Settou_ptr(i));
	else if (i == SETTOU_KIGOU)
		p++;

	if (jrec -> jofsst) {
		i = sel_sjmode(jrec);

		if (seldict(jrec -> dicid)) {
			(*curdict->getdic)(curdict, jrec -> jseg);

			p = dicbuf + jrec->jofsst + 1;

			while (*p != HINSIBLKTERM) {
				setkouho(clrec, (TypeDicOfs)(p - dicbuf), i);
				p = skipkstr(p);
			}
		}
		else {
			setkouho(clrec, (TypeDicOfs)1, i);
		}
	}
	else {
		tbl = Selsjadrs(jrec->class - C_N_ARABIA);
		while ((j = *tbl++) != SELNUMTERM) {
			if (!chrck_numtbl(jrec -> flags, j)) continue;

			setkouho(clrec, (TypeDicOfs)0, (int)SelNumFunc(j));
		}
	}
}
