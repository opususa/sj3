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

#ifndef SJ3DIC_H
#define SJ3DIC_H

#include <config.h>
#include <stdio.h>
#include <sys/types.h>
#include "sjtool.h"

/* codecnv.h */
int init_code();
void cnvcode(unsigned char *);
void printout_mb(FILE *, unsigned char *);
void printout(FILE *, unsigned char *);
void normal_out(char *, ...);
void error_out(char *, ...);

/* dictdisp.c */
void dictdisp(char *);

/* dictmake.c */
void dictmake(char *);

/* hinsi.c */
char *hns2str(int);
int str2hns(char *);

/* sj3dic.c */
extern int sj3dic_sys_code;
extern char prog_name[LONGLENGTH];
extern char home_dir[LONGLENGTH];
extern char user_name[LONGLENGTH];
extern char term_name[LONGLENGTH];
extern char serv_name[LONGLENGTH];
extern char file_name[LONGLENGTH];
extern char dict_name[LONGLENGTH];
extern int verbose_flag;
extern int init_flag;
extern int force_flag;
void init_env();
void usage(int);
int parsearg(int, char **);
void make_dicname();
void setdicname(char *);
void setsjserv(char *);

/* sj3err.c */
void sj3error(FILE *, int);

/* sjrc.c */
struct wordent {
 char word_str[MAXWORD];
};
extern char RCfile[LONGLENGTH];
extern struct functbl {
 char *keyword;
 void (*func)(struct wordent *);
} funcs[];
int sjset_code();
int getsjrc();
void setrc (char *, FILE *);
int much(char *, char *);
int getword(char *, struct wordent *);
int IsTerminator(u_char);
int isTerminator(u_char);
int IsEscape(u_char);
int IsDelimitor(u_char);
void set_dict (struct wordent *);
void set_server(struct wordent *);

#endif /* SJ3DIC_H */

