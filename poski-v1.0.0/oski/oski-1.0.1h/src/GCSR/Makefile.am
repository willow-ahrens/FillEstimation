#
# Build the GCSR module.
#

include $(top_srcdir)/src/Makefile.common
include $(top_srcdir)/src/Makefile.mattypes
include $(top_srcdir)/src/Makefile.luaoski

mattype = GCSR
mattype_sources = module.c matmult.c getset.c #trisolve.c

pkglib_LTLIBRARIES =

include Make.stub

#
# OSKI-Lua transformation files.
#
dist_oskilua_DATA = GCSR.lua

# Recursively build kernel implementations
SUBDIRS = MatMult

include Make.GCSR

# eof
