include config.mk

.PHONY: all clean install dist
.SUFFIXES: .c

all: lrcplay lrcrec

lrcplay: lrcplay.c lrc.c lrc.h
lrcrec: lrcrec.c lrc.c lrc.h

.c:
	gcc ${CFLAGS} -o $@ $< lrc.c

install: all
	mkdir -p ${DESTDIR}${BINDIR}
	mkdir -p ${DESTDIR}${MAN1DIR}
	install -m 775 lrcplay ${DESTDIR}${BINDIR}
	install -m 775 lrcrec ${DESTDIR}${BINDIR}
	install -m 444 lrcplay.1 ${DESTDIR}${MAN1DIR}
	install -m 444 lrcrec.1 ${DESTDIR}${MAN1DIR}

dist: clean
	mkdir -p lrcutils-${VERSION}
	cp lrcplay.c lrcrec.c lrcplay.1 lrcrec.1 README.md config.mk Makefile \
	    lrcutils-${VERSION}
	tar czf lrcutils-${VERSION}.tar.gz lrcutils-${VERSION}

clean:
	rm -rf lrcplay lrcrec lrcutils-${VERSION}.*
