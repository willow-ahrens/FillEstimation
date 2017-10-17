#
# Off-line benchmarking utilities.
#

include $(top_srcdir)/src/Makefile.common
include $(top_srcdir)/src/Makefile.mattypes
include $(top_srcdir)/src/Makefile.luaoski
include $(top_srcdir)/src/Makefile.build
include $(top_srcdir)/src/Make.extlibs_util

# source files common to all benchmark programs
benchcommon_sources =
benchcommon_cflags = -DMOD_NAME=self $(oski_util_cflags) \
	-I$(top_builddir)/include
benchcommon_env = top_srcdir=$(top_srcdir) \
	top_builddir=$(top_builddir) \
	srcdir=$(srcdir) \
	OSKI_BYPASS_CHECK=yes OSKI_TICKS_PER_SEC=`cat oskitimer.dat`

# Off-line benchmarking
benchbin_PROGRAMS =
EXTRA_DIST = \
	genprof-begin.sh genprof-end.sh genprof-common.sh genprof-loop2d.sh \
	genprof-MatMult.sh genprof-MatMult-MATSTD.sh \
	genprof-MatTrisolve.sh genprof-MatTrisolve-MATSTD.sh \
	genprof-MatTransMatMult.sh genprof-MatTransMatMult-MATSTD.sh \
	genprof-MatMultAndMatTransMult.sh \
	genprof-MatMultAndMatTransMult-MATSTD.sh \
	genprof-MatPowMult.sh genprof-MatPowMult-MATSTD.sh

# List of benchmark data files
benchdata_files =

# Timer calibration
benchbin_PROGRAMS += oskitimer
oskitimer_SOURCES = timer.c
oskitimer_CFLAGS = $(benchcommon_cflags)
oskitimer_LDADD = $(oski_ldadd)
oskitimer_LDFLAGS = $(oski_ldflags)

benchdata_files += oskitimer.dat
oskitimer.dat: oskitimer
	$(top_builddir)/bench/oskitimer --calibrate > oskitimer.dat

# Benchmark programs
include Make.bench

EXTRA_DIST += keropts.h workload.h matopts.h

# Off-line benchmarks to run
include Make.allfmtker

# Rule to initiate off-line benchmarking and install data files
benchmarks: $(benchdata_files)

# The following install hook does not attempt to regenerate
# benchmark data files based on its dependencies.
install-exec-hook: $(benchdata_files)
	test -z "$(benchdatadir)" || $(mkdir_p) "$(DESTDIR)$(benchdatadir)"
	$(INSTALL_DATA) $(benchdata_files) "$(DESTDIR)$(benchdatadir)/."

uninstall-local:
	for p in $(benchdata_files) ; do \
		pbase=`basename $$p` ; \
		rm -f "$(DESTDIR)/$(benchdatadir)/$$p" ; \
	done

# Clean-up
distclean-local:
	-rm -rf $(benchdata_files)

#
# eof
#
