# $FreeBSD: release/9.3.0/share/examples/kld/syscall/Makefile 99771 2002-07-11 13:38:05Z ru $

SUBDIR=	module test

load unload: _SUBDIR

.include <bsd.subdir.mk>
