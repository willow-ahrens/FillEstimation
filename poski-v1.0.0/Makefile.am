ACLOCAL_AMFLAGS=-I m4

SUBDIRS = 	libltdl \
		include/poski	\
		src	\
		m4	\
		tests

benchmarks:
	echo Runing off-line tuning...	
	$(srcdir)/bench/benchmark.sh

LDADD = src/libposki.la

