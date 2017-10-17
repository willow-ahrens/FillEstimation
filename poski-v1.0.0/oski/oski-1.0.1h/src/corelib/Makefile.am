# OSKI core library routines

include $(top_srcdir)/src/Makefile.common

pkglib_LTLIBRARIES =
noinst_LTLIBRARIES =

if BUILD_darwin
# HACK on Darwin: Stub needed if we don't use libtool to link.
pkglib_LTLIBRARIES += liboski_ltdlstub.la
liboski_ltdlstub_la_SOURCES = ltdlstub.c
liboski_ltdlstub_la_CFLAGS = -DMOD_NAME=liboski_ltdlstub
liboski_ltdlstub_la_LDFLAGS = $(release_ldflags)
endif

# Type independent core library
noinst_LTLIBRARIES += liboski_core.la
liboski_core_la_SOURCES = \
	debug.c \
	error.c \
	kerinfo.c \
	malloc.c \
	mattypes.c \
	modloader.c \
	scalinfo.c \
	simplelist.c \
	sprintf.c \
	trace.c
liboski_core_la_CFLAGS = -DMOD_NAME=liboski \
			-DOSKI_LIB_PATH_DEFAULT="\"$(pkglibdir)\""

# Add on type dependent core library
typedep_sources = \
	blas.c \
	gemv.c \
	lua.c \
	matcommon.c \
	tconst.c \
	trsv.c

include Make.stub

# eof
