/**
 * @file src/TuneMat/poski_TuneMat_common.c
 * @brief 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <poski/poski_threadcommon.h>
#include <poski/poski_matrixcommon.h>
#include <poski/poski_tunematcommon.h>
#include <poski/poski_matmulttype.h>
#include <poski/poski_kerneltype.h>
#include <poski/poski_malloc.h>
#include <poski/poski_print.h>

/* -------------------------------------------------------------------
 *  Private to this module.
 */

/** Fraction of observed workload available for tuning. */
//#define BEST_FRAC_OBSERVED .25

/** Fraction of hint workload available for tuning. */
//#define BEST_FRAC_WORKLOAD .25

/**
 *  \brief Returns the estimated number of seconds available for tuning.
 *
 *  The estimate is based on the larger of the following two
 *  quantities:
 *    - Estimated time to execute the trace determined by the
 *      workload hints.
 *    - The actual accumulated kernel execution time so far.
 */
/*
static double
ComputeTuningBudget (oski_matrix_t A_tunable)
{
	double time_workhints;
	double time_measured;

	if (A_tunable == INVALID_MAT)
		return 0.0;

	time_workhints = oski_EstimateTraceTimeTotal (A_tunable->workhints,
			A_tunable->time_stream);
	time_measured = oski_EstimateTraceTimeTotal (A_tunable->trace,
			A_tunable->time_stream);

	poski_printMessage (2, "Tuning budget computation:");
	poski_printMessage (2,
			"   Workload hints (simulated): %g seconds",
			time_workhints);
	poski_printMessage (2, "   Observed trace: %g seconds", time_measured);

	if (time_workhints > time_measured)
		return time_workhints * BEST_FRAC_WORKLOAD;
	else
		return time_measured * BEST_FRAC_OBSERVED;
}
*/
/**
 *  \brief Determines whether the heuristic-selected data structure
 *  leads to faster execution times than the input data structure.
 *
 *  \returns 1 if the tuned implementation is faster, and 0 otherwise.
 *
 *  \todo Implement this routine, given that we will need to
 *  allocate temporary vectors.
 */
/*
static int
ChooseFastest (oski_matrix_t A_tunable)
{
	oski_id_t id_save;
	oski_index_t m, n; // dimensions

	// temporary vectors
	oski_value_t* x;
	oski_value_t* y;
	oski_vecstruct_t xv;
	oski_vecstruct_t yv;

	// timing
	oski_timer_t timer = oski_CreateTimer();
	double t_ref, t_tuned;

	int err;

	if (A_tunable == INVALID_MAT || A_tunable->tuned_mat.type_id == INVALID_ID
			|| timer == INVALID_TIMER)
		return 0; // Don't know, so assume tuned is slower

	m = A_tunable->props.num_rows;
	n = A_tunable->props.num_cols;

	err = oski_MultiMalloc (NULL, __LINE__, 2,
			(size_t)(sizeof(oski_value_t)*m), &y,
			(size_t)(sizeof(oski_value_t)*n), &x);
	if (err) { oski_DestroyTimer(timer); return 0; }

	// "warm" cache
	oski_ZeroMem (x, sizeof(oski_value_t)*n);
	oski_ZeroMem (y, sizeof(oski_value_t)*m);

	xv.num_rows = n;
	xv.num_cols = 1;
	xv.orient = LAYOUT_ROWMAJ;
	xv.stride = 1;
	xv.val = x;
	xv.rowinc = 1;
	xv.colinc = 1;

	yv.num_rows = m;
	yv.num_cols = 1;
	yv.orient = LAYOUT_ROWMAJ;
	yv.stride = 1;
	yv.val = y;
	yv.rowinc = 1;
	yv.colinc = 1;

	// Warm up call 
	oski_MatMult (A_tunable, OP_NORMAL, TVAL_ONE, &xv, TVAL_ZERO, &yv);

	// Time tuned implementation 
	oski_RestartTimer (timer);
	oski_MatMult (A_tunable, OP_NORMAL, TVAL_ONE, &xv, TVAL_ZERO, &yv);
	oski_StopTimer (timer);
	t_tuned = oski_ReadElapsedTime (timer);

	// Temporarily "hide" the tuned implementation to time the reference 
	id_save = A_tunable->tuned_mat.type_id;
	A_tunable->tuned_mat.type_id = INVALID_ID;

	// Warm up reference call
	oski_MatMult (A_tunable, OP_NORMAL, TVAL_ONE, &xv, TVAL_ZERO, &yv);

	// Time reference 
	oski_RestartTimer (timer);
	oski_MatMult (A_tunable, OP_NORMAL, TVAL_ONE, &xv, TVAL_ZERO, &yv);
	oski_StopTimer (timer);
	t_ref = oski_ReadElapsedTime (timer);
	A_tunable->tuned_mat.type_id = id_save; // unhide 

	// clean-up 
	oski_FreeAll (2, x, y);
	oski_DestroyTimer (timer);

	if (1.05*t_ref < t_tuned)
	{
		poski_printMessage (4, "Ref is %gx faster than tuned!",
				t_tuned / t_ref);
		poski_printMessage (5, "t_ref = %g s", t_ref);
		poski_printMessage (5, "t_tuned = %g s", t_tuned);
		return 0;
	}
	else
		return 1;
}

static void
FreeTunedMat (oski_matrix_t A_tunable)
{
	if (A_tunable != INVALID_MAT && A_tunable->tuned_mat.type_id != INVALID_ID)
	{
		oski_DestroyMatRepr_funcpt func_DestroyTuned =
			OSKI_MATTYPEID_METHOD (A_tunable->tuned_mat.type_id, DestroyMatRepr);
		if (func_DestroyTuned != NULL)
		{
			(*func_DestroyTuned) (A_tunable->tuned_mat.repr);
			A_tunable->tuned_mat.repr = NULL;
			A_tunable->tuned_mat.type_id = INVALID_ID;
		}
		oski_Free (A_tunable->tuned_xforms);
		A_tunable->tuned_xforms = NULL;
	}
}
*/
/* -------------------------------------------------------------------
 *  Public
 */

