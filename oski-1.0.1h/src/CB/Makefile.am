#
# Build the CB module.
#

include $(top_srcdir)/src/Makefile.common
include $(top_srcdir)/src/Makefile.mattypes
include $(top_srcdir)/src/Makefile.luaoski

mattype = CB
mattype_sources = module.c convert.c matmult.c getset.c #trisolve.c

pkglib_LTLIBRARIES =

include Make.stub

#
# OSKI-Lua transformation files.
#
dist_oskilua_DATA = CB.lua

# Recursively build kernel implementations
#SUBDIRS = MatMult
#include Make.CB

# eof
