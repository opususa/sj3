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
 * $SonyRCSfile: srchnum.c,v $  
 * $SonyRevision: 1.1 $ 
 * $SonyDate: 1994/06/03 08:02:30 $
 */

#include <string.h>
#include "sj_kcnv.h"
#include "sj_yomi.h"
#include "sj_hinsi.h"
#include "sj_suuji.h"

#include "sj_kanakan.h"

static void	srch_kurai1(), srch_kurai2();

static void
srch_josuu_sub(JREC *jrec, TypeGram gram)
{
	u_char	*tagp;
	DICTL	*dp;

	for (dp = dictlist ; dp ; dp = dp -> next) {
		curdict = dp -> dict;
		dicinl  = 1;
		dicsaml = 0;
		prevseg = -1;
		while (tagp = srchdict(tagp)) setnumrec(tagp, jrec, gram);
	}
}

static void
srch_josuu(JREC *jrec)
{
	u_char	*kp;
	int	klen;
	int	len;

	if (headcode && headcode != SETTOU_DAI) return;

	kp   = cnvstart;
	klen = cnvlen;

	cnvstart += (len = jrec -> numlen);
	cnvlen   -= len;

	srch_josuu_sub(jrec, JOSUUSI);

	if (!headcode) srch_josuu_sub(jrec, JOSUUSI2);

	cnvstart = kp;
	cnvlen   = klen;
}

static void
setnrec_sub(u_char *p, u_short flag, int stb)
{
	JREC	*rec;
	int	len1;
	int	len2;

	len1 = len2 = p - suuji_yptr;
	if (headcode) len1 += headlen;
	if (stb) len2--;

	if ((int)suuji_len == len2) {
		if (suuji_wkeep) {
			memcpy((u_char *)suuji_wkeep, (u_char *)suuji_wbuf,
				sizeof(suuji_wbuf[0]) * NUMWORDBUF);
		}
		if (suuji_ukeep) {
			memcpy(suuji_ukeep, suuji_ubuf, NUMKETALENGTH);
		}
		suuji_exit = suuji_keta;
		return;
	}

	if (rec = argjrec(len1, (JREC *)NULL)) {
		rec -> class  = suuji_class;
		rec -> hinsi  = SUUSI;
		rec -> sttofs = headcode;
		rec -> stbofs = (u_char)stb;
		rec -> flags  = flag;
		rec -> numlen = (u_char)len2;

		if (!stb) srch_josuu(rec);
	}
}

static void
setnrec(u_char *p, u_short flag)
{
	setnrec_sub(p, flag, 0);

	if (!headcode && (*p == Y_S_KIGOU2)) {
		setnrec_sub(p + 1, flag, -1);
	}
}

static u_char*
srchtbl(u_char ch, u_char *tbl, int rec, int n)
{
	int	high, low, mid;
	u_char	*p;

	if (!ch) return NULL;

	low = 0;
	high = n - 1;

	while (low <= high) {
		mid = (low + high) / 2;
		p = tbl + mid * rec;

		if (ch == *p)
			break;

		else if (ch < *p)
			high = mid - 1;

		else
			low = mid + 1;
	}

	if (low > high) return NULL;

	while (mid-- > 0 && ch == *(p - rec)) p -= rec;

	return p;
}

static int
isconnect(u_char *cnct, int num)
{
	return (cnct[num / 8] & (0x80 >> (num % 8)));
}

static int
string_cmp(u_char *s, int l, u_char *d)
{
	u_char	*p;

	p = s;

	while (*s && l-- > 0) {
		if (*s != *d) return 0;
		s++;
		d++;
	}

	return (s - p);
}

