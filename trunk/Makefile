PREFIX    = usr
SHAREDIR  = ${PREFIX}/share
MANDIR    = ${SHAREDIR}/man
BINDIR    = ${PREFIX}/bin
SVNREV 	  = 123
VER	  = 0.1.0
DIST	  = euclid-wm-${VER}


ifndef ${XDG_CONFIG_HOME}
XDG_CONFIG_HOME=${HOME}/.config
endif
CONFDIR = ${XDG_CONFIG_HOME}/euclid-wm

CC = cc -pedantic -Wall
CFLAGS = -O2 -g -std=c99
LDFLAGS = -lX11

.PHONY: all install install_conf clean uninstall dist dist_clean

all: euclid-wm

euclid-wm: euclid-wm.c
	${CC} ${LDFLAGS} ${CFLAGS} $< -o $@

install: all
	@install -m755 euclid-wm -D ${DESTDIR}/${BINDIR}/euclid-wm
	@install -m644 euclid.desktop -D ${DESTDIR}/${SHAREDIR}/xsessions/euclid.desktop
	@install -m644 euclid.1 -D ${DESTDIR}/${MANDIR}/man1/euclid-wm.1
	@install -m644 euclid-wm.conf.sample -D -b ${DESTDIR}/${SHAREDIR}/euclid-wm/euclid-wm.conf
	@install -m644 euclidrc -D -b ${DESTDIR}/${SHAREDIR}/euclid-wm/euclidrc
	@install -m644 VERSION -D ${DESTDIR}/${SHAREDIR}/euclid-wm/VERSION 2>/dev/null || echo "From SVN: `svn info | grep Revision: | cut -d ' ' -f2`/`date +%F`" >${DESTDIR}/${SHAREDIR}/euclid-wm/VERSION
	@sed s_/usr/share_${DESTDIR}/${SHAREDIR}_ <start-euclid >start-euclid-local
	@install -m755 start-euclid-local -D ${DESTDIR}/${BINDIR}/start-euclid

install_conf:
	@install -b -D -m600 euclid-wm.conf.sample ${CONFDIR}/euclid-wm.conf
	@install -b -D -m700 euclidrc ${CONFDIR}/euclidrc

uninstall:
	rm -f ${DESTDIR}/${BINDIR}/euclid-wm
	rm -f ${DESTDIR}/${SHAREDIR}/xsessions/euclid.desktop
	rm -f ${DESTDIR}/${MANDIR}/man1/euclid-wm.1

clean:
	rm -f euclid-wm

dist: 
	mkdir ${DIST}	
	svn co http://euclid-wm.googlecode.com/svn/trunk/ ./${DIST} -r ${SVNREV}
	echo "${VER} (svn${SVNREV}/`date +%F`)" > ./${DIST}/VERSION
	rm -rf ${DIST}/.svn
	tar -cvz ${DIST} -feuclid-wm.${VER}.tar.gz

dist_clean: 
	rm -rf ${DIST}

