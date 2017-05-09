#
# Build the DENSE module.
#

include $(top_srcdir)/src/Makefile.common
include $(top_srcdir)/src/Makefile.mattypes
include $(top_srcdir)/src/Makefile.luaoski

mattype = DENSE
mattype_sources = module.c matmult.c getset.c trisolve.c

pkglib_LTLIBRARIES =

include Make.stub

#
# OSKI-Lua transformation files.
#
dist_oskilua_DATA = DENSE.lua

# eof
