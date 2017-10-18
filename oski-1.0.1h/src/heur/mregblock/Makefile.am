#
# Build the heuristic modules
#

include $(top_srcdir)/src/Makefile.common
include $(top_srcdir)/src/Makefile.mattypes

# --------------------------------------------------------------------
# Start of heuristic modules

pkglib_LTLIBRARIES =

# --------------------------------------------------------------------
# MBCSR SpMV heuristic

mregblock_sources = mregblock.c mregblock_eval.c
mregblock_cflags = -DMOD_NAME=liboski_heur_mregblock

include Make.stub

# eof

