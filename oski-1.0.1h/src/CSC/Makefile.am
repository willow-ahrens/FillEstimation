#
# Build the CSC module.
#

include $(top_srcdir)/src/Makefile.common
include $(top_srcdir)/src/Makefile.mattypes
include $(top_srcdir)/src/Makefile.luaoski

mattype = CSC
mattype_sources = module.c matmult.c getset.c trisolve.c

pkglib_LTLIBRARIES =

include Make.stub

#
# OSKI-Lua transformation files.
#
dist_oskilua_DATA = CSC.lua

# eof
