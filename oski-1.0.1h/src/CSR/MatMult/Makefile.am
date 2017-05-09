#
# Build the CSR sparse matrix-vector multiply implementations.
#

include $(top_srcdir)/src/Makefile.common
include $(top_srcdir)/src/Makefile.mattypes

EXTRA_DIST = CSR_HEADER.c

# Set source filenames
mattype_real_sources =
include Make.CSR_MatMult_Tr
mattype_complex_sources =
include Make.CSR_MatMult_Tc

noinst_LTLIBRARIES =

if BUILD_Tis
noinst_LTLIBRARIES += libmatmult_Tis.la
libmatmult_Tis_la_SOURCES = $(mattype_real_sources)
libmatmult_Tis_la_CFLAGS = -DMOD_NAME=liboski_mat_CSR $(Tis_cflags)
endif

if BUILD_Tid
noinst_LTLIBRARIES += libmatmult_Tid.la
libmatmult_Tid_la_SOURCES = $(mattype_real_sources)
libmatmult_Tid_la_CFLAGS = -DMOD_NAME=liboski_mat_CSR $(Tid_cflags)
endif

if BUILD_Tic
noinst_LTLIBRARIES += libmatmult_Tic.la
libmatmult_Tic_la_SOURCES = $(mattype_complex_sources)
libmatmult_Tic_la_CFLAGS = -DMOD_NAME=liboski_mat_CSR $(Tic_cflags)
endif

if BUILD_Tiz
noinst_LTLIBRARIES += libmatmult_Tiz.la
libmatmult_Tiz_la_SOURCES = $(mattype_complex_sources)
libmatmult_Tiz_la_CFLAGS = -DMOD_NAME=liboski_mat_CSR $(Tiz_cflags)
endif

if BUILD_Tls
noinst_LTLIBRARIES += libmatmult_Tls.la
libmatmult_Tls_la_SOURCES = $(mattype_real_sources)
libmatmult_Tls_la_CFLAGS = -DMOD_NAME=liboski_mat_CSR $(Tls_cflags)
endif

if BUILD_Tld
noinst_LTLIBRARIES += libmatmult_Tld.la
libmatmult_Tld_la_SOURCES = $(mattype_real_sources)
libmatmult_Tld_la_CFLAGS = -DMOD_NAME=liboski_mat_CSR $(Tld_cflags)
endif

if BUILD_Tlc
noinst_LTLIBRARIES += libmatmult_Tlc.la
libmatmult_Tlc_la_SOURCES = $(mattype_complex_sources)
libmatmult_Tlc_la_CFLAGS = -DMOD_NAME=liboski_mat_CSR $(Tlc_cflags)
endif

if BUILD_Tlz
noinst_LTLIBRARIES += libmatmult_Tlz.la
libmatmult_Tlz_la_SOURCES = $(mattype_complex_sources)
libmatmult_Tlz_la_CFLAGS = -DMOD_NAME=liboski_mat_CSR $(Tlz_cflags)
endif

# eof
