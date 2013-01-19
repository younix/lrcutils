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

#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include "lrc.h"

useconds_t
waiting(useconds_t wtime, useconds_t time_sum)
{
	if (wtime < time_sum)	/* check for timing tags in the past */
		return time_sum;

	wtime -= time_sum;
	usleep(wtime);		/* wait ... */
	time_sum += wtime;	/* how longe we've been waiting till now? */

	return time_sum;
}

void
usage(void)
{
	fprintf(stderr, "lrcplay FILE\n");
	exit(EXIT_FAILURE);
}

int
main(int argc, char **argv)
{
	char line[BUFSIZ];
	char *lyric;
	char *ttag;			/* timing tag	*/
	char *tag, *tag_value;
	struct lrc_info lrc_info = {0};
	useconds_t wtime = 0;		/* waiting time	*/
	useconds_t time_sum = 0;

	if (argc < 2)
		usage();
	
	FILE *fh = fopen(argv[1], "r");

	if (fh == NULL)
		return EXIT_FAILURE;

	while (fgets(line, BUFSIZ, fh)) {
		/* check for none conforming lines */
		if (line[0] != '[' || strnlen(line, BUFSIZ) < 5)
			continue;

		if (isdigit(line[1]))	/* check for first lyric line */
			break;

		tag = line + 1;
		char *tag_separator = strchr(line, ':');
		if (tag_separator != NULL) {
			*tag_separator = '\0';
			tag_value = tag_separator + 1;

			char *tag_end = strchr(tag_value, ']');
			*tag_end = '\0';

			set_lrc_info(&lrc_info, tag, tag_value);
			printf("%s: %s\n", tag, tag_value);
		}
	}

	do {
		if (line[0] != '[')	/* check for empty lines */
			continue;

		/* search for begin of lyric */
		if ((lyric = strchr(line, ']')) == NULL)
			continue;
		lyric++;

		wtime = str2time(line);
		time_sum = waiting(wtime, time_sum);

		while ((ttag = strchr(lyric, '<')) != NULL) {
			fwrite(lyric, sizeof *lyric, ttag - lyric, stdout);
			fflush(stdout);

			wtime = str2time(ttag);
			time_sum = waiting(wtime, time_sum);

			/* search for begin of lyric */
			if ((lyric = strchr(ttag, '>')) == NULL)
				continue;
			lyric++;
		}

		printf("%s", lyric);
		fflush(stdout);
	} while (fgets(line, BUFSIZ, fh));

	fclose(fh);

	return EXIT_SUCCESS;
}
