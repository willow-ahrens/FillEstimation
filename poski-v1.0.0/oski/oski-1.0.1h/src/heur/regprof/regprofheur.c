/**
 *  \file src/heur/regprofheur.c
 *  \brief Register blocking heuristic for general workloads.
 */

#include <oski/heur/regprofheur.h>
#include <oski/CSC/format.h>
#include <oski/MBCSR/format.h>
#include <oski/heur/estfill.h>
#include <oski/xforms.h>
#include <oski/tune.h>

#define MAX_R 8	 /**< Max row block size to consider */
#define MAX_C 8	 /**< Max column block size to consider */
#define HEUR_SWEEP_FRAC .02 /**< Fraction of matrix to scan for fill est. */

/* --------------------------------------------------------------
 *  Implementation
 */

#include <oski/config.h>
#include <math.h>

#if HAVE_C99_FPCLASSIFY
	/** Returns 1 <==> x == 0.0 */
#define FPTEST_IS_ZERO(x)  (fpclassify(x) == FP_ZERO)
#else /* !HAVE_C99_FPCLASSIFY */
	/** Returns 1 <==> x == 0.0 */
#define FPTEST_IS_ZERO(x)  (x == 0.0)
#endif

/**
 *  \brief Evaluate 1 term (kernel) of heuristic
 *  \param[in] cur_sum  Current sum.
 *  \param[in] profile  Profile of the kernel to evaluate.
 *  \param[in] weight  Weight of this profile.
 *  \param[in,out] new_sum A previously initialized register profile
 *  in which to store the new sum.
 */
static void
EvalTerm (const oski_regprof_t * cur_sum,
	  const oski_regprof_t * profile, double weight,
	  oski_regprof_t * new_sum)
{
  if (FPTEST_IS_ZERO (weight))
    {
      oski_CopyRegProfile (cur_sum, new_sum);
    }
  else
    {
      oski_CopyRegProfile (profile, new_sum);
      oski_InvRegProfile (new_sum);
      oski_ScaleRegProfile (new_sum, weight);
      oski_AddEqRegProfile (new_sum, cur_sum);
    }
}

static void
EvalHeur (const simplelist_t * W, size_t k, const oski_regprof_t * fill,
	  const oski_regprof_t * cur_sum, oski_regprofheur_t * results);

static void
EvalHeurAlt (const simplelist_t * W, size_t k, const oski_regprof_t * fill,
	     const oski_regprof_t * cur_sum, oski_regprofheur_t * results)
{
  oski_regprofheur_t res_alt;
  oski_regprof_t sum_alt;
  const oski_regprofkernel_t *kernel =
    (const oski_regprofkernel_t *) simplelist_GetElem (W, k);

  if (kernel == NULL || !kernel->has_alt_profile)
    return;

  oski_InitRegProfile (&sum_alt, cur_sum->max_r, cur_sum->max_c);
  EvalTerm (cur_sum, &(kernel->alt_profile), kernel->norm_flops, &sum_alt);
  EvalHeur (W, k + 1, fill, &sum_alt, &res_alt);
  oski_ResetRegProfile (&sum_alt);

  if (results->perf_est < res_alt.perf_est)
    {
      /* Replace results with alternate results */
      switch (kernel->id)
	{
	case OSKI_KERNEL_MatTransMatMult:
	  results->enabled.MatTransMatMult = 0;
	  break;
	case OSKI_KERNEL_MatMultAndMatTransMult:
	  if (kernel->op.mat == OP_NORMAL || kernel->op.mat == OP_CONJ)
	    results->enabled.MatMultAndMatMult = 0;
	  else
	    results->enabled.MatMultAndMatTransMult = 0;
	  break;
	default:
	  break;
	}
      results->r = res_alt.r;
      results->c = res_alt.c;
      results->perf_est = res_alt.perf_est;
    }
}

