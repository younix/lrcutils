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

#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define CSEC2USEC(csec)	((csec) * 10000)
#define SEC2USEC(sec)	((sec) * 1000000)
#define MIN2USEC(min)	(SEC2USEC((min) * 60))

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
	if (argc < 2)
		usage();
	
	FILE *fh = fopen(argv[1], "r");

	if (fh == NULL)
		return EXIT_FAILURE;

	char line[BUFSIZ];
	char *lyric, *ttag;		/* timing tag	*/
	useconds_t wtime = 0;		/* waiting time	*/
	useconds_t time_sum = 0;

	while (fgets(line, BUFSIZ, fh)) {
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

		puts(lyric);
		fflush(stdout);
	}

	fclose(fh);

	return EXIT_SUCCESS;
}
