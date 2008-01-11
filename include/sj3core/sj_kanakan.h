/*
 * Copyright (c) 2004 Iwata <iwata@quasiquote.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef KANAKAN_H
#define KANAKAN_H

#include "sj_kcnv.h"
#include "sj_struct.h"

/*
 * NOTE:
 *  All of these funtions calling rule are not following
 * ANSI like arguement lists, fun(dest, src, ...).
 * There are fun(src, dest, ...).
 */

/* adddic.c */
u_int adddic(u_char *, u_char *, TypeGram);

/* addelcmn.c */
void set_size(u_char *, int, int, int);
u_int addel_arg(u_char *, u_char *, TypeGram, u_char *, int);
int cvtknj(u_char *, u_char *, u_char *);
int srchkana(u_char **, int *);
int srchgram(u_char *, u_char **, TypeGram);
int srchkanji(u_char **, u_char *, int);
TypeIdxOfs count_uidx(void);
void chg_uidx(TypeDicSeg, u_char *, int);

/* alloc.c */
JREC *alloc_jrec(void);
void free_jrec(JREC *);
CLREC *alloc_clrec(void);
void free_clrec(CLREC *);

/* charsize.c */
int codesize(u_char);

/* chrtbl.c */
extern u_char chrtbl[256];

/* cl2knj.c */
int cl2knj(u_char *, int, u_char *);
int nextcl(u_char *, int);
int prevcl(u_char *, int);
int selectnum(void);

/* clstudy.c */
int clstudy(u_char *, u_char *, STDYOUT *);
void delclsub(u_char *);
void mkclidx(void);

/* cmpstr.c */
int cmpstr(u_char *, u_char *);

/* conjunc.c */
extern u_char *conjadr[];

/* connect.c */
extern u_char connadr[];
extern u_char *rigtadr[];

/* cvtclknj.c */
void cvtclknj(void);

/* cvtdict.c */
void cvtdict(KHREC *, CLREC *, int);
void cvtminasi(int);
void cvtwakachi(CLREC *);

/* cvtkanji.c */
void cvtphknj(void);
void cvtkouho(KHREC *);
void setstyrec(KHREC *);

/* deldic.c */
u_int deldic(u_char *, u_char *, TypeGram);

/* dict.c */
void get_askknj(void);
int seldict(TypeDicID);
u_char *get_idxptr(TypeDicSeg);

/* functbl.c */
extern VFunc cvtnum_func[];
extern UCPFunc getkan_func[];
extern IFunc setjrec_func[];
extern IFunc hiraknj_func[];

/* fuzoku.c */
void setclrec(JREC *, u_char *, TypeCnct);
RECURS void srchfzk(JREC *, u_char *, TypeCnct, int);

/* fzkyomi.c */
extern u_char *fzkadr[];

/* getkanji.c */
u_char *getkan_none(u_char *, u_char *, u_char *, int, int);
u_char *getkan_hira(u_char *, u_char *, u_char *, int, int);
u_char *getkan_kata(u_char *, u_char *, u_char *, int, int);
u_char *getkan_knj(u_char *, u_char *, u_char *, int, int);
u_char *getkan_ofs(u_char *, u_char *, u_char *, int, int);
u_char *getkan_norm(u_char *, u_char *, u_char *, int, int);
u_char *getkan_ascii(u_char *, u_char *, u_char *, int, int);
int getkanji(u_char *, int, u_char *, u_char *);

/* getrank.c */
void getrank(void);

/* hzstrlen.c */
int euc_codesize(u_char c);
int hzstrlen(u_char *, int);

/* init.c */
void seg_count(DICT *);
void mkidxtbl(DICT *);
void initwork(void);

/* istrcmp.c */
int istrcmp(u_char *, u_char *, int, int);

/* memcpy.c */
void sstrncpy(u_char *, u_char *, int);

/* memory.c */
JREC *free_jlst(JREC *);
CLREC *free_clst(CLREC *, int);
void free_clall(CLREC *);
void free_jall(JREC *);
void freework(void);

/* mk2claus.c */
void mk2claus(void);

/* mkbunset.c */
void mkbunsetu(void);
CLREC *argclrec(int);

/* mkjiritu.c */
void mkjiritu(int);
JREC *argjrec(int, JREC *);

/* mkkouho.c */
void mkkouho(void);
u_char *makekan_none(u_char *, u_char *, int);
u_char *makekan_1byte(u_char *, u_char *, int);
u_char *makekan_knj(u_char *, u_char *, int);
u_char *makekan_ofs(u_char *, u_char *, int);
u_char *makekan_norm(u_char *, u_char *, int);
u_char *makekan_ascii(u_char *, u_char *, int);
int sel_sjmode(JREC *);

