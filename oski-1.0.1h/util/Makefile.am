#
# Build a small library of utilities useful for the testing
# and offline benchmarking programs. This library is not
# intended for BeBOP-OSKI end-users, though they libraries
# are distributed since the benchmark programs (which are
# distributed) require them.
#

include $(top_srcdir)/src/Makefile.common
include $(top_srcdir)/src/Makefile.mattypes

utilcommon_cflags = -DMOD_NAME=liboski_util
util_sources = \
	abort_prog.h \
	array_util.h \
	parse_opts.c parse_opts.h \
	readhbpat.h \
	rand_util.h \
	stat.h \
	testvec.h \
	timing.h
util_sources_typedep = \
	array_util.c \
	parse_opts_typedep.c \
	rand_util.c \
	readhbpat.c expand.c \
	stat.c \
	testvec.c

EXTRA_DIST =

# Run-time CPU probe
EXTRA_DIST += detect_cpu.sh
noinst_PROGRAMS = oskigetcpu
oskigetcpu_SOURCES = getcpu.c
oskigetcpu_LDFLAGS = $(support_ldflags)

pkglib_LTLIBRARIES = liboski_util.la
liboski_util_la_SOURCES = $(util_sources)
liboski_util_la_CFLAGS = $(utilcommon_cflags)
liboski_util_la_LIBADD = ../src/liboski.la ../src/corelib/liboski_core.la
liboski_util_la_LDFLAGS = $(release_ldflags)

# Type dependent
include Make.stub

#
# eof
#