/**
 *  \brief
 *
 *  Basic outline of this routine's implementation:
 *  \code
WHILE
    !IsTuned(A)
    AND tuning_time_left > 0
    AND i_heur <= NUM_HEURISTICS
DO
    LET heur = GetHeuristic( i_heur );
    LET results = NULL

    IF GetTotalCostEstimate(heur, A) <= tuning_time_left THEN
        LET t0 = GetTimer();
        results = EvaluateHeuristic( heur, A );
            // results == NULL if heuristic does not apply to A
        LET elapsed_time = GetTimer() - t0;
        tuning_time_left -= elapsed_time;
    ENDIF

    IF results THEN
        LET t0 = GetTimer();
        A_tuned = ApplyHeuristic( heur, results, A );
            // convert A to new data structure
        LET elapsed_time = GetTimer() - t0;
        tuning_time_left -= elapsed_time;

        A = ChooseFastest( A, A_tuned, A->trace );
    ENDIF

    i_heur = i_heur + 1;
DONE
 *  \endcode
 *
 *  \todo The current implementation does not try to re-tune
 *  if already tuned.
 *  \todo Check that the new data structure really is faster
 *  than the old.
 */
/*
int poski_TuneSubMat (poski_matstruct_t A_tunable)
{
	double tuning_time_left;
	size_t i_heur, num_heur;

	if (A_tunable->tuned_mat.type_id != INVALID_ID)	// Already tuned 
		return TUNESTAT_AS_IS;

	poski_printMessage (1, "Tuning");

	// Try each heuristic.
	tuning_time_left = ComputeTuningBudget (A_tunable);
	i_heur = 1;
	num_heur = oski_GetNumHeur ();

	poski_printMessage (2, "Checking %d heuristics...", num_heur);
	while ((A_tunable->tuned_mat.type_id == INVALID_ID)
			&& (tuning_time_left > 0) && (i_heur <= num_heur))
	{
		oski_id_t id_heur;
		double heur_cost;
		void *results;

		id_heur = oski_LookupHeurIdByNum (i_heur);
		i_heur++;
		if (id_heur == INVALID_ID)
			continue;

		poski_printMessage (2, "Trying heuristic %d (ID=%d): '%s'",
				(int) i_heur - 1, (int) id_heur,
				oski_LookupHeurDescById (id_heur));
		poski_printMessage (2, "%g seconds remain", tuning_time_left);

		if (!oski_IsHeurApplicable (id_heur, A_tunable))
		{
			poski_printMessage (3, "Heuristic does not apply.");
			continue;
		}

		heur_cost = oski_EstimateHeurCost (id_heur, A_tunable);
		if (heur_cost > tuning_time_left)
		{
			poski_printMessage (3,
					"Heuristic is too expensive (~%d SpMVs)",
					(int) (ceil
						(heur_cost /
						 A_tunable->time_stream)));
			continue;
		}

		// Heuristic is cheap enough to run, so do it.
		oski_RestartTimer (A_tunable->timer);

		poski_printMessage (3, "Evaluating heuristic...");
		results = oski_EvaluateHeur (id_heur, A_tunable);
		if (results != NULL)
		{
			int is_shared = A_tunable->is_shared; // save shared state 

			poski_printMessage (3, "Applying heuristic...");

			A_tunable->is_shared = 1; // pretend input mat is shared
			oski_ApplyHeurResults (id_heur, results, A_tunable);
			if (ChooseFastest (A_tunable))
			{
				poski_printMessage (4, "Keeping tuned matrix. (%d)",
						A_tunable->tuned_mat.type_id);
				if (!is_shared)
					oski_FreeInputMatRepr (A_tunable);
			}
			else
			{
				poski_printMessage (4, "Reverting to reference format.");
				FreeTunedMat (A_tunable);
			}
			A_tunable->is_shared = is_shared; // restore original state
		}
		else
			poski_printMessage (3, "Skipping this optimization...");

		oski_StopTimer (A_tunable->timer);
		tuning_time_left -= oski_ReadElapsedTime (A_tunable->timer);

	}				// while not tuned 

	if (A_tunable->tuned_mat.type_id != INVALID_ID)
		return TUNESTAT_NEW;
	else
		return TUNESTAT_AS_IS;
}
*/
/* eof */