/* mknumber.c */
void num_type00(u_char *, u_char *, JREC *);
void num_type01(u_char *, u_char *, JREC *);
void num_type02(u_char *, u_char *, JREC *);
void num_type03(u_char *, u_char *, JREC *);
void num_type04(u_char *, u_char *, JREC *);
void num_type05(u_char *, u_char *, JREC *);
void num_type06(u_char *, u_char *, JREC *);
void num_type07(u_char *, u_char *, JREC *);
void num_type08(u_char *, u_char *, JREC *);
void num_type09(u_char *, u_char *, JREC *);
void num_type10(u_char *, u_char *, JREC *);
void num_type11(u_char *, u_char *, JREC *);
void num_type12(u_char *, u_char *, JREC *);
void num_type13(u_char *, u_char *, JREC *);
void num_type14(u_char *, u_char *, JREC *);

/* mvmemd.c */
void mvmemd(u_char *, u_char *, int);

/* mvmemi.c */
void mvmemi(u_char *, u_char *, int);

/* peepdic.c */
int getusr(u_char *);
int nextusr(u_char *);
int prevusr(u_char *);

/* ph2knj.c */
int ph2knj(u_char *, u_char *, int);

/* ph_khtbl.c */
int ph_khtbl(CLREC *);

/* priority.c */
int priority(CLREC *);
void pritiny(void);

/* prtytbl.c */
extern u_char taipri[7][45];
extern u_char sttpri[5][31];

/* s2ctbl.c */
extern u_char kigou[];

/* sdepend.c */
DictFile *opendict(char *, char *);
int closedict(DictFile *);
void lock_dict(DictFile *, int);
void unlock_dict(DictFile *, int);
int is_dict_locked(DictFile *);
StdyFile *openstdy(char *, char *);
int closestdy(StdyFile *);
int putstydic(void);
int putcldic(void);
int makedict(char *, int, int, int);
int makestdy(char *, int, int, int);
void sj_closeall(void);
int set_dictpass(DictFile *, char *);
int set_stdypass(char *);
int set_dictcmnt(DictFile *, char *);
int set_stdycmnt(char *);
int get_stdysize(int *, int *, int *);

/* selclrec.c */
void selclrec(void);

/* selsuuji.c */
extern u_short *selsjadrs[];

/* setconj.c */
int setconj(TypeGram, JREC *, CREC *);

/* setjrec.c */
int setj_atrb(u_char *);
int setj_ofs(u_char *);
int setj_knj(u_char *);
int setj_norm1(u_char *);
int setj_norm2(u_char *);
int setj_norm3(u_char *); /* unused??? */
void setjrec(u_char *, int);
void setnumrec(u_char *, JREC *, TypeGram);
void setcrec(u_char *);

/* setkouho.c */
void  setkouho(CLREC *, TypeDicOfs, int);
void ph_setkouho(CLREC *, TypeDicOfs, STDYIN *);
int hiraknj_atrb(u_char *, int *);
int hiraknj_ofs(u_char *, int *);
int hiraknj_knj(u_char *, int *);
int hiraknj_hask(u_char *, int *);
int hiraknj_kask(u_char *, int *);
int hiraknj_norm(u_char *, int *);
int hiraknj_hira(u_char *, int *);

/* setubi.c */
u_char *getstb(TypeGram);
void setubi(JREC *, u_char *);

/* sj2code.c */
int sj2cd_chr(u_char *, u_char *);
int sj2cd_str(u_char *, u_char *, int);

/* skiphblk.c */
u_char *skiphblk(u_char *);

/* skipkstr.c */
u_char *skipkstr(u_char *);

/* srchdict.c */
int yomicmp(u_char *, u_char *, u_char *);
u_char *srchdict(u_char *);

/* srchhead.c */
int srchhead(void);

/* srchidx.c */
TypeDicSeg srchidx(TypeDicSeg, int);

/* srchnum.c*/
void srchnum(void);
void setwdnum(u_char *, int, u_short *);
int setucnum(u_char *, int, u_char *);

/* sstrcmp.c */
int sstrcmp(u_char *, u_char *);

/* sstrlen.c */
int sstrlen(u_char *);

/* sstrncmp.c */
int sstrncmp(u_char *, u_char *dst, int);

/* stbtbl.c */
extern u_char *stbadr[];

/* stttbl.c */
extern u_char *settou_ptr[];
extern u_char scncttbl[2][24];

/* study.c */
int study(STDYOUT *);
STDYIN  *srchstdy(TypeDicSeg, TypeDicOfs, TypeDicID);

/* suujitbl.c */
extern u_char suuji_tbl[];
extern u_char kurai1_tbl[];
extern u_char kurai2_tbl[];
extern u_char num1tbl[];
extern u_char num2tbl[];
extern u_char num3tbl[];
extern u_char num4tbl[];
extern u_char num5tbl[];
extern u_char num6tbl[];

/* terminat.c */
int terminate(TypeCnct, u_char *);

/* termtbl.c */
extern u_char termtbl[];

/* wakachi.c */
void wakachi(void);


#endif /* KANAKAN_H */

