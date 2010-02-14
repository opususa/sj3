AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I config

EXTRA_DIST = \
	CHANGES.eucJP \
	README \
	README.eucJP \
	sj3.spec.in

SUBDIRS = \
	include \
	doc \
	lib \
	src \
	dict

install-data-local:
	$(mkdir_p) $(DESTDIR)$(SJ3VARDIR)
