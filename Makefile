CFLAGS=-std=c99 -pedantic -Wall

lrcplay: lrcplay.c
	gcc ${CFLAGS} -o $@ lrcplay.c

clean:
	rm -f lrcplay