static int
check_num(u_char *ptr)
{
	int	i;
	int	j;
	int	k;
	int	tmp;
	int	hketa;
	int	lketa;
	int	flg = FALSE;
	int	cnt;
	u_short	flag;

	memset((u_char *)suuji_wbuf, 0, sizeof(u_short) * NUMWORDBUF);
	flag = 0;

	hketa = lketa = cnt = 0;
	for (i = suuji_keta ; --i >= 0 ;) {
		if (hketa >= 4) return FALSE;
		tmp = suuji_ubuf[i];
		if (j = ((tmp >> 6) & 3)) {
			if (j < hketa) return FALSE;
			if (j > hketa) {
				flag |= JFLAG_N01;
				do {
					suuji_wbuf[hketa] |= (_NUM0<<(lketa*4));
					if (++lketa >= 4) {
						lketa = 0;
						hketa++;
					}
				} while (j > hketa);
			}
		}
		k = ((tmp >> 4) & 3);
		if (j || k) {
			if (k < lketa) return FALSE;
			if (k > lketa) {
				flag |= JFLAG_N01;
				do {
					suuji_wbuf[hketa] |= (_NUM0<<(lketa*4));
					lketa++;
				} while (k > lketa);
			}
			flg = TRUE;
		}
		tmp &= 0x0f;
		suuji_wbuf[hketa] |= (tmp << (lketa * 4));
		cnt++;

		j = (1 << tmp);
		if (j & NUMBERS1) flag |= JFLAG_NS1;
		if (j & NUMBER01) flag |= JFLAG_N01;
		if (j & NUMBER23) flag |= JFLAG_N23;

		if (tmp != _NUM0) {
			flg = FALSE;

			switch (lketa) {
			case 0:
				if (tmp == _NUM1) flag |= JFLAG_N_1;
				break;

			case 1:
				flag |= JFLAG_N10;
				break;
			}
		}
		if (++lketa >= 4) {
			lketa = 0;
			hketa++;
		}
	}

	if (flg) return FALSE;

	if (suuji_keta == 1 || tmp != _NUM0) flag |= JFLAG_NN0;

	if (hketa)
		flag |= (JFLAG_NK4 | JFLAG_NK2);
	else if (lketa > 1)
		flag |= JFLAG_NK2;

	if (cnt > 1) flag |= JFLAG_NSN;

	setnrec(ptr, flag);

	return TRUE;
}

static void
srch_number1(u_char *ptr)
{
	u_char		ch;
	u_char		mode;
	u_char		*p;
	int		l;
	TypeClass	cls;
	int		len;
	u_char		*tptr;

	if (suuji_keta >= NUMKETALENGTH) return;

	if (!(p = srchtbl((ch = *ptr), Suuji_tbl, SJTBLRECLEN, SJTBLRECNUM)))
		return;

	len = suuji_yptr + MAXCLINPUTLEN - ptr;

	for ( ; *SjTblYPos(p) == ch ; p += SJTBLRECLEN) {
		if (!(l = string_cmp(SjTblYPos(p), SJTBLYLEN, ptr))) continue;

		if (l > len) continue;

		mode = *SjTblModeP(p);
		suuji_ubuf[suuji_keta++] = SjTblNumber(mode);
		tptr = ptr + l;

		switch (suuji_class) {
		case C_N_ARABIA:
			if (!(mode & SJTBLNARABIAF)) break;

			check_num(tptr);
			srch_number1(tptr);
			srch_kurai2(tptr, (u_char *)NULL);

			break;

		case C_N_SUUJI:
			if (mode & (SJTBLNARABIAF | SJTBLNYOMIF)) break;

			check_num(tptr);
			srch_number1(tptr);
			srch_kurai1(tptr, (u_char *)NULL);
			srch_kurai2(tptr, (u_char *)NULL);

			break;

		case C_N_KAZU:
		case C_N_NANSUU:
			if (mode & SJTBLNNARABIF) break;
			if (mode & SJTBLNNANSUUF) {
				cls = suuji_class;
				suuji_class = C_N_NANSUU;
			}

			check_num(tptr);
			srch_kurai1(tptr, SjTblCnctP(p));
			srch_kurai2(tptr, SjTblCnctP(p));

			if (mode & SJTBLNNANSUUF) suuji_class = cls;
			break;

		default:
			if (mode & SJTBLNARABIAF) {
				suuji_class = C_N_ARABIA;
				check_num(tptr);
				srch_number1(tptr);
				srch_kurai2(tptr, (u_char *)NULL);

				suuji_class = C_N_KAZU;
			}
			else if (mode & SJTBLNYOMIF) {
				if (mode & SJTBLNNANSUUF)
					suuji_class = C_N_NANSUU;
				else
					suuji_class = C_N_KAZU;
				check_num(tptr);
			}
			else if (mode & SJTBLNNARABIF) {
				suuji_class = C_N_SUUJI;
				check_num(tptr);
				srch_number1(tptr);
				break;
			}
			else {
				suuji_class = C_N_SUUJI;
				srch_number1(tptr);

				suuji_class = C_N_KAZU;
				check_num(tptr);
			}
			srch_kurai1(tptr, SjTblCnctP(p));
			srch_kurai2(tptr, SjTblCnctP(p));
			break;
		}

		suuji_keta--;
	}
}

