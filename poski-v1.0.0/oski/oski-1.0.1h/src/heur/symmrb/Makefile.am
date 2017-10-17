#
# Build the heuristic modules
#

include $(top_srcdir)/src/Makefile.common
include $(top_srcdir)/src/Makefile.mattypes

pkglib_LTLIBRARIES =

# --------------------------------------------------------------------
# Symmetric register blocked SpMV heuristic

symmrb_sources = symmrb.c symmrb_eval.c
symmrb_cflags = -DMOD_NAME=liboski_heur_symmrb

include Make.stub

# eof
