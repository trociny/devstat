# Copyright (c) 2008 Mikolaj Golub
# All rights reserved.
#
# $Id: Makefile,v 1.3 2008/06/03 20:35:31 mikolaj Exp $

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
	${CC} ${CFLAGS} ${LDADD} ${PROG}.c -o ${PROG}

clean:
	rm -f ${PROG}