/** Evaluate the heuristic, given a workload and fill ratios. */
static void
EvalHeur (const simplelist_t * W, size_t k, const oski_regprof_t * fill,
	  const oski_regprof_t * cur_sum, oski_regprofheur_t * results)
{
  oski_regprof_t sum;		/* Sum of all \f$f(k) / P(k,r,c)\f$ terms */

  oski_InitRegProfile (&sum, cur_sum->max_r, cur_sum->max_c);

  if (k <= simplelist_GetLength (W))
    {
      const oski_regprofkernel_t *kernel =
	(const oski_regprofkernel_t *) simplelist_GetElem (W, k);

      EvalTerm (cur_sum, &(kernel->profile), kernel->norm_flops, &sum);
      EvalHeur (W, k + 1, fill, &sum, results);
      EvalHeurAlt (W, k, fill, cur_sum, results);
    }
  else /* All terms counted; minimize */
    {
      double min_time_est;	/* Minimum estimation of time per nz */
      oski_index_t r_heur, c_heur;	/* Block size achieving min time */

      oski_CopyRegProfile (cur_sum, &sum);
      oski_MulEqRegProfile (&sum, fill);

      oski_PrintDebugRegProfile (3, "Simulated time to minimize", &sum);

      min_time_est = oski_MinRegProfile (&sum, &r_heur, &c_heur);

      results->r = r_heur;
      results->c = c_heur;
      results->perf_est = 1.0 / min_time_est;
    }

  /* Memory clean-up */
  oski_ResetRegProfile (&sum);
}

/** Estimates the fill and returns 1 on success, 0 on error. */
static int
EstFill (const oski_matrix_t A, oski_regprof_t * fill)
{
  oski_fillprofile_BCSR_t *fill_raw =
    oski_EstimateFillBCSR (&(A->input_mat), &(A->props), MAX_R, MAX_C,
			   HEUR_SWEEP_FRAC);
  oski_index_t r;

  if (fill_raw == NULL)
    return 0;

  if (!oski_InitRegProfile (fill, fill_raw->max_r, fill_raw->max_c))
    return 0;

  /* Convert profile to different format */
  for (r = 1; r <= MAX_R; r++)
    {
      oski_index_t c;
      for (c = 1; c <= MAX_C; c++)
	{
	  REGPROF_SET (fill, r, c, PROF_FILLBCSR_GET (fill_raw, r, c));
	}
    }
  oski_DestroyBCSRFillProfile (fill_raw);
  return 1;
}

static oski_regprofheur_t *
CreateResults (void)
{
  oski_regprofheur_t *results = oski_Malloc (oski_regprofheur_t, 1);
  if (results == NULL)
    return NULL;
  results->r = 1;
  results->c = 1;
  results->perf_est = 0;
  results->enabled.MatTransMatMult = 1;
  results->enabled.MatMultAndMatMult = 1;
  results->enabled.MatMultAndMatTransMult = 1;
  results->enabled.MatPowMult = 0;
  results->enabled.MatTransPowMult = 0;
  return results;
}

static simplelist_t *
GetWorkload (const oski_matrix_t A)
{
  double t_hint = oski_EstimateTraceTimeTotal (A->workhints, A->time_stream);
  double t_obs = oski_EstimateTraceTimeTotal (A->trace, A->time_stream);

  return oski_GetRegProfileKernelWorkload (&(A->props),
					   t_hint >
					   t_obs ? A->workhints : A->trace);
}

static char *
MakeTransformString (const char *input_mat, int r, int c)
{
  return oski_StringPrintf ("return MBCSR(%s, %d, %d)", input_mat, r, c);
}

/* --------------------------------------------------------------
 *  Public interface
 */

const char *
oski_GetShortDesc (void)
{
  return "Register blocking heuristic for MBCSR and general workloads";
}

const char *
oski_GetLongDesc (void)
{
  return oski_GetShortDesc ();
}

void
oski_InitModule (void)
{
  oski_PrintDebugMessage (2, "Initializing: %s", oski_GetShortDesc ());
}

void
oski_CloseModule (void)
{
  oski_PrintDebugMessage (2, "Shutting down: %s", oski_GetShortDesc ());
}

