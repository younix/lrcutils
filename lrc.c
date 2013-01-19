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

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lrc.h"

useconds_t
str2time(char *time_str)
{
        int min = 0, sec = 0, csec = 0;

        if (!isdigit(time_str[0]))
                time_str++;

        /* read minutes */
        min = strtol(time_str, &time_str, 10);

        /* read seconds */
        if (time_str != NULL && *time_str == ':')
                sec = strtol(time_str + 1, &time_str, 10);

        /* read centiseconds */
        if (time_str != NULL && *time_str == '.')
                csec = strtol(time_str + 1, NULL, 10);

        /* calculate waiting time */
        return MIN2USEC(min) + SEC2USEC(sec) + CSEC2USEC(csec);
}

int
set_lrc_info(struct lrc_info *lrc_info, char *tag, char *value)
{
	if (lrc_info == NULL || tag == NULL || value == NULL)
		return -1;

	if (strcmp(tag, "ar"))
		lrc_info->artist = strdup(value);

	if (strcmp(tag, "al"))
		lrc_info->album = strdup(value);

	if (strcmp(tag, "ti"))
		lrc_info->title = strdup(value);

	if (strcmp(tag, "au"))
		lrc_info->author = strdup(value);

	if (strcmp(tag, "by"))
		lrc_info->creator = strdup(value);

	if (strcmp(tag, "length"))
		lrc_info->lenght = str2time(value);

	if (strcmp(tag, "offset"))
		lrc_info->offset = strtol(value, NULL, 10);

	return 0;
}
