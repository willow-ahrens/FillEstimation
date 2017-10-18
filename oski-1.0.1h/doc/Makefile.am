#
# Doxygen-generated reference documentation.
#

.PHONY: dox

doxdir = $(top_srcdir)/doc

EXTRA_DIST = oski_dox.cfg.in libdep.dot

dox: oski_dox.cfg
	doxygen oski_dox.cfg

oski_dox.cfg: $(doxdir)/oski_dox.cfg.in
	cat $(doxdir)/oski_dox.cfg.in \
		| sed "s,\@version\@,$(VERSION)," \
		| sed "s,\@top_srcdir\@,$(top_srcdir)," \
		| sed "s,\@top_builddir\@,$(top_builddir)," \
	> $@

libdep.ps: $(doxdir)/libdep.dot
	dot -Tps $(doxdir)/libdep.dot > libdep.ps

distclean-local:
	-rm -rf html latex oski_dox.cfg libdep.ps

# eof
