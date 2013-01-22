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

#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "lrc.h"

int
lrc_usleep(useconds_t usec)
{
        struct timespec time;

        time.tv_sec = usec / 1e6;
        time.tv_nsec = (usec - (time.tv_sec * 1e6)) * 1e3;

        return nanosleep(&time, NULL);
}

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

	if (strcmp(tag, "ar") == 0)
		lrc_info->artist = strdup(value);

	else if (strcmp(tag, "al") == 0)
		lrc_info->album = strdup(value);

	else if (strcmp(tag, "ti") == 0)
		lrc_info->title = strdup(value);

	else if (strcmp(tag, "au") == 0)
		lrc_info->author = strdup(value);

	else if (strcmp(tag, "by") == 0)
		lrc_info->creator = strdup(value);

	else if (strcmp(tag, "length") == 0)
		lrc_info->length = str2time(value);

	else if (strcmp(tag, "offset") == 0)
		lrc_info->offset = strtol(value, NULL, 10);

	else
		return -1;	/* unknown tag */

	return 0;
}

void
print_lrc_info(struct lrc_info *lrc_info)
{
        if (lrc_info->artist  != NULL)
		printf("[ar:%s]\n", lrc_info->artist);

        if (lrc_info->album   != NULL)
		printf("[al:%s]\n", lrc_info->album);

        if (lrc_info->title   != NULL)
		printf("[ti:%s]\n", lrc_info->title);

        if (lrc_info->author  != NULL)
		printf("[au:%s]\n", lrc_info->author);

        if (lrc_info->creator != NULL)
		printf("[by:%s]\n", lrc_info->creator);

        if (lrc_info->length > 0) {
		printf("[length:%d:%d]\n",
			USEC2SEC(lrc_info->length) / 60,	/* minutes */
			USEC2SEC(lrc_info->length) % 60);	/* seconds */
	}

        if (lrc_info->offset != 0)
		printf("[offset:%d]\n", lrc_info->offset);
}
