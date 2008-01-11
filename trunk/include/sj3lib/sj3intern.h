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

#ifndef SJ3INTERN_H
#define SJ3INTERN_H

#include "sj3lowlib.h"

/* level1.c */
void sj3_set_timeout(int);
int sj3_make_connection(SJ3_CLIENT_ENV *, char *, char *, char *);
int sj3_erase_connection(SJ3_CLIENT_ENV *);
long sj3_open_dictionary(SJ3_CLIENT_ENV *, char *, char *);
int sj3_close_dictionary(SJ3_CLIENT_ENV *, long);
int sj3_open_study_file(SJ3_CLIENT_ENV *, char *, char *);
int sj3_close_study_file(SJ3_CLIENT_ENV *);
int sj3_get_id_size(SJ3_CLIENT_ENV *);
int sj3_lock_server(SJ3_CLIENT_ENV *);
int sj3_unlock_server(SJ3_CLIENT_ENV *);
int sj3_ikkatu_henkan(SJ3_CLIENT_ENV *, u_char *, u_char *, int, int);
int sj3_bunsetu_henkan(SJ3_CLIENT_ENV *, u_char *, int, u_char *, int);
int sj3_bunsetu_jikouho(SJ3_CLIENT_ENV *, u_char *, int, int);
int sj3_bunsetu_maekouho(SJ3_CLIENT_ENV *, u_char *, int, int);
int sj3_bunsetu_kouhosuu(SJ3_CLIENT_ENV *, u_char *, int, int);
int sj3_bunsetu_zenkouho(SJ3_CLIENT_ENV *, u_char *, int, SJ3_DOUON *, int);
int sj3_tango_gakusyuu(SJ3_CLIENT_ENV *, SJ3_STUDYREC *);
int sj3_bunsetu_gakusyuu(SJ3_CLIENT_ENV *, u_char *, u_char *, SJ3_STUDYREC *, int);
int sj3_tango_touroku(SJ3_CLIENT_ENV *, long, u_char *, u_char *, int, int);
int sj3_tango_sakujo(SJ3_CLIENT_ENV *, long, u_char *, u_char *, int, int);
int sj3_tango_syutoku(SJ3_CLIENT_ENV *, int, u_char *, int);
int sj3_tango_jikouho(SJ3_CLIENT_ENV *, int, u_char *, int);
int sj3_tango_maekouho(SJ3_CLIENT_ENV *, int, u_char *, int);
int sj3_make_dict_file(SJ3_CLIENT_ENV *, char *, int, int, int);
int sj3_make_study_file(SJ3_CLIENT_ENV *, char *, int, int, int);
int sj3_make_directory(SJ3_CLIENT_ENV *, char *);
int sj3_access(SJ3_CLIENT_ENV *, char *, int);
int sj3_who(SJ3_CLIENT_ENV *, SJ3_WHO_STRUCT *, int);
int sj3_quit(SJ3_CLIENT_ENV *);
int sj3_kill(SJ3_CLIENT_ENV *);
int sj3_version(SJ3_CLIENT_ENV *, char *, int);

#endif /* SJ3INTERN_H */
