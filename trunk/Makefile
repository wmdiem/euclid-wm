PREFIX    = usr
SHAREDIR  = ${PREFIX}/share
MANDIR    = ${SHAREDIR}/man
BINDIR    = ${PREFIX}/bin
SVNREV 	  = 224 
VER	  = 0.4.3
DIST	  = euclid-wm-${VER}


ifndef ${XDG_CONFIG_HOME}
XDG_CONFIG_HOME=${HOME}/.config
endif
CONFDIR = ${XDG_CONFIG_HOME}/euclid-wm

CC = cc -pedantic -Wall 
CFLAGS = -O3 -g -std=c99 
LDFLAGS = -lX11 -lXinerama

.PHONY: all install install_conf clean uninstall dist dist_clean

all: euclid-wm

euclid-wm: euclid-wm.c
	${CC} $< ${LDFLAGS} ${CFLAGS} -o $@

noxinerama: euclid-wm.c
	${CC} $< -lX11 ${CFLAGS} -D NOXINERAMA -o $@
	cp noxinerama euclid-wm

install: all
	@install -m755 euclid-wm -D ${DESTDIR}/${BINDIR}/euclid-wm
	@install -m644 euclid.desktop -D ${DESTDIR}/${SHAREDIR}/xsessions/euclid.desktop
	@install -m644 euclid.1 -D ${DESTDIR}/${MANDIR}/man1/euclid-wm.1
	@install -m644 euclid-wm.conf.sample -D -b ${DESTDIR}/${SHAREDIR}/euclid-wm/euclid-wm.conf
	@install -m644 euclidrc -D -b ${DESTDIR}/${SHAREDIR}/euclid-wm/euclidrc
	@install -m644 VERSION -D ${DESTDIR}/${SHAREDIR}/euclid-wm/VERSION 2>/dev/null || echo "From SVN: `svn info | grep Revision: | cut -d ' ' -f2`/`date +%F`" >${DESTDIR}/${SHAREDIR}/euclid-wm/VERSION
	@sed s_/usr/share_/${SHAREDIR}_ <start-euclid >start-euclid-local
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
	rm -f start-euclid-local
	rm -f noxinerama

dist: 
	mkdir ${DIST}	
	svn co http://euclid-wm.googlecode.com/svn/trunk ./${DIST} -r ${SVNREV}
	echo "${VER} (svn${SVNREV}/`date +%F`)" > ./${DIST}/VERSION
	rm -rf ${DIST}/.svn
	tar -cvz ${DIST} -f${DIST}.tar.gz

dist_clean: 
	rm -rf ${DIST}

