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

#ifndef SJ3MKDIC_H
#define SJ3MKDIC_H

#include <stdio.h>

#include "sj3_dict_struct.h"

/* char.c */
int cnvyomi(int);
int h2kcode(int);
int codesize(u_char);
void output_knj(FILE *, u_char *, int);
void output_str(FILE *, char *);
void output_int(FILE *, int *);
int yomi2zen(int);
void output_yomi(FILE *, u_char *);

/* cnvhinsi.c */
int u_strcmp(u_char *, u_char *);
int cnvhinsi(u_char *);
char *hinsi_str(int);

/* file.c */
FILE *Fopen(char *, char *);
void Fclose(FILE *);
int Fseek(FILE *, long, int);
long Ftell(FILE *);
long Fsize(char *);
size_t Fread(char *, int, int, FILE *);
size_t Fwrite(char *, int, int, FILE *);
int Fgetc(FILE *);
int Fflush(FILE *);

/* global.c */
extern DouonRec *douon_ptr;
extern int douon_num;
extern int yomi_len;
extern int hinsi_num;
extern int kanji_len;
extern HindoRec *hindo[MAXHINDONUMBER];
extern int hindo_num;
extern OffsetRec *ofsrec[MAXOFFSETNUMBER];
extern int ofsrec_num;
extern AssyukuRec *assyuku;
extern HindoRec *askknj[MAXKNJASKNUMBER];
extern int askknj_num;
extern int kanji_num;

/* hindo.c */
int check_hindo(u_char *, int);
void knjhnd_set(u_char *, int);
void knjhnd_reset(u_char *, int);
int decide_knjask();
void clear_hindo();
void clear_assyuku();

/* knjcvt.c */
u_char *knjofscvt(u_char *, int, int *);
u_char *knjcvt(u_char *, int, int *);

/* mkdict.c */
extern FILE *infp;
extern FILE *outfp;
char *make_idxlist(char *);
char *get_idxlist(char *);
void parse(int, char **);
int get_number(char *);
void usage(char *);
int getch();
void mark_file(FILE *);

/* makelist */
void flush_douon();
void makelist(int *, int *, int, int *);

/* makeseg.c */
int make_knjstr(u_char *, int, u_char *);
void makeseg();
void makehead(u_char *);

/* memory.c */
void *Malloc(unsigned);
void *Zalloc(int);
void Free(void *);

/* offset.c */
void set_ofsrec(u_char *, int, int);
int isknjexist(u_char *, int);
void clear_ofsrec();
OffsetRec *real_ofsrec(u_char *);

/* readline.c */
int *readline();
void setline(int (*)());

/* string.c */
int bubun_str(u_char *, int, u_char *, int);
int overlap_str(u_char *, int, u_char *, int);
int istrlen(int *);
int istrcmp(int *, int *);
int top_strcmp(int *, int *);
int last_strcmp(int *, int *);
int string_cmp(u_char *, int, u_char *, int);

#endif /* SJ3MKDIC_H */


