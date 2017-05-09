#
# Build the CSR sparse symmetric matrix-vector multiply implementations.
#

include $(top_srcdir)/src/Makefile.common
include $(top_srcdir)/src/Makefile.mattypes

EXTRA_DIST = CSR_HEADER.c

# Set source filenames
mattype_sources =
include Make.CSR_SymmMatMult
mattype_real_sources = $(mattype_sources)
mattype_complex_sources = $(mattype_sources)
include Make.CSR_SymmMatHermMult
include Make.CSR_HermMatMult
include Make.CSR_HermMatTransMult

noinst_LTLIBRARIES =

if BUILD_Tis
noinst_LTLIBRARIES += libsymm_Tis.la
libsymm_Tis_la_SOURCES = $(mattype_real_sources)
libsymm_Tis_la_CFLAGS = -DMOD_NAME=liboski_mat_CSR $(Tis_cflags)
endif

if BUILD_Tid
noinst_LTLIBRARIES += libsymm_Tid.la
libsymm_Tid_la_SOURCES = $(mattype_real_sources)
libsymm_Tid_la_CFLAGS = -DMOD_NAME=liboski_mat_CSR $(Tid_cflags)
endif

if BUILD_Tic
noinst_LTLIBRARIES += libsymm_Tic.la
libsymm_Tic_la_SOURCES = $(mattype_complex_sources)
libsymm_Tic_la_CFLAGS = -DMOD_NAME=liboski_mat_CSR $(Tic_cflags)
endif

if BUILD_Tiz
noinst_LTLIBRARIES += libsymm_Tiz.la
libsymm_Tiz_la_SOURCES = $(mattype_complex_sources)
libsymm_Tiz_la_CFLAGS = -DMOD_NAME=liboski_mat_CSR $(Tiz_cflags)
endif

if BUILD_Tls
noinst_LTLIBRARIES += libsymm_Tls.la
libsymm_Tls_la_SOURCES = $(mattype_real_sources)
libsymm_Tls_la_CFLAGS = -DMOD_NAME=liboski_mat_CSR $(Tls_cflags)
endif

if BUILD_Tld
noinst_LTLIBRARIES += libsymm_Tld.la
libsymm_Tld_la_SOURCES = $(mattype_real_sources)
libsymm_Tld_la_CFLAGS = -DMOD_NAME=liboski_mat_CSR $(Tld_cflags)
endif

if BUILD_Tlc
noinst_LTLIBRARIES += libsymm_Tlc.la
libsymm_Tlc_la_SOURCES = $(mattype_complex_sources)
libsymm_Tlc_la_CFLAGS = -DMOD_NAME=liboski_mat_CSR $(Tlc_cflags)
endif

if BUILD_Tlz
noinst_LTLIBRARIES += libsymm_Tlz.la
libsymm_Tlz_la_SOURCES = $(mattype_complex_sources)
libsymm_Tlz_la_CFLAGS = -DMOD_NAME=liboski_mat_CSR $(Tlz_cflags)
endif

# eof
