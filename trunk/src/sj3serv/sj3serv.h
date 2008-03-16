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

#ifndef SJ3SERV_H
#define SJ3SERV_H

#include <setjmp.h>

#include "sj_kanakan.h"

typedef struct {
	uid_t uid;
	gid_t gid;
} sj3_ugid;

/* auth.c */

int priv_init();

int priv_getugid(char *, sj3_ugid *);

/* comuni.c */
extern int client_num;
extern Client *client;
extern Client *cur_client;
extern int maxfds;
extern fd_set fds_org;
extern jmp_buf client_dead;
extern int client_fd;

void socket_init();
void open_socket();
void close_socket();
void communicate();
void put_flush();
void put_byte(int);
void put_work(int);
void put_int(int);
u_char *put_string(u_char *);
u_char *put_ndata(u_char *, int);
void get_buf();
int get_byte();
int get_word();
int get_int();
int get_nstring(u_char *, int);
u_char *get_ndata(u_char *, int);

/* execute.c */
int make_full_path(char *, int);
WorkArea *alloc_workarea();
void free_workarea(WorkArea *);
void exec_connect();
void exec_disconnect();
void exec_opendict();
void exec_closedict();
void close_dictlist(DICTL *);
void exec_openstdy();
void exec_closestdy();
void exec_stdysize();
void exec_ph2knj(int);
void exec_cl2knj(int);
void exec_nextcl(int);
void exec_prevcl(int);
void exec_cl2knj_cnt(int);
void exec_cl2knj_all(int);
void exec_study();
void exec_clstudy(int);
void exec_adddict(int);
void exec_deldict(int);
void exec_getdict(int);
void exec_nextdict(int);
void exec_prevdict(int);
void exec_makedict();
void exec_makestdy();
void exec_access();
void exec_makedir();
void exec_who();
void exec_kill();
void exec_quit();
void exec_version();
void exec_dictpass();
void exec_dictcmnt();
void exec_stdypass();
void exec_stdycmnt();
void exec_stdypara();
void execute_cmd();

/* error.c */
int open_error();
void error_out(const char *, ...);
void warning_out(const char *, ...);
int open_log();
void logging_out(const char *, ...);
int open_debug();
void debug_out(int, const char *, ...);

/* main.c */
void server_terminate();

extern char program_name[MAXPATHLEN];
extern char runcmd_file[MAXPATHLEN];
extern int daemon_enable;
extern char pid_file[MAXPATHLEN];
extern char chuser_name[MAXLOGNAME];
extern int chroot_enable;
extern char chroot_dir[MAXPATHLEN];
extern int strict_auth_enable;
extern char domain_socket_name[MAXPATHLEN];
extern char dict_dir[MAXPATHLEN];
extern char dict_file[FILENAME_MAX];
extern char log_file[MAXPATHLEN];
extern char debug_file[MAXPATHLEN];
extern int debug_level;
extern char error_file[MAXPATHLEN];
extern int max_client;
extern int dir_mode;
extern int file_mode;
int read_line(FILE *, char *, int);
void read_runcmd();
void set_default();
void parse_arg(int, char **);
void preload_dict();
void preopen_dict();
int check_user(char *, char *);

/* priv.c */
void set_priv(const char *);

/* timestamp.c */
extern char *time_stamp;

/* version.c */
extern char *version_number;


#endif
