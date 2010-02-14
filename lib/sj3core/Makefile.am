if BUILDABLE_SJ3SERV

INCLUDES = \
	-I$(top_srcdir)/include/sj3compat \
	-I$(top_srcdir)/include/sj3common \
	-I$(top_srcdir)/include/sj3core

lib_LTLIBRARIES = libsj3core.la

libsj3core_la_LDFLAGS = $(top_builddir)/lib/sj3compat/libsj3compat.la -version-info @LIBSJ3CORE_VERSION_INFO@

libsj3core_la_SOURCES = \
	adddic.c \
	addelcmn.c \
	alloc.c \
	charsize.c \
	chrtbl.c \
	cl2knj.c \
	clstudy.c \
	cmpstr.c \
	conjunc.c \
	connect.c \
	cvtclknj.c \
	cvtdict.c \
	cvtkanji.c \
	deldic.c \
	dict.c \
	functbl.c \
	fuzoku.c \
	fzkyomi.c \
	getkanji.c \
	getrank.c \
	global.c \
	hzstrlen.c \
	init.c \
	istrcmp.c \
	memory.c \
	mk2claus.c \
	mkbunset.c \
	mkjiritu.c \
	mkkouho.c \
	mknumber.c \
	mvmemd.c \
	mvmemi.c \
	peepdic.c \
	ph2knj.c \
	ph_khtbl.c \
	priority.c \
	prtytbl.c \
	s2ctbl.c \
	sdepend.c \
	selclrec.c \
	selsuuji.c \
	setconj.c \
	setjrec.c \
	setkouho.c \
	setubi.c \
	sj2code.c \
	skiphblk.c \
	skipkstr.c \
	srchdict.c \
	srchhead.c \
	srchidx.c \
	srchnum.c \
	sstrcmp.c \
	sstrlen.c \
	sstrncmp.c \
	stbtbl.c \
	stttbl.c \
	study.c \
	suujitbl.c \
	terminat.c \
	termtbl.c \
	wakachi.c

endif
