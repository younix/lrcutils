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
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "lrc.h"

#define USEC2CSEC(usec)	((usec) / 10000)

void
print_lrc_info(struct lrc_info *lrc_info)
{
	if (lrc_info->artist  != NULL) printf("[ar:%s]\n", lrc_info->artist);
	if (lrc_info->album   != NULL) printf("[al:%s]\n", lrc_info->album);
	if (lrc_info->title   != NULL) printf("[ti:%s]\n", lrc_info->title);
	if (lrc_info->author  != NULL) printf("[au:%s]\n", lrc_info->author);
	if (lrc_info->creator != NULL) printf("[by:%s]\n", lrc_info->creator);
}

void
usage(void)
{
	fprintf(stderr, "lrcrec [OPTIONS] FILE\n"
			"\n"
			"OPTIONS\n"
			"\t-r artist\n"
			"\t-l album\n"
			"\t-t title\n"
			"\t-u author\n"
			"\t-b creator\n");
	exit(EXIT_FAILURE);
}

int
main(int argc, char **argv)
{
	char line[BUFSIZ];
	char *lyric;
	int min, sec, csec;
	struct timeval start_time;
	struct timeval cur_time;
	struct lrc_info lrc_info = {0};
	int ch;

	while ((ch = getopt(argc, argv, "r:l:t:u:b:")) != -1) {
		switch (ch) {
		case 'r':
			lrc_info.artist = strdup(optarg);
			break;
		case 'l':
			lrc_info.album = strdup(optarg);
			break;
		case 't':
			lrc_info.title = strdup(optarg);
			break;
		case 'u':
			lrc_info.author = strdup(optarg);
			break;
		case 'b':
			lrc_info.creator = strdup(optarg);
			break;
		default:
			usage();
			/* NOTREACHED */
		}
	}
	argc -= optind;
	argv += optind;

	if (argc < 1)
		usage();

	FILE *fh = fopen(argv[0], "r");

	if (fh == NULL)
		return EXIT_FAILURE;

	print_lrc_info(&lrc_info);

	gettimeofday(&start_time, NULL);

	while (fgets(line, BUFSIZ, fh)) {
		if (line[0] == '[') {	/* check for timing tags */
			/* search for begin of lyric */
			if ((lyric = strchr(line, ']')) == NULL)
				continue;
			lyric++;
		} else {
			lyric = line;
		}

		getchar();		/* just wait for return */

		/* calculate time difference between start and now */
		gettimeofday(&cur_time, NULL);
		cur_time.tv_sec -= start_time.tv_sec;
		cur_time.tv_usec -= start_time.tv_usec;

		/* calculate carry if needed */
		if (cur_time.tv_usec < 0) {
			cur_time.tv_sec--;
			cur_time.tv_usec += 1000000;
		}

		min = cur_time.tv_sec / 60;
		sec = cur_time.tv_sec % 60;
		csec = USEC2CSEC(cur_time.tv_usec);

		printf("[%d:%02d.%02d]%s", min, sec, csec, lyric);
		fflush(stdout);
	}

	fclose(fh);

	return EXIT_SUCCESS;
}
