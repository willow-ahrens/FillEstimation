#include <stdio.h>
#include <stdlib.h>

#include <poski/poski.h>

#define MAX_TTYPE 3
#define MAX_PTYPE 2

#define TO_STRING(x) # x

char 			*MATFILE[1] 	= {"Small"};
poski_threadtype_t 	TTYPE[3] 	= {POSKI_PTHREAD, POSKI_THREADPOOL, POSKI_OPENMP};	
poski_partitiontype_t 	PTYPE[2] 	= {OneD, SemiOneD};		
poski_copymode_t 	MODE[2] 	= {SHARE_INPUTMAT, COPY_INPUTMAT};		
poski_tunehint_t 	HINT[2] 	= {HINT_SINGLE_BLOCK, HINT_MULTIPLE_BLOCKS}; 	
poski_kernel_t 		KERNEL[1] 	= {KERNEL_MatMult};		
poski_operation_t 	OP[2] 		= {OP_NORMAL, OP_TRANS};	
int 			TUNE[2]		= {0, 1};

char *find_thread(poski_threadtype_t ttype);
char *find_partition(poski_partitiontype_t ptype);
char *find_kernel(poski_kernel_t kernel);
char *find_operation(poski_operation_t op);
char *find_mode(poski_copymode_t mode);
char *find_tune(poski_int_t tune);
char *find_hint(poski_tunehint_t hint);
char *find_modify(poski_int_t modify);
void getArgs(int argc, char **argv);

void Naive_SpMV (poski_sparse_matrix_t *A, poski_value_t alpha, poski_value_t *x, poski_value_t beta, poski_value_t *y);
void OSKI_SpMV (poski_sparse_matrix_t *A, poski_value_t alpha, poski_value_t *x, poski_value_t beta, poski_value_t *y);
void pOSKI_SpMV (poski_sparse_matrix_t *A, poski_value_t alpha, poski_value_t *x, poski_value_t beta, poski_value_t *y);

/**********************************************************
 * @ Name 	: main
 * @ Action 	: 
 * @ Return 	: 0 if success, error message otherwise 
 * @ Fix 	:
 **********************************************************/
// test option
int num; int report; int iter;
// matrix option
char *matfile; poski_MatFormat_t matformat=HB; poski_copymode_t mode;
// tune option
poski_int_t tune; poski_tunehint_t hint;
// kernel option
poski_kernel_t kernel; poski_operation_t op;
// thread option
poski_threadtype_t ttype; poski_int_t nthreads;
// partition option
poski_partitiontype_t ptype; poski_int_t npartitions;
// matrix, vector
poski_sparse_matrix_t *A; int in_length, out_length;

