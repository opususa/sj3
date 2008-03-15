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

#ifndef SJ3PROXY_H
#define SJ3PROXY_H

#include <stdio.h>
#include <netdb.h>
#include <sys/param.h>

extern int chroot_enable;
extern char address_family_str[256];
extern int address_family;
extern char chuser_name[MAXLOGNAME];
extern char chroot_dir[MAXPATHLEN];
extern char socket_file[MAXPATHLEN];
extern char host_name[NI_MAXHOST];
extern char service_name[NI_MAXSERV];

void set_priv(const char *);

#endif

