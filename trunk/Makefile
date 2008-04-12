# Copyright (c) 2008 Mikolaj Golub
# All rights reserved.
#
# $Id: Makefile,v 1.2 2008/04/12 20:50:59 mikolaj Exp $

PROG=	devstat

LDADD=	-ldevstat

WARNS=	-Wsystem-headers -Werror -Wall -Wno-format-y2k -W \
	-Wstrict-prototypes -Wmissing-prototypes \
	-Wpointer-arith -Wreturn-type -Wcast-qual \
	-Wwrite-strings -Wswitch -Wshadow -Wcast-align \
	-Wbad-function-cast -Wchar-subscripts -Winline \
	-Wnested-externs -Wredundant-decls -std=c99

CFLAGS +=	${WARNS}

all:	${PROG}

${PROG}:	${PROG}.c
	gcc ${CFLAGS} ${LDADD} ${PROG}.c -o ${PROG}

clean:
	rm -f ${PROG}
