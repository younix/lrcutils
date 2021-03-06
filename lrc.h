/*
 * Copyright (c) 2013 Jan Klemkow <j.klemkow@wemelug.de>
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

#ifndef LRC_H
#define LRC_H

#include <unistd.h>

#define CSEC2USEC(csec) ((csec) * 10000)
#define SEC2USEC(sec)   ((sec) * 1000000)
#define MIN2USEC(min)   (SEC2USEC((min) * 60))
#define USEC2SEC(usec)	((usec) / 1000000)

struct lrc_info {
	char *artist;
	char *album;
	char *title;
	char *author;
	char *creator;

	useconds_t length;
	signed int offset;	/* milliseconds */
};

#define LRC_INFO_NULL {NULL, NULL, NULL, NULL, NULL, 0, 0};

int lrc_usleep(useconds_t usec);
useconds_t str2time(char *time_str);
int set_lrc_info(struct lrc_info *lrc_info, char *tag, char *value);
void print_lrc_info(struct lrc_info *lrc_info);

#endif
