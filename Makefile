CFLAGS=-std=c99 -pedantic -Wall

.PHONY: all clean

all: lrcplay lrcrec bar

.SUFFIXES: .c
.c:
	gcc ${CFLAGS} -o $@ $<

bar: bar.c
	gcc ${CFLAGS} `pkg-config --libs --cflags gtk+-2.0 gio-2.0` -o $@ $<

clean:
	rm -f lrcplay lrcrec bar