static void
srch_kurai1(u_char *ptr, u_char *cnct)
{
	u_char		ch;
	u_char		*p;
	int		l;
	int		keta;
	int		mode;
	TypeClass	cls;
	int		len;
	u_char		*tptr;

	if (!(p = srchtbl((ch = *ptr), Kurai1_tbl, KR1TBLRECLEN, KR1TBLRECNUM)))
		return;

	len = suuji_yptr + MAXCLINPUTLEN - ptr;

	for ( ; *Kr1TblYPos(p) == ch ; p += KR1TBLRECLEN) {
		if (!(l = string_cmp(Kr1TblYPos(p), KR1TBLYLEN, ptr))) continue;

		if (l > len) continue;

		mode = *Kr1TblModeP(p);
		keta = Kr1TblKeta(mode);
		tptr = ptr + l;

		if (suuji_class != C_N_KAZU && suuji_class != C_N_NANSUU) {
			cls = suuji_class;
			suuji_class = (suuji_class == C_N_ARABIA) ?
				C_N_ARAKURA : C_N_KANKURA;
			suuji_ubuf[suuji_keta - 1] |= (keta << 4);
			check_num(tptr);
			suuji_class = cls;
			suuji_ubuf[suuji_keta - 1] &= 0xcf;
			continue;
		}

		if (cnct) {
			if (!isconnect(cnct, (int)Kr1TblCnct(mode))) continue;
			suuji_ubuf[suuji_keta - 1] |= (keta << 4);
		}
		else {
			if (!Kr1TblTop(mode)) continue;
			if (suuji_keta >= NUMKETALENGTH) continue;
			suuji_ubuf[suuji_keta++] = (_NUM1 | (keta << 4));
		}

		check_num(tptr);

		srch_number1(tptr);
		srch_kurai1(tptr, (u_char *)NULL);
		srch_kurai2(tptr, Kr1TblCnctP(p));

		if (cnct)
			suuji_ubuf[suuji_keta - 1] &= 0xcf;
		else
			suuji_keta--;
	}
}

