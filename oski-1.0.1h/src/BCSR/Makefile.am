#
# Build the BCSR module.
#

include $(top_srcdir)/src/Makefile.common
include $(top_srcdir)/src/Makefile.mattypes
include $(top_srcdir)/src/Makefile.luaoski

mattype = BCSR
mattype_sources = module.c convert.c getset.c \
	matmult.c ata.c

pkglib_LTLIBRARIES =

include Make.stub

#
# OSKI-Lua transformation files
#
dist_oskilua_DATA = BCSR.lua

SUBDIRS = MatMult MatTransMatMult

include Make.BCSR

# eof
