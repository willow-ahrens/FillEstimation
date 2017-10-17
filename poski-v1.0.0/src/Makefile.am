#
# Main pOSKI Library
#
# Add all files for poski library

SRC=	Common/poski_Common.c	\
	Common/poski_InitCloseHandler.c	\
	Common/poski_SysConfigureHandler.c	\
	Common/poski_LogFileHandler.c	\
	Common/poski_MessageHandler.c	\
	Common/poski_Malloc.c	\
	\
	LoadMatrix/poski_LoadMatrixHandler.c	\
	LoadMatrix/poski_LoadMatrix_common.c	\
	LoadMatrix/poski_LoadMatrix_HB_to_CSR.c	\
	LoadMatrix/poski_LoadMatrix_MM_to_CSR.c		\
	\
	Kernel/poski_KernelHandler.c	\
	Kernel/poski_Kernel_common.c	\
	Kernel/poski_Kernel_matmult.c	\
	\
	Thread/poski_ThreadHandler.c	\
	Thread/poski_Thread_common.c	\
	Thread/poski_Thread_NUMA.c	\
	\
	Partition/poski_PartitionHandler.c	\
	Partition/poski_Partition_common.c	\
	Partition/poski_Partition_OneD.c	\
	Partition/poski_Partition_SemiOneD.c	\
	\
	Matrix/poski_MatrixHandler.c	\
	Matrix/poski_Matrix_common.c	\
	Matrix/poski_Matrix_getset.c	\
	\
	Vector/poski_VectorHandler.c	\
	Vector/poski_Vector_common.c	\
	\
	TuneMat/poski_TuneMatHandler.c	\
	TuneMat/poski_TuneMat_common.c	\
	\
	Permute/poski_PermuteHandler.c	\
	\
	Test/poski_TestHandler.c	\
	Test/poski_Test_NaiveSpMV.c	

OSKILIBS = ${OSKIDIR}/lib/oski

#OSKI link flags
OSKILIBS_SHARED = $(OSKILIBS)/liboski.la $(OSKILIBS)/liboski_heur_mregblock_Tid.la $(OSKILIBS)/liboski_heur_regprof_Tid.la \
	$(OSKILIBS)/liboski_heur_symmrb_Tid.la $(OSKILIBS)/liboskilt.la $(OSKILIBS)/liboski_mat_BCSR_Tid.la \
	$(OSKILIBS)/liboski_mat_CB_Tid.la \
	$(OSKILIBS)/liboski_mat_CSC_Tid.la $(OSKILIBS)/liboski_mat_CSR_Tid.la $(OSKILIBS)/liboski_mat_DENSE_Tid.la \
	$(OSKILIBS)/liboski_mat_GCSR_Tid.la $(OSKILIBS)/liboski_mat_MBCSR_Tid.la $(OSKILIBS)/liboski_mat_VBR_Tid.la \
	$(OSKILIBS)/liboski_Tid.la $(OSKILIBS)/liboski_util.la $(OSKILIBS)/liboski_util_Tid.la 


OSKILIBS_STATIC = $(OSKILIBS)/liboski.a $(OSKILIBS)/liboski_heur_mregblock_Tid.a $(OSKILIBS)/liboski_heur_regprof_Tid.a \
	$(OSKILIBS)/liboski_heur_symmrb_Tid.a $(OSKILIBS)/liboskilt.a $(OSKILIBS)/liboski_mat_BCSR_Tid.a \
	$(OSKILIBS)/liboski_mat_CB_Tid.a \
	$(OSKILIBS)/liboski_mat_CSC_Tid.a $(OSKILIBS)/liboski_mat_CSR_Tid.a $(OSKILIBS)/liboski_mat_DENSE_Tid.a \
	$(OSKILIBS)/liboski_mat_GCSR_Tid.a $(OSKILIBS)/liboski_mat_MBCSR_Tid.a $(OSKILIBS)/liboski_mat_VBR_Tid.a \
	$(OSKILIBS)/liboski_Tid.a $(OSKILIBS)/liboski_util.a $(OSKILIBS)/liboski_util_Tid.a  

lib_LTLIBRARIES=libposki.la
libposki_la_SOURCES= $(SRC)
libposki_la_LIBADD = $(LIB_KPARTS) $(OSKILIBS_SHARED)
# to link libtool's modle
libposki_la_LDFLAGS = -module
#libposki_la_CFLAGS = -I$(top_srcdir)/include \
#	-fno-rtti -fno-exceptions -parallel -restrict -ipo -no-prec-div -fno-inline-functions	\
#	-DMOD_NAME=libposki	\
#	-DPOSKI_PACKAGE_STRING="\"$(PACKAGE_STRING)\""	\
#	-DPOSKI_LIB_PATH_DEFAULT="\"$(pkglibdir)\""	\
#	-DPOSKI_CC="\"${CC}\""	\
#	-DPOSKI_CFLAGS="\"${CFLAGS}\""	\
#	-DPOSKI_LDFLAGS="\"${LDFLAGS}\""	\
#	-DPOSKI_SUPPORT_LDFLAGS="\"${support_ldflags}\"" 
libposki_la_CFLAGS = -I${top_srcdir}/include \
	-DMOD_NAME=libposki	\
	-DPOSKI_PACKAGE_STRING="\"${PACKAGE_STRING}\""	\
	-DPOSKI_LIB_PATH_DEFAULT="\"${pkglibdir}\""	\
	-DPOSKI_CC="\"${CC}\""	\
	-DPOSKI_CFLAGS="\"${CFLAGS}\""	\
	-DPOSKI_LDFLAGS="\"${LDFLAGS}\""	\
	-DPOSKI_SUPPORT_LDFLAGS="\"${support_ldflags}\"" 
# eof