static void
srch_kurai2(u_char *ptr, u_char *cnct)
{
	u_char		ch;
	u_char		*p;
	int		l;
	int		keta;
	int		mode;
	TypeClass	cls;
	int		len;
	u_char		*tptr;

	if ((suuji_ubuf[0] & 0x0f) == _NUM0) return;

	if (!(p = srchtbl((ch = *ptr), Kurai2_tbl, KR2TBLRECLEN, KR2TBLRECNUM)))
		return;

	len = suuji_yptr + MAXCLINPUTLEN - ptr;

	for ( ; *Kr2TblYPos(p) == ch ; p += KR2TBLRECLEN) {
		if (!(l = string_cmp(Kr2TblYPos(p), KR2TBLYLEN, ptr))) continue;

		if (l > len) continue;

		mode = *Kr2TblModeP(p);
		keta = Kr2TblKeta(mode);
		tptr = ptr + l;

		if (suuji_class != C_N_KAZU && suuji_class != C_N_NANSUU) {
			suuji_ubuf[suuji_keta - 1] |= (keta << 6);
			cls = suuji_class;
			suuji_class = (suuji_class == C_N_ARABIA) ?
				C_N_ARAKURA : C_N_KANKURA;
			check_num(tptr);
			suuji_class = cls;
			suuji_ubuf[suuji_keta - 1] &= 0x3f;
			continue;
		}

		if (cnct) {
			if (!isconnect(cnct, (int)Kr2TblCnct(mode))) continue;
			suuji_ubuf[suuji_keta - 1] |= (keta << 6);
		}
		else {
			if (suuji_keta >= NUMKETALENGTH) continue;
			suuji_ubuf[suuji_keta++] = (_NUM1 | (keta << 6));
		}

		check_num(tptr);

		srch_number1(tptr);
		srch_kurai1(tptr, (u_char *)NULL);

		if (cnct)
			suuji_ubuf[suuji_keta - 1] &= 0x3f;
		else
			suuji_keta--;
	}
}

static void
srch_number2(u_char *p)
{
	u_char	ch;
	int	i;

	suuji_class = (*p == N_0) ? C_N_SUUJILONG : C_N_KAZULONG;

	suuji_keta = suuji_comma = 0;
	for (i = 0 ; i < MAXCLINPUTLEN ; i++) {
		if ((ch = *p++) == Y_COMMA) {
			if (suuji_keta) {
				if (suuji_comma) {
					if (suuji_comma != 3) break;
				}
				else {
					if (suuji_keta >= 4) break;
					if (*suuji_yptr == N_0) break;
				}
			}
			else
				break;

			suuji_comma = 1;
			ch = *p++;
		}
		else if (suuji_comma) {
			suuji_comma++;
		}

		if (suuji_comma > 3) break;

		if (!(Chrtbl[ch] & NUMBER)) break;

		if (suuji_keta < NUMKETALENGTH) {
			suuji_ubuf[suuji_keta] = ch - N_0 + _NUM0;
		}
		suuji_keta++;

		if (!suuji_comma && suuji_keta > NUMKETALENGTH) {
			setnrec(p, 0);
		}
		else if (suuji_comma == 3) {
			if (suuji_keta <= NUMKETALENGTH) {
				suuji_class = C_N_ARACMA;
				check_num(p);
			}
			else {
				suuji_class = C_N_KAZULCMA;
				setnrec(p, 0);
			}
		}
	}
}

static void
srchnum_sub(u_char *p)
{
	suuji_yptr = p;

	suuji_comma = suuji_keta = 0;
	suuji_class = 0;
	srch_number1(p);

	suuji_keta = 0;
	suuji_class = C_N_KAZU;
	srch_kurai1(p, (u_char *)NULL);
	if (suuji_exit) return;

	srch_number2(p);
}

void
srchnum()
{
	suuji_len = suuji_exit = 0;

	srchnum_sub(cnvstart);

	if (!headcode && (*cnvstart == Y_S_KIGOU1)) {
		headcode = SETTOU_KIGOU;
		headlen = 1;

		srchnum_sub(cnvstart + 1);

		headcode = headlen = 0;
	}
}

void
setwdnum(u_char *p, int len, u_short *wd)
{
	suuji_len   = (u_char)len;
	suuji_wkeep = wd;
	suuji_ukeep = NULL;
	headcode = headlen = 0;
	suuji_exit = 0;

	srchnum_sub(p);
}

int
setucnum(u_char *p, int len, u_char *ud)
{
	suuji_len   = (u_char)len;
	suuji_wkeep = NULL;
	suuji_ukeep = ud;
	headcode = headlen = 0;
	suuji_exit = 0;

	srchnum_sub(p);

	return (int)suuji_exit;
}

