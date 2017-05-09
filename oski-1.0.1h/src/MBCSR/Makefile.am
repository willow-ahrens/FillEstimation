#
# Build the MBCSR module.
#

include $(top_srcdir)/src/Makefile.common
include $(top_srcdir)/src/Makefile.mattypes
include $(top_srcdir)/src/Makefile.luaoski

mattype = MBCSR
mattype_sources = convert.c module.c getset.c \
	matmult.c trisolve.c ata.c a_and_at.c

pkglib_LTLIBRARIES =

include Make.stub

#
# OSKI-Lua transformation files
#
dist_oskilua_DATA = MBCSR.lua

# Recursively build actual kernel implementations
SUBDIRS = MatMult MatTrisolve

if BUILD_MBCSR_SymmMatMult
SUBDIRS += SymmMatMult
endif

if BUILD_MBCSR_MatTransMatMult
SUBDIRS += MatTransMatMult
endif

if BUILD_MBCSR_MatMultAndMatTransMult
SUBDIRS += MatMultAndMatTransMult
endif

include Make.MBCSR

# eof
