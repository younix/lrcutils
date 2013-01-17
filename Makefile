CFLAGS=-std=c99 -pedantic -Wall

.PHONY: all clean

all: lrcplay lrcrec

lrcplay: lrcplay.c
	gcc ${CFLAGS} -o $@ lrcplay.c

lrcrec: lrcrec.c
	gcc ${CFLAGS} -o $@ lrcrec.c

clean:
	rm -f lrcplay
