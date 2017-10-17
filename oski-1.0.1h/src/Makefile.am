#
# Build OSKI Library.
#
include $(top_srcdir)/src/Makefile.common
include $(top_srcdir)/src/Makefile.mattypes
include $(top_srcdir)/src/Makefile.heurtypes
include $(top_srcdir)/src/Makefile.luaoski

#debug.c error.c kerinfo.c malloc.c scalinfo.c simplelist.c sprintf.c trace.c

misc_ldflags =
if BUILD_darwin
misc_ldflags += corelib/liboski_ltdlstub.la
endif

#
# Main OSKI Library (matrix-type independent)
#
pkglib_LTLIBRARIES = liboski.la
liboski_la_SOURCES = \
	heur.c \
	init.c \
	inmatprop.c \
	structhint.c
liboski_la_CFLAGS = \
	-DMOD_NAME=liboski \
	-DOSKI_LIB_PATH_DEFAULT="\"$(pkglibdir)\"" \
	-DOSKI_BENCH_DIR_DEFAULT="\"$(benchdatadir)\"" \
	-DOSKI_CC="\"${CC}\"" \
	-DOSKI_CFLAGS="\"${CFLAGS}\"" \
	-DOSKI_LDFLAGS="\"${LDFLAGS}\"" \
	-DOSKI_F77="\"${F77}\"" \
	-DOSKI_FFLAGS="\"${FFLAGS}\"" \
	-DOSKI_SUPPORT_LDFLAGS="\"${support_ldflags}\"" \
	-DOSKI_LUA_ROOT_DIR="\"${oskiluadir}\""
liboski_la_LIBADD = \
	corelib/liboski_core.la \
	timer/liboski_tempo.la \
	../lua/src/liblua.la ../lua/src/lib/liblualib.la \
	@LIBLTDL@
liboski_la_LDFLAGS = \
	$(misc_ldflags) \
	$(support_ldflags) $(release_ldflags)

noinst_SCRIPTS = echo_oski_ldpath.sh echo_oski_lua_path.sh

echo_oski_ldpath.sh: $(top_srcdir)/src/Makefile.mattypes
	curdir=`pwd` ; cd $(top_builddir) ; predir=`pwd` ; cd $${curdir} ; \
	oski_ldpath="$${predir}:$${predir}/src/corelib/.libs:$${predir}/src/.libs:$${predir}/src/heur/.libs:$${predir}/util/.libs" ; \
	for mattype in $(mattype_list) ; do \
		oski_ldpath="$${oski_ldpath}:$${predir}/src/$${mattype}/.libs" ; \
	done ; \
	for heurtype in $(heurtype_list) ; do \
		oski_ldpath="$${oski_ldpath}:$${predir}/src/heur/$${heurtype}/.libs" ; \
	done ; \
	echo "#!/bin/sh" | tee $@ ; \
	echo "echo \"$${oski_ldpath}\"" | tee -a $@ ; \
	echo "# eof" | tee -a $@
	chmod +x $@

echo_oski_lua_path.sh: $(top_srcdir)/src/Makefile.mattypes
	curdir=`pwd` ; cd $(top_builddir) ; bdir=`pwd` ; cd $${curdir} ; \
	curdir=`pwd` ; cd $(top_srcdir) ; sdir=`pwd` ; cd $${curdir} ; \
	oski_lua_path="$${bdir}/src/?.lua;$${sdir}/src/?.lua" ; \
	for mattype in $(mattype_list) ; do \
		oski_lua_path="$${oski_lua_path};$${bdir}/src/$${mattype}/?.lua" ; \
		oski_lua_path="$${oski_lua_path};$${sdir}/src/$${mattype}/?.lua" ; \
	done ; \
	echo "#!/bin/sh" | tee $@ ; \
	echo "echo \"$${oski_lua_path}\"" | tee -a $@ ; \
	echo "# eof" | tee -a $@
	chmod +x $@


#
# Matrix type-specific modules.
#
#	blas.c gemv.c tconst.c
#
mattypedep_sources = \
	a_and_at.c \
	ata.c \
	getset.c \
	heur_typedep.c \
	hint.c \
	matcreate.c \
	matmult.c \
	matpow.c \
	matrix.c \
	perm.c \
	trace_query.c \
	trisolve.c \
	tune.c \
	vecview.c \
	xforms.c

include Make.stub

# ----------------------------------------------------------------------

# OSKI-Lua transformation interpreter files
dist_oskilua_DATA = oski.lua

distclean-local: echo_oski_ldpath.sh echo_oski_lua_path.sh
	-rm -rf echo_oski_ldpath.sh echo_oski_lua_path.sh modsyms.c

# eof
