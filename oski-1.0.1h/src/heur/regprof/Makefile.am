#
# Build the heuristic modules
#

include $(top_srcdir)/src/Makefile.common
include $(top_srcdir)/src/Makefile.mattypes

pkglib_LTLIBRARIES =

# --------------------------------------------------------------------
# MBCSR heuristic for general workloads

regprof_sources = regprofheur.c regprofquery.c regprofmgr.c
regprof_cflags = -DMOD_NAME=liboski_heur_regprof

include Make.stub

# eof