/**
 *  @brief
 *
 *  @param[in] A_tunable Valid matrix handle.
 *  @returns A newly allocated string representing the
 *  transformation/data structure that has been applied to \f$A\f$.
 *  @note The caller must free the returned string.
 */
char *poski_GetMatTransforms (const poski_matstruct_t A_tunable)
{
	char *xforms;
	
	if (A_tunable == INVALID_MAT)
	{
		poski_error("poski_GetMatTransforms", "invalid matrix object");
		return NULL;
	}

	if (A_tunable->tuned_xforms != NULL)
		xforms = poski_DuplicateString (A_tunable->tuned_xforms);
	else				/* No transformations */
		xforms = poski_DuplicateString ("return InputMat;");

	// xforms == NULL only if an error occurred.
	if (xforms == NULL)
		poski_error("poski_GetMatTransforms", "out of memory");

	return xforms;
}

void poski_TuneMat_status(const poski_matstruct_t A_tunable, int pid, int err)
{
	char *xform = "(no transformation)";

	switch (err)
	{
		case TUNESTAT_AS_IS:
			poski_printMessage (2, "+ submatrix: Did NOT tune.\n");
			break;

		case TUNESTAT_NEW:
			xform = poski_GetMatTransforms (A_tunable);
			poski_printMessage (2, "+ submatrix: DID tune: '%s'.\n", xform);
			break;

		default:
			poski_printMessage (2, "+ submatrix: No tuning status.\n");
			break;
	}
}
		

#if defined( HAVE_PTHREAD_H )
static void *TuneMat_pthread(void *threads)
{
	poski_submat_t *thread = (poski_submat_t *)threads;
	
	int pid;
	int num_partitions = thread->npartitions;
	
	for (pid=0; pid<num_partitions; pid++)
	{
		int err = poski_TuneSubMat(thread[pid].submat);
		poski_TuneMat_status(thread[pid].submat, pid, err);
	}

	pthread_exit(NULL);
}
static int poski_TuneMat_pthread(poski_mat_t A)
{
	int tid;

	poski_int_t nthreads = A->threadargs.nthreads;
	poski_int_t npartitions = A->partitionargs.npartitions;

	// [parallel] Local malloc, NUMA, assume (npartitions%nthreads = {1, 2, ...})
	poski_submat_t *thread = A->submatrix;
	poski_pthread_t *threads = poski_malloc(poski_pthread_t, nthreads, 0);

	int num_partitions = (int)(npartitions/nthreads);
	for (tid=0; tid<nthreads; tid++)
	{
		poski_SetAffinity_np(tid);
		thread[tid*num_partitions].npartitions = num_partitions;
		pthread_create(&threads[tid], NULL, TuneMat_pthread, (void *)&thread[tid*num_partitions]);
	}

	for (tid=0; tid<nthreads; tid++)
	{
		pthread_join(threads[tid], NULL);
	}
	
	poski_free(threads);

	return 0;
}
#else
static int poski_TuneMat_pthread(poski_mat_t A)
{
	poski_error("poski_TuneMat_pthread", "The pOSKI library was not built with <pthread.h>");
	return -1;
}
#endif	// HAVE_PTHREAD_H

