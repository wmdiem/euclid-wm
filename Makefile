PREFIX    = usr
SHAREDIR  = ${PREFIX}/share
MANDIR    = ${SHAREDIR}/man
BINDIR    = ${PREFIX}/bin

CC = cc -pedantic -Wall
CFLAGS = -O2 -g -std=c99
LDFLAGS = -lX11

.PHONY: all install install_conf clean uninstall

all: euclid-wm

euclid-wm: euclid-wm.c
	${CC} ${LDFLAGS} ${CFLAGS} $< -o $@

install: all
	@install -m755 euclid-wm -D ${DESTDIR}/${BINDIR}/euclid-wm
	@install -m644 euclid.desktop -D ${DESTDIR}/${SHAREDIR}/xsessions/euclid.desktop
	@install -m644 euclid.1 -D ${DESTDIR}/${MANDIR}/man1/euclid-wm.1

install_conf:
	@if [ ${XDG_CONFIG_HOME} ]; then install -b -m600 euclid-wm.conf.sample ${XDG_CONFIG_HOME}/euclid-wm.conf; else install -b -m600 euclid-wm.conf.sample ${HOME}/.config/euclid-wm.conf; fi

uninstall:
	rm -f ${DESTDIR}/${BINDIR}/euclid-wm
	rm -f ${DESTDIR}/${SHAREDIR}/xsessions/euclid.desktop
	rm -f ${DESTDIR}/${MANDIR}/man1/euclid-wm.1

clean:
	rm -f euclid-wm
