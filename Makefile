CC     = gcc
PREFIX = /usr/local

LIBS   = `pkg-config --libs gtk+-3.0`
INCS   = `pkg-config --cflags gtk+-3.0`
CFLAGS = $(INCS) $(LIBS) -Wno-incompatible-pointer-types

deskimg: deskimg.c
	${CC} ${CFLAGS} -o deskimg deskimg.c

install: deskimg
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f deskimg ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/deskimg

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/deskimg ~/.deskimg/

clean:
	rm -f deskimg 
