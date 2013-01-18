CFLAGS=-std=c99 -pedantic -Wall

.PHONY: all clean

all: lrcplay lrcrec bar

lrcplay: lrcplay.c lrc.h
lrcrec: lrcrec.c lrc.h

.SUFFIXES: .c
.c:
	gcc ${CFLAGS} -o $@ $<

bar: bar.c
	gcc ${CFLAGS} `pkg-config --libs --cflags gtk+-2.0 gdk-x11-2.0 gio-unix-2.0 gobject-2.0` -o $@ $<

clean:
	rm -f lrcplay lrcrec bar
