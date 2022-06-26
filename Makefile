CC     = gcc
PREFIX = /usr/local

LIBS   = `pkg-config --libs gtk+-3.0`
INCS   = `pkg-config --cflags gtk+-3.0`
CFLAGS = $(INCS) -Wno-incompatible-pointer-types

deskimg: deskimg.c
	${CC} ${CFLAGS} -o deskimg deskimg.c $(LIBS)

install: deskimg
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f deskimg ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/deskimg

uninstall:
	rm -rf ${DESTDIR}${PREFIX}/bin/deskimg ~/.deskimg/ ~/.config/deskimg/

clean:
	rm -f deskimg 
