#
# Build the CSR module.
#

include $(top_srcdir)/src/Makefile.common
include $(top_srcdir)/src/Makefile.mattypes
include $(top_srcdir)/src/Makefile.luaoski

mattype = CSR
mattype_sources = \
  	module.c check.c expand.c transpose.c getset.c \
	matmult.c trisolve.c

pkglib_LTLIBRARIES =

include Make.stub

#
# OSKI-Lua transformation files.
#
dist_oskilua_DATA = CSR.lua

SUBDIRS = MatMult SymmMatMult MatTrisolve

# eof
