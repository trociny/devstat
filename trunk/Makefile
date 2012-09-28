# $Id$

PROG=	devstat
MAN=	devstat.8

SYSTEM!=	uname -s

.if ${SYSTEM} == "FreeBSD"
SRCS=	devstat_freebsd.c
LDADD+=	-ldevstat -lkvm
DPADD+=	${LIBDEVSTAT} ${LIBKVM}
.elif ${SYSTEM} == "NetBSD"
SRCS=	devstat_netbsd.c
LDADD+=	-lkvm
DPADD+=	${LIBKVM}
.endif

.include <bsd.prog.mk>
