#
# Builds a library stub, liboskilt, for use when linking without using
# libtool.
#

include $(top_srcdir)/src/Makefile.common
include $(top_srcdir)/src/Make.extlibs

oski_ldadd_all = $(oski_ldadd) \
  $(oski_ldadd_Tis) $(oski_ldadd_Tid) $(oski_ldadd_Tic) $(oski_ldadd_Tiz) $(oski_ldadd_TiL) \
  $(oski_ldadd_Tls) $(oski_ldadd_Tld) $(oski_ldadd_Tlc) $(oski_ldadd_Tlz) $(oski_ldadd_TlL)

pkglib_LTLIBRARIES = liboskilt.la
liboskilt_la_SOURCES = modsyms.c
liboskilt_la_CFLAGS = -DMOD_NAME=liboskilt
if BUILD_darwin
liboskilt_la_LIBADD = $(oski_ldadd_all)
endif
liboskilt_la_LDFLAGS = $(release_ldflags)

modsyms.c: $(CSYMX_BIN)
	echo "/* Generated on `date` by `whoami` [`hostname`] */" > $@
	for f in $(oski_ldadd_all) ; do \
	    $(CSYMX_BIN) --decl \
	        `echo $$f | sed 's,liboski\(.*\)\.la,.libs/liboski\1.a,g'` \
	        >> $@ ; \
	done
	echo "struct { const char* name; void* addr; }" >> $@
	echo "lt_preloaded_symbols[] = {" >> $@
	for f in $(oski_ldadd_all) ; do \
	    $(CSYMX_BIN) --lt-pairs \
	        `echo $$f | sed 's,liboski\(.*\)\.la,.libs/liboski\1.a,g'` \
	        >> $@ ; \
	done
	echo "0x0, 0x0};" >> $@
	echo "/* eof */" >> $@

distclean-local:
	-rm -rf modsyms.c

# eof
