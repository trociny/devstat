# Copyright (c) 2008 Mikolaj Golub
# All rights reserved.
#
# $Id: Makefile,v 1.4 2009/07/05 17:36:24 mikolaj Exp $

PROG = devstat

SYSTEM != uname -s

.if ${SYSTEM} == "FreeBSD"
SYS = freebsd
LDADD = -ldevstat
.elif ${SYSTEM} == "NetBSD"
SYS = netbsd
LDADD = -lkvm
.endif

WARNS = -Wsystem-headers -Werror -Wall -Wno-format-y2k -W \
	-Wstrict-prototypes -Wmissing-prototypes \
	-Wpointer-arith -Wreturn-type -Wcast-qual \
	-Wwrite-strings -Wswitch -Wshadow -Wcast-align \
	-Wbad-function-cast -Wchar-subscripts -Winline \
	-Wnested-externs -Wredundant-decls -std=c99

CFLAGS +=	${WARNS}

all:	${PROG}

${PROG}:	${PROG}_${SYS}.c
	${CC} ${CFLAGS} ${LDADD} ${PROG}_${SYS}.c -o ${PROG}

clean:
	rm -f ${PROG} *.core
