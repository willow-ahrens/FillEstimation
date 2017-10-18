#
# Build the heuristic modules
#

include $(top_srcdir)/src/Makefile.common
include $(top_srcdir)/src/Makefile.mattypes

# --------------------------------------------------------------------
# Generic utilities for all heuristics

heur_cflags =
heur_sources = estfill.c

noinst_LTLIBRARIES =

if BUILD_Tis
noinst_LTLIBRARIES += liboski_heur_Tis.la
liboski_heur_Tis_la_SOURCES = $(heur_sources)
liboski_heur_Tis_la_CFLAGS = $(heur_cflags) $(Tis_cflags)
endif

if BUILD_Tid
noinst_LTLIBRARIES += liboski_heur_Tid.la
liboski_heur_Tid_la_SOURCES = $(heur_sources)
liboski_heur_Tid_la_CFLAGS = $(heur_cflags) $(Tid_cflags)
endif

if BUILD_Tic
noinst_LTLIBRARIES += liboski_heur_Tic.la
liboski_heur_Tic_la_SOURCES = $(heur_sources)
liboski_heur_Tic_la_CFLAGS = $(heur_cflags) $(Tic_cflags)
endif

if BUILD_Tiz
noinst_LTLIBRARIES += liboski_heur_Tiz.la
liboski_heur_Tiz_la_SOURCES = $(heur_sources)
liboski_heur_Tiz_la_CFLAGS = $(heur_cflags) $(Tiz_cflags)
endif

if BUILD_Tls
noinst_LTLIBRARIES += liboski_heur_Tls.la
liboski_heur_Tls_la_SOURCES = $(heur_sources)
liboski_heur_Tls_la_CFLAGS = $(heur_cflags) $(Tls_cflags)
endif

if BUILD_Tld
noinst_LTLIBRARIES += liboski_heur_Tld.la
liboski_heur_Tld_la_SOURCES = $(heur_sources)
liboski_heur_Tld_la_CFLAGS = $(heur_cflags) $(Tld_cflags)
endif

if BUILD_Tlc
noinst_LTLIBRARIES += liboski_heur_Tlc.la
liboski_heur_Tlc_la_SOURCES = $(heur_sources)
liboski_heur_Tlc_la_CFLAGS = $(heur_cflags) $(Tlc_cflags)
endif

if BUILD_Tlz
noinst_LTLIBRARIES += liboski_heur_Tlz.la
liboski_heur_Tlz_la_SOURCES = $(heur_sources)
liboski_heur_Tlz_la_CFLAGS = $(heur_cflags) $(Tlz_cflags)
endif

# eof