int main(int argc, char **argv)
{
	int ttype_id, ptype_id;
	int tid, pid;
	int error=0;
	int count=0;

	// initialize pOSKI library
	poski_Init();

	// get arguments
	getArgs(argc, argv);

	// print the status
	//if (report==1) poski_printMessage(0, "Test%d:\n", num);
	//else poski_printMessage(0, "Test%d...", num);

	// get the number of cores on the system
	int ncores = poski_system_configure();

	// create a shared sparse matrix in CSR format 
	if (matfile=="Small")
	{
		// 10x10 with 20 nnz
		A = SetTestSampleSparseMatrix();
	}
	else
	{	
		// load the nonzero pattern from a HB format
		A = poski_LoadMatrixFile_to_CSR(matfile, matformat);
	}
	
	// report the arguments
	if(report==1)
	{
		poski_printMessage(0,"\t+ matfile = %s\n", matfile);
		poski_printMessage(0,"\t+ matrix = %dx%d with %d\n", A->nrows, A->ncols, A->nnz);
		poski_printMessage(0,"\t+ copy_mode = %s\n", find_mode(mode));
		poski_printMessage(0,"\t+ tune = %s, hint = %s\n", find_tune(tune), find_hint(hint));
		poski_printMessage(0,"\t+ kernel = %s, op = %s\n", find_kernel(kernel), find_operation(op));
		poski_printMessage(0,"\t+ ttype = {POSKI_PTHREAD, POSKI_THREADPOOL, POSKI_OPENMP}, nthreads = {1, ..., %d}\n", ncores);
		poski_printMessage(0,"\t+ ptype = {OneD, SemiOneD}, npartitions = {1, ..., %d}\n", (ncores*2));
	}

	// initialize the shared input parameters for y=alpha*op(A)x + beta*y
	// set the size of input and output vector
	if(op==OP_NORMAL)
	{
		in_length = A->ncols;
		out_length = A->nrows;
	}
	else if (op==OP_TRANS)
	{
		in_length = A->nrows;
		out_length = A->ncols;
	}

	// set the shared alpha and beta 	
	poski_value_t alpha = 3.3;
	poski_value_t beta = 2.3;	

	// create the shared input vector x
	poski_value_t *x;
	x = CreateTestSingleVec(in_length, RANDOM);

	// private output vectors for Naive, OSKI, and pOSKI
	poski_value_t *y1, *y2, *y3;
	
	// run Naive once
	y1 = CreateTestSingleVec(out_length, ZERO);
	Naive_SpMV(A, alpha, x, beta, y1);
	
	// run OSKI once
	y2 = CreateTestSingleVec(out_length, ZERO);
	OSKI_SpMV(A, alpha, x, beta, y2);	

	// run pOSKI multiple times with different parallel options 
	for (ttype_id=0; ttype_id<MAX_TTYPE; ttype_id++)
	{	// {POSKI_PTHREAD, POSKI_THREADPOOL, POSKI_OPENMP}
		ttype = TTYPE[ttype_id];
		for (tid=1;tid<=(ncores);tid++)	
		{	// 1 <= nthreads <= ncores
			nthreads = tid;
			for (ptype_id=0; ptype_id<MAX_PTYPE; ptype_id++)
			{	// {OneD, SemiOneD}
				ptype = PTYPE[ptype_id];
				for (pid=1; pid<=(nthreads*2);pid++)
				{	// 1 <= npartitions <= 2*nthreads
					npartitions = pid;

					// run pOSKI with nthreads and npartitions
					y3 = CreateTestSingleVec(out_length, ZERO);
					pOSKI_SpMV(A, alpha, x, beta, y3);
					count++;
					// check the output with the output from OSKI
					if (poski_check_TwoVectors(A, y1, y3, out_length)<0)
					{
						error++;
					}
				}
			}
		}
	}

	// report the status
	if (report==1) { poski_printMessage(0, "Passed %d, failed %d out of %d tests on Test%d!\n",(count-error), error, count, num); TEST_LINECUT(1); }
	else poski_printMessage1(0, "Passed %d, failed %d out of %d tests on Test%d!\n", (count-error), error, count, num);

	// destroy the shared sparse matrix in CSR and vectors
	poski_Destroy_CSR(A);
	poski_free(x); poski_free(y1); poski_free(y2); poski_free(y3);

	// close pOSKI library
	poski_Close();

	return 0;
}

// Naive routine
void Naive_SpMV (poski_sparse_matrix_t *A, poski_value_t alpha, poski_value_t *x, poski_value_t beta, poski_value_t *y)
{
	int i;
	
	for(i=0;i<iter;i++)
		poski_NavieMatMultCSR(A, alpha, x, beta, y, op);
}

// OSKI routine
void OSKI_SpMV (poski_sparse_matrix_t *A, poski_value_t alpha, poski_value_t *x, poski_value_t beta, poski_value_t *y)
{
	int i;
	
	oski_matrix_t A_tunable = oski_CreateMatCSR(A->Aptr, A->Aind, A->Aval, A->nrows, A->ncols, mode, 2, INDEX_ZERO_BASED, MAT_GENERAL);

	oski_vecview_t x_view=oski_CreateVecView(x, in_length, STRIDE_UNIT);
	oski_vecview_t y_view=oski_CreateVecView(y, out_length, STRIDE_UNIT);

	if (tune==1)
	{
		if (hint==HINT_SINGLE_BLOCK) oski_SetHint(A_tunable, HINT_SINGLE_BLOCK, 2, 2);
		else if (hint==HINT_MULTIPLE_BLOCKS) oski_SetHint(A_tunable, HINT_MULTIPLE_BLOCKS, 2, 3, 3, 4, 4);
		oski_SetHintMatMult(A_tunable, op, 1, SYMBOLIC_VEC, 1, SYMBOLIC_VEC, 500);
		oski_TuneMat(A_tunable);
	}

	for(i=0;i<iter;i++)
		oski_MatMult(A_tunable, op, alpha, x_view, beta, y_view);
	
	oski_DestroyMat(A_tunable);
	oski_DestroyVecView(x_view);
	oski_DestroyVecView(y_view);
}

