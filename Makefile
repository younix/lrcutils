CFLAGS=-std=c99 -pedantic -Wall

.PHONY: all clean

all: lrcplay lrcrec

.SUFFIXES: .c
.c:
	gcc ${CFLAGS} -o $@ $<

clean:
	rm -f lrcplay lrcrec