double
oski_HeurEstimateCost (const oski_matrix_t A)
{
  return 40 * A->time_stream;
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
 *    -# The matrix uses non-symmetric storage.
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
      oski_PrintDebugMessage (3, "Matrix not in compatible format.");
      return 0;
    }

  if (A->props.pattern.is_symm || A->props.pattern.is_herm)
    {
      int applies = 1;
      if (A->input_mat.type_id == id_CSR)
	{
	  const oski_matCSR_t *mat =
	    (const oski_matCSR_t *) A->input_mat.repr;
	  applies = mat->stored.is_upper == mat->stored.is_lower;
	}
      else if (A->input_mat.type_id == id_CSC)
	{
	  const oski_matCSC_t *mat =
	    (const oski_matCSC_t *) A->input_mat.repr;
	  const oski_matCSR_t *matT = mat->mat_trans;
	  applies = matT->stored.is_upper == matT->stored.is_lower;
	}
      if (!applies)
	{
	  oski_PrintDebugMessage (3, "Matrix uses symmetric storage.");
	  return 0;
	}
    }

  /* Check OR conditions */
  oski_PrintDebugMessage (4, "User did%s specify explicit blocks",
			  (A->structhints.block_size.num_sizes >= 1
			   && !(A->structhints.is_unaligned)) ? "" : " not");

  return 1;
}

int
oski_HeurApplyResults (const void *results, oski_matrix_t A)
{
  const oski_regprofheur_t *block_size = (const oski_regprofheur_t *) results;
  char *xform;
  int err;

  if (results == NULL)
    return ERR_BAD_ARG;
  xform = MakeTransformString (OLUA_INMAT, block_size->r, block_size->c);
  if (xform == NULL)
    return ERR_OUT_OF_MEMORY;

  oski_PrintDebugMessage (4, "Transform: '%s'", xform);
  err = oski_ApplyMatTransforms (A, xform);
  oski_PrintDebugMessage (4, "Result: %d", err);
  oski_Free (xform);

  /* Selectively disable individual kernel implementations as needed */
  if (!err && A->tuned_mat.repr != NULL)
    {
      oski_matMBCSR_t *mat = (oski_matMBCSR_t *) A->tuned_mat.repr;
      mat->enabled.MatTransMatMult = block_size->enabled.MatTransMatMult;
      mat->enabled.MatMultAndMatMult = block_size->enabled.MatMultAndMatMult;
      mat->enabled.MatMultAndMatTransMult =
	block_size->enabled.MatMultAndMatTransMult;
      mat->enabled.MatPowMult = block_size->enabled.MatPowMult;
      mat->enabled.MatTransPowMult = block_size->enabled.MatTransPowMult;
    }

  return err;
}

void *
oski_HeurEvaluate (const oski_matrix_t A)
{
  oski_regprofheur_t *results;
  oski_regprof_t fill;
  simplelist_t *workload;
  oski_regprof_t cur_sum;

  results = CreateResults ();
  if (results == NULL)
    return NULL;

  if (!EstFill (A, &fill))
    {
      oski_Free (results);
      return NULL;
    }

  oski_PrintDebugRegProfile (3, "Fill ratio", &fill);

  workload = GetWorkload (A);
  if (workload == NULL)
    {
      oski_ResetRegProfile (&fill);
      oski_Free (results);
      return NULL;
    }

  if (!oski_InitRegProfile (&cur_sum, MAX_R, MAX_C))
    {
      oski_DestroyRegProfileKernelWorkload (workload);
      oski_ResetRegProfile (&fill);
      oski_Free (results);
      return NULL;
    }

  EvalHeur (workload, 1, &fill, &cur_sum, results);

  oski_ResetRegProfile (&cur_sum);
  oski_DestroyRegProfileKernelWorkload (workload);
  oski_ResetRegProfile (&fill);

  oski_PrintDebugMessage (3, "Results of heuristic evaluation:");
  oski_PrintDebugMessage (4, "Block size: %d x %d",
			  (int) results->r, (int) results->c);
  oski_PrintDebugMessage (4, "Estimated performance: %g Mflop/s",
			  results->perf_est);

  oski_PrintDebugMessage (4, "%sabled A^T*A*x",
			  results->enabled.MatTransMatMult ? "En" : "Dis");
  oski_PrintDebugMessage (4, "%sabled A*x, A*w",
			  results->enabled.MatMultAndMatMult ? "En" : "Dis");
  oski_PrintDebugMessage (4, "%sabled A*x, A^T*w",
			  results->enabled.
			  MatMultAndMatTransMult ? "En" : "Dis");
  oski_PrintDebugMessage (4, "%sabled A^k*x",
			  results->enabled.MatPowMult ? "En" : "Dis");
  oski_PrintDebugMessage (4, "%sabled (A^T)^k*x",
			  results->enabled.MatTransPowMult ? "En" : "Dis");
  return results;
}

/* eof */
