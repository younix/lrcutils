CFLAGS=-std=c99 -pedantic -Wall -Wextra -D_XOPEN_SOURCE=700

.PHONY: all clean

all: lrcplay lrcrec

lrcplay: lrcplay.c lrc.c lrc.h
lrcrec: lrcrec.c lrc.c lrc.h

.SUFFIXES: .c
.c:
	gcc ${CFLAGS} -o $@ $< lrc.c

clean:
	rm -f lrcplay lrcrec
