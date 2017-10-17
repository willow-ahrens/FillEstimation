#
# Stub file common to all the testing programs.
#

include $(top_srcdir)/src/Makefile.common
include $(top_srcdir)/src/Makefile.mattypes
include $(top_srcdir)/src/Makefile.build
include $(top_srcdir)/src/Make.extlibs_util

matrix_files = \
	ibm32.pua \
	raefsky6.pua \
	cannes445.pha \
	lp_nug06.pua \
	lp_nug06-T.pua \
	12-raefsky4-cut.pua 12-raefsky4-cut.psa

EXTRA_DIST = defs.sh skip_test.sh test.sh make_def_test.sh \
	format_battery_simple.sh format_battery.sh \
	$(matrix_files) $(TESTS)
MAINTAINERCLEANFILES = Makefile.in

OSKI_BENCH_DIR = $(top_builddir)/bench

TESTS_ENVIRONMENT = top_builddir=$(top_builddir) top_srcdir=$(top_srcdir) \
	OSKI_BENCH_DIR="$(OSKI_BENCH_DIR)"

MAKE_DEF_TEST_SCRIPT = ${top_srcdir}/tests/make_def_test.sh

# ------------------------------------------------------

testcommon_cflags = -DMOD_NAME=self $(oski_util_cflags) \
	-I$(top_builddir)/include
testcommon_sources =

TESTS =
noinst_PROGRAMS =

include Make.test

# ------------------------------------------------------
distclean-local:
	-rm -rf *.err *.out *-ans

# eof
