if BUILDABLE_SJ3DICT

EXTRA_DIST = visual.dic

all: sj3main.dic

sj3main.dic: visual.dic
	$(top_srcdir)/src/sj3mkdic/sj3mkdic ./visual.dic sj3main.dic > Moutput

install::
	($(mkdir_p) $(SJ3DICTDIR); \
	 $(INSTALL_DATA) sj3main.dic $(SJ3DICTDIR); \
	 $(mkdir_p) $(SJ3DICTDIR)/user; \
	 chown $(SJ3OWNER) $(SJ3DICTDIR)/user; \
	 chgrp $(SJ3GROUP) $(SJ3DICTDIR)/user;)

CLEANFILES = sj3main.dic Moutput

endif
