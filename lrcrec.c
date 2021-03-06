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
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <ctype.h>
#include <termios.h>

#include "lrc.h"

#define USEC2CSEC(usec)	((usec) / 10000)

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
			"\t-b creator\tof the lrc file\n"
			"\t-L length\tof the song\n"
			"\t-O offset\tin milliseconds\n");
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
	struct lrc_info lrc_info = LRC_INFO_NULL;
	int ch, key;
	struct termios tty;

	while ((ch = getopt(argc, argv, "r:l:t:u:b:L:O:")) != -1) {
		switch (ch) {
		case 'r':
			set_lrc_info(&lrc_info, "ar", optarg);
			break;
		case 'l':
			set_lrc_info(&lrc_info, "al", optarg);
			break;
		case 't':
			set_lrc_info(&lrc_info, "ti", optarg);
			break;
		case 'u':
			set_lrc_info(&lrc_info, "au", optarg);
			break;
		case 'b':
			set_lrc_info(&lrc_info, "by", optarg);
			break;
		case 'L':
			set_lrc_info(&lrc_info, "length", optarg);
			break;
		case 'O':
			set_lrc_info(&lrc_info, "offset", optarg);
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

	tcgetattr(fileno(stdin), &tty);
	tty.c_lflag &= ~(ICANON);	/* get input without return */
	tty.c_lflag &= ~(ECHO);		/* turn off input echo on tty */
	tcsetattr(fileno(stdin), TCSANOW, &tty);

	while (fgets(line, BUFSIZ, fh)) {
		if (line[0] == '[') {	/* check for tags */
			if (isalpha(line[1])) {		/* ID tag */
				printf("%s", line);
				continue;
			}

			/* search for begin of lyric */
			if ((lyric = strchr(line, ']')) == NULL)
				continue;
			lyric++;
		} else {
			lyric = line;
		}
input:
		key = getchar();	/* just wait for any key */

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

		printf("[%d:%02d.%02d]", min, sec, csec);

		if (key == 'e') {	/* insert empty line */
			printf("\n");
			fflush(stdout);

			goto input;
		} else {
			printf("%s", lyric);
			fflush(stdout);
		}
	}

	fclose(fh);

	return EXIT_SUCCESS;
}
