poski_build_tests = test_main
TESTS = test_main.sh

matrix_files = Dense ibm32.pua raefsky6.pua

EXTRA_DIST = test_main.sh $(matrix_files)
MAINTAINERCLEANFILES = Makefile.in
 
OSKILIBS = $(OSKIDIR)/lib/oski

#OSKI link flags
OSKILIBS_SHARED = $(OSKILIBS)/liboski.la $(OSKILIBS)/liboski_heur_mregblock_Tid.la $(OSKILIBS)/liboski_heur_regprof_Tid.la \
	$(OSKILIBS)/liboski_heur_symmrb_Tid.la $(OSKILIBS)/liboskilt.la $(OSKILIBS)/liboski_mat_BCSR_Tid.la \
	$(OSKILIBS)/liboski_mat_CB_Tid.la \
	$(OSKILIBS)/liboski_mat_CSC_Tid.la $(OSKILIBS)/liboski_mat_CSR_Tid.la $(OSKILIBS)/liboski_mat_DENSE_Tid.la \
	$(OSKILIBS)/liboski_mat_GCSR_Tid.la $(OSKILIBS)/liboski_mat_MBCSR_Tid.la $(OSKILIBS)/liboski_mat_VBR_Tid.la \
	$(OSKILIBS)/liboski_Tid.la $(OSKILIBS)/liboski_util.la $(OSKILIBS)/liboski_util_Tid.la \
	@LIBLTDL@

#check_PROGRAMS = $(poski_build_tests)
noinst_PROGRAMS = $(poski_build_tests)
test_main_SOURCES = test_main.c 
test_main_LDADD = $(top_builddir)/src/libposki.la $(OSKILIBS_SHARED)
test_main_CFLAGS = -I$(top_srcdir)/include
#test_main_LDFLAGS = -module
#test_main_CFLAGS = -DMOD_NAME=self -I$(top_srcdir)/include