#if defined( HAVE_OPENMP_H)
void TuneMat_openmp(void *threads)
{
	poski_submat_t *thread = (poski_submat_t *)threads;
	
	int pid;
	int num_partitions = thread->npartitions;

	for (pid=0; pid<num_partitions; pid++)
	{
		int err = poski_TuneSubMat(thread[pid].submat);
		poski_TuneMat_status(thread[pid].submat, pid, err);
	}
}

static int poski_TuneMat_openmp(poski_mat_t A)
{
	int nthreads, npartitions;
	nthreads = A->threadargs.nthreads;
	npartitions = A->partitionargs.npartitions;
	int pid, tid, num_partitions;

	poski_submat_t *thread = A->submatrix;

	num_partitions = (int)(npartitions/nthreads);
	// [parallel] Local malloc, NUMA, assume (npartitions%nthreads == 0)
#pragma omp parallel for num_threads(nthreads)
	for (tid=0; tid<nthreads; tid++)
	{
		poski_SetAffinity_np(tid);
		thread[tid*num_partitions].npartitions = num_partitions;
		TuneMat_openmp(&thread[tid*num_partitions]);
	}

	return 0;	// success!
}
#else
static int poski_TuneMat_openmp(poski_mat_t A)
{
	poski_error("poski_TuneMat_openmp", "The pOSKI library was not built with <omp.h>");
	return -1;
}
#endif	// HAVE_OPENMP_H

#if defined ( HAVE_PTHREAD_H )
static int poski_TuneMat_threadpool(poski_mat_t A)
{
	poski_int_t nthreads = A->threadargs.nthreads;
	poski_int_t npartitions = A->partitionargs.npartitions;
	poski_threadpool_t *threadpool = A->threadargs.thread;

	int pid, tid, num_partitions;
	
	// [parallel] Local malloc, NUMA, assume (npartitions%nthreads = {1, 2, ...})
	poski_matmult_t *thread = poski_malloc(poski_matmult_t, nthreads, 0);

	num_partitions = (int)(npartitions/nthreads);

	for (tid=0; tid<nthreads; tid++)
	{
		int start = tid*num_partitions;
		thread[tid].tid = tid;
		thread[tid].num_partitions = num_partitions;
		thread[tid].submat = &A->submatrix[start];
		thread[tid].op = OP_NORMAL;
		thread[tid].alpha = 0;
		thread[tid].beta = 0;
		thread[tid].x = NULL;
		thread[tid].y = NULL;
		threadpool[tid].Job = TUNEMAT;
		threadpool[tid].kernel = ((void *)&thread[tid]);
	}
	poski_barrier_wait (threadpool->StartBarrier, (nthreads+1));
	poski_barrier_wait (threadpool->EndBarrier, (nthreads+1));

	poski_free(thread);

	return 0;
}
#else
static int poski_TuneMat_threadpool(poski_mat_t A)
{
	poski_error("poski_TuneMat_threadpool", "The pOSKI library was not built with <pthread.h>");
	return -1;
}
#endif	// HAVE_PTHREAD_H

/**
 * @brief Tune matrix using the selected threading model.
 * @internal
 * @param[in] A_tunable Tunable matrix object.
 *
 * @return 0 if success, or an error message otherwise.
 */
int poski_TuneMat_run(poski_mat_t A_tunable)
{
	poski_threadtype_t ttype = A_tunable->threadargs.ttype;
	switch (ttype)
	{
		case POSKI_PTHREAD:
		{
			poski_TuneMat_pthread(A_tunable);
			break;
		}
		case POSKI_OPENMP:
		{
			poski_TuneMat_openmp(A_tunable); 
			break;
		}
		case POSKI_THREADPOOL:
		{
			poski_TuneMat_threadpool(A_tunable);
			break;
		}
		default:
		{
			poski_TuneMat_threadpool(A_tunable);
			break;
		}	
	}

	return 0;
}

