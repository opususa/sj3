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
 * $SonyRCSfile: sj3lib.h,v $  
 * $SonyRevision: 1.1 $ 
 * $SonyDate: 1994/06/03 08:03:57 $
 */


#ifndef SJ3LIB_H
#define SJ3LIB_H

#include <sys/types.h>

#define SJ3_NORMAL_END         0
#define SJ3_SERVER_DEAD        (1 << 0)



#define SJ3_CONNECT_ERROR      (1 << 1)
#define SJ3_ALREADY_CONNECTED  (1 << 2)
#define SJ3_CANNOT_OPEN_MDICT  (1 << 3)
#define SJ3_CANNOT_OPEN_UDICT  (1 << 4)
#define SJ3_CANNOT_OPEN_STUDY  (1 << 5)
#define SJ3_CANNOT_MAKE_UDIR   (1 << 6)
#define SJ3_CANNOT_MAKE_UDICT  (1 << 7)
#define SJ3_CANNOT_MAKE_STUDY  (1 << 8)



#define SJ3_DISCONNECT_ERROR   (1 << 1)
#define SJ3_NOT_CONNECTED      (1 << 2)
#define SJ3_NOT_OPENED_MDICT   (1 << 3)
#define SJ3_NOT_OPENED_UDICT   (1 << 4)
#define SJ3_NOT_OPENED_STUDY   (1 << 5)
#define SJ3_CLOSE_MDICT_ERROR  (1 << 6)
#define SJ3_CLOSE_UDICT_ERROR  (1 << 7)
#define SJ3_CLOSE_STUDY_ERROR  (1 << 8)



#define SJ3_DICT_ERROR      1
#define SJ3_DICT_LOCKED     2
#define SJ3_BAD_YOMI_STR    3
#define SJ3_BAD_KANJI_STR   4
#define SJ3_BAD_HINSI_CODE  5

#define SJ3_WORD_EXIST      6
#define SJ3_DOUON_FULL      7
#define SJ3_DICT_FULL       8
#define SJ3_INDEX_FULL      9
#define SJ3_TOUROKU_FAILED  10

#define SJ3_WORD_NOT_EXIST  6
#define SJ3_SYOUKYO_FAILED  10




#define SJ3_IKKATU_YOMI     512




#define SJ3_BUNSETU_YOMI    128
#define SJ3_BUNSETU_KANJI   512




#ifndef SJ3_WORD_ID_SIZE
# define SJ3_WORD_ID_SIZE   32
#endif

typedef struct studyrec {
 unsigned char dummy[SJ3_WORD_ID_SIZE];
} SJ3_STUDYREC;




typedef struct bunsetu {
 int srclen;
 int destlen;
 unsigned char *srcstr;
 unsigned char *deststr;
 struct studyrec dcid;
} SJ3_BUNSETU;



typedef struct douon {
 unsigned char ddata[SJ3_BUNSETU_KANJI];
 int  dlen;
 SJ3_STUDYREC dcid;
} SJ3_DOUON;



#define SJ3_H_NRMNOUN 1
#define SJ3_H_PRONOUN 12
#define SJ3_H_LNAME   21
#define SJ3_H_FNAME   22
#define SJ3_H_LOCNAME 24
#define SJ3_H_PREFIC  25
#define SJ3_H_RENTAI  26
#define SJ3_H_CONJUNC 27
#define SJ3_H_SUBNUM  29
#define SJ3_H_NUMERAL 30
#define SJ3_H_PREFIX  31
#define SJ3_H_POSTFIX 36
#define SJ3_H_ADVERB  45
#define SJ3_H_ADJECT  60
#define SJ3_H_ADJVERB 71
#define SJ3_H_SILVERB 80
#define SJ3_H_ZILVERB 81
#define SJ3_H_ONEVERB 90
#define SJ3_H_KAVERB  91
#define SJ3_H_GAVERB  92
#define SJ3_H_SAVERB  93
#define SJ3_H_TAVERB  94
#define SJ3_H_NAVERB  95
#define SJ3_H_BAVERB  96
#define SJ3_H_MAVERB  97
#define SJ3_H_RAVERB  98
#define SJ3_H_WAVERB  99
#define SJ3_H_SINGLE  189

/* sj3.c */
int sj3_open(char *, char *);

int sj3_open_with_list(char *, char *, int, char **, int *, int **);

int sj3_close();

int sj3_getkan(u_char *, SJ3_BUNSETU *, u_char *, int);
int sj3_getkan_euc(u_char *, SJ3_BUNSETU *, u_char *, int);
int sj3_getkan_mb(u_char *, SJ3_BUNSETU *, u_char *, int);

int sj3_douoncnt(u_char *);
int sj3_douoncnt_euc(u_char *);
int sj3_douoncnt_mb(u_char *);

int sj3_getdouon(u_char *, SJ3_DOUON *);
int sj3_getdouon_euc(u_char *, SJ3_DOUON *);
int sj3_getdouon_mb(u_char *, SJ3_DOUON *);

int sj3_gakusyuu(SJ3_STUDYREC *);

int sj3_gakusyuu2(u_char *, u_char *, SJ3_STUDYREC *);
int sj3_gakusyuu2_euc(u_char *, u_char *, SJ3_STUDYREC *);
int sj3_gakusyuu2_mb(u_char *, u_char *, SJ3_STUDYREC *);

int sj3_touroku(u_char *, u_char *, int);
int sj3_touroku_euc(u_char *, u_char *, int);
int sj3_touroku_mb(u_char *, u_char *, int);

int sj3_syoukyo(u_char *, u_char *, int);
int sj3_syoukyo_euc(u_char *, u_char *, int);
int sj3_syoukyo_mb(u_char *, u_char *, int);

int sj3_getdict(u_char *);
int sj3_getdict_euc(u_char *);
int sj3_getdict_mb(u_char *);

int sj3_nextdict(u_char *);
int sj3_nextdict_euc(u_char *);
int sj3_nextdict_mb(u_char *);

int sj3_prevdict(u_char *);
int sj3_prevdict_euc(u_char *);
int sj3_prevdict_mb(u_char *);

int sj3_lockserv();

int sj3_unlockserv();

extern int sj3_error_number;

#endif /* SJ3LIB_H */
