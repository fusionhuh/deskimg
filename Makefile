CC         = gcc
PREFIX = /usr/local

LIBS     = `pkg-config --libs gtk+-3.0` -lstdc++
INCS     = 
CFLAGS = $(INCS) -std=c++17 `pkg-config --cflags gtk+-3.0` 

deskimg: deskimg.cpp
	${CC} ${CFLAGS} -o deskimg deskimg.cpp $(LIBS)

install: deskimg
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f deskimg ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/deskimg

uninstall:
	rm -rf ${DESTDIR}${PREFIX}/bin/deskimg ~/.deskimg/ ~/.config/deskimg/

clean:
	rm -f deskimg
