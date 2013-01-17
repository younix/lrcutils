CFLAGS=-std=c99 -pedantic -Wall

.PHONY: all clean

all: lrcplay lrcrec

lrcplay: lrcplay.c lrc.h
lrcrec: lrcrec.c lrc.h

.SUFFIXES: .c
.c:
	gcc ${CFLAGS} -o $@ $<

clean:
	rm -f lrcplay lrcrec
