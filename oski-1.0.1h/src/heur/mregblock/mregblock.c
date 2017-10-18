/**
 *  \file src/heur/mregblock.c
 *  \brief Implementation of the register blocking SpMV heuristic.
 *  \ingroup TUNING
 *
 *  This heuristic and the MBCSR data structure is intended to
 *  replace BCSR and its heuristic eventually.
 */

#include <oski/heur/mregblock.h>

const char *
oski_GetShortDesc (void)
{
  return "Register blocking heuristic for MBCSR";
}

const char *
oski_GetLongDesc (void)
{
  return "Register blocking heuristic for MBCSR";
}

void
oski_InitModule (void)
{
  oski_PrintDebugMessage (2, "Initializing: %s", oski_GetShortDesc ());
  oski_LoadProfileMBCSRMatMult (NULL);
}

void
oski_CloseModule (void)
{
  oski_PrintDebugMessage (2, "Shutting down: %s", oski_GetShortDesc ());
  oski_UnloadProfileMBCSRMatMult ();
}

/**
 *  \brief Returns 1 if the execution time in a given trace is
 *  appears to be 'dominated' by matrix-vector multiply.
 *
 *  \param[in] trace A trace.
 *  \param[in] time_stream Estimated time (in seconds) to stream
 *  through the matrix at least once.
 *
 *  This implementation says matrix-vector multiply 'dominates'
 *  the trace time if it accounts for at least 50% of that time.
 *
 *  In addition, if no matrix vector multiplies appear in the
 *  trace, this routine returns 0.
 */
static int
IsTimeDominatedByMatMult (const oski_trace_t trace, double time_stream)
{
  double t_total = oski_EstimateTraceTimeTotal (trace, time_stream);
  double t_matmult = oski_EstimateTraceTime (trace, time_stream,
					     OSKI_KERNEL_MatMult, NULL, 0,
					     NULL) +
    oski_EstimateTraceTime (trace, time_stream,
			    OSKI_KERNEL_MatTransMatMult, NULL, 0,
			    NULL) + oski_EstimateTraceTime (trace,
							    time_stream,
							    OSKI_KERNEL_MatMultAndMatTransMult,
							    NULL, 0,
							    NULL) +
    oski_EstimateTraceTime (trace, time_stream,
			    OSKI_KERNEL_MatPowMult, NULL, 0, NULL);
  if (t_total > 0)
    return (t_matmult / t_total) >= 0.5;
  else
    return 0;
}

/**
 *  \brief Returns 1 if uniformly-aligned register blocked
 *  matrix-vector multiply should be considered for the given
 *  matrix.
 *
 *  This implementation says register blocking is 'applicable'
 *  (i.e., could be considered) for the given matrix if all of
 *  the following conditions hold:
 *    -# The matrix has a CSR or CSC representation.
 *
 *  AND at least one of the following conditions holds:
 *    -# The user explicitly provided a hint that the structure
 *       has blocks (\ref HINT_SINGLE_BLOCKSIZE,
 *       \ref HINT_MULTIPLE_BLOCKSIZES), and that such blocks
 *       are aligned (\ref HINT_ALIGNED_BLOCKS).
 *    -# The execution time is 'dominated' by sparse matrix-vector
 *       multiply. For the definnition of 'dominated', see
 *       \ref IsTimeDominatedByMatMult().
 */
int
oski_HeurIsApplicable (const oski_matrix_t A)
{
  oski_id_t id_CSR = INVALID_ID;
  oski_id_t id_CSC = INVALID_ID;

  if (A == INVALID_MAT)
    return 0;

  /* Check AND (mandatory) conditions */
  id_CSR = oski_LookupMatTypeId ("CSR", OSKI_IND_ID, OSKI_VAL_ID);
  id_CSC = oski_LookupMatTypeId ("CSC", OSKI_IND_ID, OSKI_VAL_ID);

  if (A->input_mat.type_id != id_CSR && A->input_mat.type_id != id_CSC)
    {
      /* No CSR or CSC representation exists already */
      oski_PrintDebugMessage (3, "Matrix not in a compatible format.");
      return 0;
    }

  /* Check OR conditions */
  oski_PrintDebugMessage (4, "User did%s specify explicit blocks",
			  (A->structhints.block_size.num_sizes >= 1
			   && !(A->structhints.is_unaligned)) ? "" : " not");
  oski_PrintDebugMessage (4, "Workload hints are%s dominated by SpMV",
			  IsTimeDominatedByMatMult (A->workhints,
						    A->
						    time_stream) ? "" :
			  " not");
  oski_PrintDebugMessage (4, "Trace is%s dominated by SpMV",
			  IsTimeDominatedByMatMult (A->trace,
						    A->
						    time_stream) ? "" :
			  " not");

  return
    (A->structhints.block_size.num_sizes >= 1
     && !(A->structhints.is_unaligned))
    || IsTimeDominatedByMatMult (A->workhints, A->time_stream)
    || IsTimeDominatedByMatMult (A->trace, A->time_stream);
}

/**
 *  \brief Returns an estimate of the cost of evaluating
 *  this heuristic.
 *
 *  \todo This implementation currently assumes our pessimistic
 *  upper-bound on the heuristic evaluation cost of 40 x (the time
 *  to stream through the matrix), and should be changed to use a
 *  build-time benchmark.
 */
double
oski_HeurEstimateCost (const oski_matrix_t A)
{
  return 40 * A->time_stream;
}

/* eof */
