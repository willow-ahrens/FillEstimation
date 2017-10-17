#
# Build the VBR module.
#

include $(top_srcdir)/src/Makefile.common
include $(top_srcdir)/src/Makefile.mattypes
include $(top_srcdir)/src/Makefile.luaoski

mattype = VBR
mattype_sources = convert.c module.c matmult.c getset.c #trisolve.c

pkglib_LTLIBRARIES =

include Make.stub

#
# OSKI-Lua transformation files.
#
dist_oskilua_DATA = VBR.lua

# eof
