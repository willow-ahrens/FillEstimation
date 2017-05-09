#
# Build the CSR triangular solve implementations.
#

include $(top_srcdir)/src/Makefile.common
include $(top_srcdir)/src/Makefile.mattypes

noinst_LTLIBRARIES =
TRISOLVE_SOURCES = \
	lower_normal.c lower_trans.c lower_conj.c lower_conjtrans.c \
	upper_normal.c upper_trans.c upper_conj.c upper_conjtrans.c

if BUILD_Tis
noinst_LTLIBRARIES += libtrisolve_Tis.la
libtrisolve_Tis_la_SOURCES = $(TRISOLVE_SOURCES)
libtrisolve_Tis_la_CFLAGS = -DMOD_NAME=liboski_mat_CSR $(Tis_cflags)
endif

if BUILD_Tid
noinst_LTLIBRARIES += libtrisolve_Tid.la
libtrisolve_Tid_la_SOURCES = $(TRISOLVE_SOURCES)
libtrisolve_Tid_la_CFLAGS = -DMOD_NAME=liboski_mat_CSR $(Tid_cflags)
endif

if BUILD_Tic
noinst_LTLIBRARIES += libtrisolve_Tic.la
libtrisolve_Tic_la_SOURCES = $(TRISOLVE_SOURCES)
libtrisolve_Tic_la_CFLAGS = -DMOD_NAME=liboski_mat_CSR $(Tic_cflags)
endif

if BUILD_Tiz
noinst_LTLIBRARIES += libtrisolve_Tiz.la
libtrisolve_Tiz_la_SOURCES = $(TRISOLVE_SOURCES)
libtrisolve_Tiz_la_CFLAGS = -DMOD_NAME=liboski_mat_CSR $(Tiz_cflags)
endif

if BUILD_Tls
noinst_LTLIBRARIES += libtrisolve_Tls.la
libtrisolve_Tls_la_SOURCES = $(TRISOLVE_SOURCES)
libtrisolve_Tls_la_CFLAGS = -DMOD_NAME=liboski_mat_CSR $(Tls_cflags)
endif

if BUILD_Tld
noinst_LTLIBRARIES += libtrisolve_Tld.la
libtrisolve_Tld_la_SOURCES = $(TRISOLVE_SOURCES)
libtrisolve_Tld_la_CFLAGS = -DMOD_NAME=liboski_mat_CSR $(Tld_cflags)
endif

if BUILD_Tlc
noinst_LTLIBRARIES += libtrisolve_Tlc.la
libtrisolve_Tlc_la_SOURCES = $(TRISOLVE_SOURCES)
libtrisolve_Tlc_la_CFLAGS = -DMOD_NAME=liboski_mat_CSR $(Tlc_cflags)
endif

if BUILD_Tlz
noinst_LTLIBRARIES += libtrisolve_Tlz.la
libtrisolve_Tlz_la_SOURCES = $(TRISOLVE_SOURCES)
libtrisolve_Tlz_la_CFLAGS = -DMOD_NAME=liboski_mat_CSR $(Tlz_cflags)
endif

# eof