int poski_TuneHint_Structure_run(poski_mat_t A_tunable, poski_tunehint_t hint, int k, int *r, int *c)
{
	oski_structhint_t *hint_rec;

	int npartitions = A_tunable->partitionargs.npartitions;

	poski_matstruct_t A;

	int pid;
	for (pid=0;pid<npartitions;pid++)
	{
		A = A_tunable->submatrix[pid].submat;
		hint_rec = &(A->structhints);
		switch (hint)
		{
			case HINT_NO_BLOCKS:
				oski_AllocStructHintBlocksizes (hint_rec, 0);
				break;

			case HINT_SINGLE_BLOCKSIZE:
				{
					if (r[0] != ARGS_NONE)	/* Process optional arguments, if any. */
					{
						if (r[0] < 1)
						{
							OSKI_ERR_BAD_HINT_ARG (oski_SetHint,
									1, HINT_SINGLE_BLOCKSIZE, r[0]);
							return ERR_BAD_HINT_ARG;
						}
						if (c[0] < 1)
						{
							OSKI_ERR_BAD_HINT_ARG (oski_SetHint,
									2, HINT_SINGLE_BLOCKSIZE, c[0]);
							return ERR_BAD_HINT_ARG;
						}

						if (oski_AllocStructHintBlocksizes (hint_rec, 1))
						{
							hint_rec->block_size.r_sizes[0] = r[0];
							hint_rec->block_size.c_sizes[0] = c[0];
						}
						else
						{
							oski_ResetStructHintBlocksizes (hint_rec);
							hint_rec->block_size.num_sizes = 1;
						}
					}
				}
				break;

			case HINT_MULTIPLE_BLOCKSIZES:
				{
					if (k == ARGS_NONE)
					{
						oski_ResetStructHintBlocksizes (hint_rec);
						hint_rec->block_size.num_sizes = 2;
					}
					else if (k >= 1)	/* caller specified a list */
					{
						/**
						 *  Attempt to allocate space for this list. If it
						 *  fails, we still record that there is a mix of
						 *  block sizes present, but do not record the
						 *  list. This behavior is OK because the implementation
						 *  is free to regard/disregard hints as desired.
						 */
						if (oski_AllocStructHintBlocksizes (hint_rec, k))
						{
							int i;
							int any_errors = 0;

							for (i = 0; i < k; i++)
							{
								if (r[i] < 1)
								{
									OSKI_ERR_BAD_HINT_ARG (oski_SetHint,
											2 + 2 * i,
											HINT_MULTIPLE_BLOCKSIZES, r[i]);
									hint_rec->block_size.c_sizes[i] = 0;
									any_errors++;
								}
								else
									hint_rec->block_size.r_sizes[i] = r[i];

								if (c[i] < 1)
								{
									OSKI_ERR_BAD_HINT_ARG (oski_SetHint,
											3 + 2 * i,
											HINT_MULTIPLE_BLOCKSIZES, c[i]);
									hint_rec->block_size.c_sizes[i] = 0;
									any_errors++;
								}
								else
									hint_rec->block_size.c_sizes[i] = c[i];
							}		/* for( i = ... ) */

							if (any_errors)
								return ERR_BAD_HINT_ARG;
						}
					}
					else			/* k < 0! */
					{
						OSKI_ERR_BAD_HINT_ARG (oski_SetHint,
								1, HINT_MULTIPLE_BLOCKSIZES, k);
						return ERR_BAD_HINT_ARG;
					}
				}
				break;

			case HINT_ALIGNED_BLOCKS:
				hint_rec->is_unaligned = 0;
				break;

			case HINT_UNALIGNED_BLOCKS:
				hint_rec->is_unaligned = 1;
				break;
			default:
				assert (0);		/* should never occur */
		}
	}
	return 0;

}

int poski_TuneHint_MatMult_run(poski_mat_t A_tunable, poski_operation_t op, poski_value_t alpha, poski_vec_t x_view, poski_value_t beta, poski_vec_t y_view, int num_calls)
{
	int err;
	poski_traceargs_MatMult_t args;

	int npartitions = A_tunable->partitionargs.npartitions;
	int pid;
	poski_matstruct_t A;
	poski_vecview_t x, y;

	for (pid=0;pid<npartitions;pid++)
	{
		A = A_tunable->submatrix[pid].submat;
		if (x_view == SYMBOLIC_VECTOR)
			x = SYMBOLIC_VEC;
		else
			x = x_view->subvector[pid].subvec;
		if (y_view == SYMBOLIC_VECTOR)
			y = SYMBOLIC_VEC;
		else
			y = y_view->subvector[pid].subvec;
	
		/* Error check input parameters */
		err = poski_CheckArgsMatMult (A, op, alpha, x, beta, y,	MACRO_TO_STRING (oski_SetHintMatMult));
		if (err)
			return err;
		/*
		 *  Record this hint. Although this call may fail, ignore it
		 *  since hints are strictly optional anyway.
		 */
		poski_MakeArglistMatMult (op, alpha, x, beta, y, &args);
		poski_RecordCalls (A->workhints, OSKI_KERNEL_MatMult, &args, sizeof(args), NULL, num_calls, TIME_NOT_AVAIL);
	}

	return 0;

}