// POSKI routine
void pOSKI_SpMV (poski_sparse_matrix_t *A, poski_value_t alpha, poski_value_t *x, poski_value_t beta, poski_value_t *y)
{
	int i;

	poski_threadarg_t *threadargA = poski_InitThreads();	
	poski_ThreadHints(threadargA, NULL, ttype, nthreads);	

	poski_partitionarg_t *partitionargA = poski_partitionMatHints(ptype, npartitions, KERNEL_MatMult, op);

	poski_mat_t A_tunable = poski_CreateMatCSR(A->Aptr, A->Aind, A->Aval, A->nrows, A->ncols, A->nnz, mode, threadargA, partitionargA, 2, INDEX_ZERO_BASED, MAT_GENERAL);

	if (tune==1)
	{
		if (hint==HINT_SINGLE_BLOCK) poski_TuneHint_Structure(A_tunable, HINT_SINGLE_BLOCK, 2, 2);
		else if (hint==HINT_MULTIPLE_BLOCKS) poski_TuneHint_Structure(A_tunable, HINT_MULTIPLE_BLOCKS, 2, 3, 3, 4, 4);
		poski_TuneHint_MatMult(A_tunable, op, 1, SYMBOLIC_VECTOR, 1, SYMBOLIC_VECTOR, 500);
		poski_TuneMat(A_tunable);
	}

	poski_partitionvec_t *partitionVec1 = poski_PartitionVecHints(A_tunable, KERNEL_MatMult, op, INPUTVEC);
	poski_partitionvec_t *partitionVec2 = poski_PartitionVecHints(A_tunable, KERNEL_MatMult, op, OUTPUTVEC);

	poski_vec_t x_view = poski_CreateVec(x, in_length, STRIDE_UNIT, partitionVec1);
	poski_vec_t y_view = poski_CreateVec(y, out_length, STRIDE_UNIT, partitionVec2);

	for(i=0;i<iter;i++)
		poski_MatMult(A_tunable, op, alpha, x_view, beta, y_view);

	poski_DestroyThreads(threadargA);
	poski_DestroyPartitionMat(partitionargA);
	poski_DestroyPartitionVec(partitionVec1);
	poski_DestroyPartitionVec(partitionVec2);
	poski_DestroyVec(x_view);
	poski_DestroyVec(y_view);
	poski_DestroyMat(A_tunable);
}

char *find_thread(poski_threadtype_t ttype)
{
	if (ttype==POSKI_PTHREAD) return TO_STRING(POSKI_PTHREAD);
	else if (ttype==POSKI_THREADPOOL) return TO_STRING(POSKI_THREADPOOL);
	else if (ttype==POSKI_OPENMP) return TO_STRING(POSKI_OPNEMP);
	else return "ERROR";
}
char *find_partition(poski_partitiontype_t ptype)
{
	if (ptype==OneD) return TO_STRING(OneD);
	else if (ptype==SemiOneD) return TO_STRING(SemiOneD);
	else return "ERROR";
}
char *find_kernel(poski_kernel_t kernel)
{
	if (kernel==KERNEL_MatMult) return TO_STRING(MatMult);
	else return "ERROR";
}
char *find_operation(poski_operation_t op)
{
	if (op==OP_NORMAL) return TO_STRING(OP_NORMAL);
	else if (op==OP_TRANS) return TO_STRING(OP_TRANS);
	else return "ERROR";
}
char *find_mode(poski_copymode_t mode)
{
	if (mode==SHARE_INPUTMAT) return TO_STRING(SHARE_INPUTMAT);
	else if (mode==COPY_INPUTMAT) return TO_STRING(COPY_INPUTMAT);
	else return "ERROR";
}
char *find_tune(poski_int_t tune)
{
	if (tune == 0) return "NO";
	else if (tune == 1) return "YES";
	else return "ERROR";
}
char *find_hint(poski_tunehint_t hint)
{
	if (hint==HINT_SINGLE_BLOCK) return TO_STRING(SINGLE_BLOCK);
	else if (hint==HINT_MULTIPLE_BLOCKS) return TO_STRING(MULTIPLE_BLOCKS);
	else return "ERROR";
}
char *find_modify(poski_int_t modify)
{
	if (modify==0) return "NO";
	else if (modify==1) return "YES";
	else return "ERROR";
}

void getArgs(int argc, char **argv)
{
	// initialize 
	num = 1;		// first test
	iter = 3;		// 3 iterations
	report = 1;		// report
	matfile = "Small";	// test with small sparse matrix
	mode = MODE[0];		// shared matrix 
	tune = 0;		// no tune
	hint = HINT[0];		// single block 
	kernel = KERNEL[0];	// SpMV
	op = OP[0];		// Ax
	
	// get user's inputs
	int i;
	for (i=1; i<argc; i++)
	{
		if (argv[i][0] == '-')
		{
			switch(argv[i][1])
			{
				case 'n': num = atoi(argv[++i]); break;
				case 'i': iter = atoi(argv[++i]); break;
				case 'r': report = atoi(argv[++i]); break;
				//case 'f': matfile = MATFILE[atoi(argv[++i])]; break;
				case 'f': matfile = argv[++i]; printf("matrix = %s\n",matfile); break;
				case 'm': mode = MODE[atoi(argv[++i])]; break;
				case 't': tune = TUNE[atoi(argv[++i])]; break;
				case 'h': hint = HINT[atoi(argv[++i])]; break;
				case 'k': kernel = KERNEL[atoi(argv[++i])]; break;
				case 'o': op = OP[atoi(argv[++i])]; break;
				default: fprintf(stderr, "Unknown switch %s\n", argv[i]); break;
			}
		}
	}
}

