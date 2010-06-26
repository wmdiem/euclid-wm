PREFIX    = usr
SHAREDIR  = ${PREFIX}/share
MANDIR    = ${SHAREDIR}/man
BINDIR    = ${PREFIX}/bin

CC = cc -pedantic -Wall
CFLAGS = -O2 -g -std=c99
LDFLAGS = -lX11

.PHONY: all install clean uninstall

all: euclid-wm

euclid-wm: euclid-wm.c
	${CC} ${LDFLAGS} ${CFLAGS} $< -o $@

install: all
	@install -m755 euclid-wm -D ${DESTDIR}/${BINDIR}/euclid-wm
	@install -m644 euclid.desktop -D ${DESTDIR}/${SHAREDIR}/xsessions/euclid.desktop
	@install -m644 euclid.1 -D ${DESTDIR}/${MANDIR}/man1/euclid-wm.1

uninstall:
	rm -f ${DESTDIR}/${BINDIR}/euclid-wm
	rm -f ${DESTDIR}/${SHAREDIR}/xsessions/euclid.desktop
	rm -f ${DESTDIR}/${MANDIR}/man1/euclid-wm.1

clean:
	rm -f euclid-wm

