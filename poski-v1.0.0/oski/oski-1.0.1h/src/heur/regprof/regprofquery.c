/**
 *  \brief Query routines needed by register profile heuristic.
 *  \file src/heur/regprofquery.c
 */

#include <stdarg.h>

#include <oski/common.h>
#include <oski/mangle.h>
#include <oski/simplelist.h>
#include <oski/matrix.h>
#include <oski/heur/regprofmgr.h>
#include <oski/heur/regprofquery.h>

/* -------------------------------------------------------------------
 *  Comparison functions for trace queries
 */

/** \name Arguments for transpose matching in trace queries */
/*@{*/
static const oski_traceargs_MatMult_t g_MatMult_OP_NORMAL =
  { OP_NORMAL, 0, 0, LAYOUT_COLMAJ, 0, LAYOUT_COLMAJ };
static const oski_traceargs_MatMult_t g_MatMult_OP_CONJ =
  { OP_CONJ, 0, 0, LAYOUT_COLMAJ, 0, LAYOUT_COLMAJ };
static const oski_traceargs_MatMult_t g_MatMult_OP_TRANS =
  { OP_TRANS, 0, 0, LAYOUT_COLMAJ, 0, LAYOUT_COLMAJ };
static const oski_traceargs_MatMult_t g_MatMult_OP_CONJ_TRANS =
  { OP_CONJ_TRANS, 0, 0, LAYOUT_COLMAJ, 0, LAYOUT_COLMAJ };

static const oski_traceargs_MatTrisolve_t g_MatTrisolve_OP_NORMAL =
  { OP_NORMAL, 0, 0, LAYOUT_COLMAJ };
static const oski_traceargs_MatTrisolve_t g_MatTrisolve_OP_CONJ =
  { OP_CONJ, 0, 0, LAYOUT_COLMAJ };
static const oski_traceargs_MatTrisolve_t g_MatTrisolve_OP_TRANS =
  { OP_TRANS, 0, 0, LAYOUT_COLMAJ };
static const oski_traceargs_MatTrisolve_t g_MatTrisolve_OP_CONJ_TRANS =
  { OP_CONJ_TRANS, 0, 0, LAYOUT_COLMAJ };

static const oski_traceargs_MatTransMatMult_t g_MatTransMatMult_OP_AT_A =
  { OP_AT_A, 0, 0, LAYOUT_COLMAJ, 0, LAYOUT_COLMAJ, LAYOUT_COLMAJ };
static const oski_traceargs_MatTransMatMult_t g_MatTransMatMult_OP_AH_A =
  { OP_AH_A, 0, 0, LAYOUT_COLMAJ, 0, LAYOUT_COLMAJ, LAYOUT_COLMAJ };
static const oski_traceargs_MatTransMatMult_t g_MatTransMatMult_OP_A_AT =
  { OP_A_AT, 0, 0, LAYOUT_COLMAJ, 0, LAYOUT_COLMAJ, LAYOUT_COLMAJ };
static const oski_traceargs_MatTransMatMult_t g_MatTransMatMult_OP_A_AH =
  { OP_A_AH, 0, 0, LAYOUT_COLMAJ, 0, LAYOUT_COLMAJ, LAYOUT_COLMAJ };

static const oski_traceargs_MatMultAndMatTransMult_t
  g_MatMultAndMatTransMult_OP_NORMAL =
  { MAKE_VAL_COMPLEX (0, 0), 0, LAYOUT_COLMAJ,
  MAKE_VAL_COMPLEX (0, 0), LAYOUT_COLMAJ,
  OP_NORMAL, MAKE_VAL_COMPLEX (0, 0), LAYOUT_COLMAJ,
  MAKE_VAL_COMPLEX (0, 0), LAYOUT_COLMAJ
};
static const oski_traceargs_MatMultAndMatTransMult_t
  g_MatMultAndMatTransMult_OP_CONJ =
  { MAKE_VAL_COMPLEX (0, 0), 0, LAYOUT_COLMAJ,
  MAKE_VAL_COMPLEX (0, 0), LAYOUT_COLMAJ,
  OP_CONJ, MAKE_VAL_COMPLEX (0, 0), LAYOUT_COLMAJ,
  MAKE_VAL_COMPLEX (0, 0), LAYOUT_COLMAJ
};
static const oski_traceargs_MatMultAndMatTransMult_t
  g_MatMultAndMatTransMult_OP_TRANS =
  { MAKE_VAL_COMPLEX (0, 0), 0, LAYOUT_COLMAJ,
  MAKE_VAL_COMPLEX (0, 0), LAYOUT_COLMAJ,
  OP_TRANS, MAKE_VAL_COMPLEX (0, 0), LAYOUT_COLMAJ,
  MAKE_VAL_COMPLEX (0, 0), LAYOUT_COLMAJ
};
static const oski_traceargs_MatMultAndMatTransMult_t
  g_MatMultAndMatTransMult_OP_CONJ_TRANS =
  { MAKE_VAL_COMPLEX (0, 0), 0, LAYOUT_COLMAJ,
  MAKE_VAL_COMPLEX (0, 0), LAYOUT_COLMAJ,
  OP_CONJ_TRANS, MAKE_VAL_COMPLEX (0, 0), LAYOUT_COLMAJ,
  MAKE_VAL_COMPLEX (0, 0), LAYOUT_COLMAJ
};

static const oski_traceargs_MatPowMult_t g_MatPowMult_OP_NORMAL =
  { OP_NORMAL, 0, 0, MAKE_VAL_COMPLEX (0, 0), LAYOUT_COLMAJ, MAKE_VAL_COMPLEX (0, 0),
LAYOUT_COLMAJ, LAYOUT_COLMAJ };
static const oski_traceargs_MatPowMult_t g_MatPowMult_OP_CONJ =
  { OP_CONJ, 0, 0, MAKE_VAL_COMPLEX (0, 0), LAYOUT_COLMAJ, MAKE_VAL_COMPLEX (0, 0),
LAYOUT_COLMAJ, LAYOUT_COLMAJ };
static const oski_traceargs_MatPowMult_t g_MatPowMult_OP_TRANS =
  { OP_TRANS, 0, 0, MAKE_VAL_COMPLEX (0, 0), LAYOUT_COLMAJ, MAKE_VAL_COMPLEX (0, 0),
LAYOUT_COLMAJ, LAYOUT_COLMAJ };
static const oski_traceargs_MatPowMult_t g_MatPowMult_OP_CONJ_TRANS =
  { OP_CONJ_TRANS, 0, 0, MAKE_VAL_COMPLEX (0, 0), LAYOUT_COLMAJ,
MAKE_VAL_COMPLEX (0, 0), LAYOUT_COLMAJ, LAYOUT_COLMAJ };
/*@}*/

/** Retrieves a field of a given name from an argument signature
  * of a given base type. */
#define GET_ARGS(args, typ, field) \
	    ((const oski_traceargs_##typ##_t *)(args))-> field

/** Matches transpose option for SpMV */
static int
MatchesMatMultOp (const void *a, const void *b)
{
  oski_matop_t opA = GET_ARGS (a, MatMult, opA);
  oski_matop_t opB = GET_ARGS (b, MatMult, opA);
  return opA == opB;
}

/** Matches transpose option for triangular solve */
static int
MatchesMatTrisolveOp (const void *a, const void *b)
{
  oski_matop_t opA = GET_ARGS (a, MatTrisolve, opT);
  oski_matop_t opB = GET_ARGS (b, MatTrisolve, opT);
  return opA == opB;
}

/** Matches transpose option for \f$A^TA\cdot x\f$ kernel */
static int
MatchesMatTransMatMultOp (const void *a, const void *b)
{
  oski_ataop_t opA = GET_ARGS (a, MatTransMatMult, op);
  oski_ataop_t opB = GET_ARGS (b, MatTransMatMult, op);
  return opA == opB;
}

/** Matches transpose option for simultaneous \f$A\cdot x, \f$A^T\cdot w\f$ */
static int
MatchesMatMultAndMatTransMultOp (const void *a, const void *b)
{
  oski_matop_t opA = GET_ARGS (a, MatMultAndMatTransMult, opA);
  oski_matop_t opB = GET_ARGS (a, MatMultAndMatTransMult, opA);
  return opA == opB;
}

/** Matches transpose option for \f$A^k\cdot x\f$ kernel, with \f$k > 2\f$ */
static int
MatchesMatPowMultOp (const void *a, const void *b)
{
  oski_matop_t opA = GET_ARGS (a, MatPowMult, opA);
  oski_matop_t opB = GET_ARGS (b, MatPowMult, opA);
  return opA == opB;
}

/** \name Register profile file names */
/*@{*/
#define PROF_MatMult "profile_MBCSR_MatMult_T" \
	MACRO_TO_STRING(IND_TAG) MACRO_TO_STRING(VAL_TAG) ".dat"
#define PROF_MatTransMult "profile_MBCSR_MatTransMult_T" \
	MACRO_TO_STRING(IND_TAG) MACRO_TO_STRING(VAL_TAG) ".dat"

#define PROF_SymmMatMult "profile_MBCSR_SymmMatMult_T" \
	MACRO_TO_STRING(IND_TAG) MACRO_TO_STRING(VAL_TAG) ".dat"

#define PROF_LowerMatTrisolve "profile_MBCSR_LowerMatTrisolve_T" \
	MACRO_TO_STRING(IND_TAG) MACRO_TO_STRING(VAL_TAG) ".dat"
#define PROF_UpperMatTrisolve "profile_MBCSR_UpperMatTrisolve_T" \
	MACRO_TO_STRING(IND_TAG) MACRO_TO_STRING(VAL_TAG) ".dat"
#define PROF_LowerMatTransTrisolve "profile_MBCSR_LowerMatTransTrisolve_T" \
	MACRO_TO_STRING(IND_TAG) MACRO_TO_STRING(VAL_TAG) ".dat"
#define PROF_UpperMatTransTrisolve "profile_MBCSR_UpperMatTransTrisolve_T" \
	MACRO_TO_STRING(IND_TAG) MACRO_TO_STRING(VAL_TAG) ".dat"

#define PROF_MatTransMatMult "profile_MBCSR_MatTransMatMult_T" \
	MACRO_TO_STRING(IND_TAG) MACRO_TO_STRING(VAL_TAG) ".dat"
#define PROF_MatTransMatTransMult "profile_MBCSR_MatTransMatTransMult_T" \
	MACRO_TO_STRING(IND_TAG) MACRO_TO_STRING(VAL_TAG) ".dat"

#define PROF_MatMultAndMatMult "profile_MBCSR_MatMultAndMatMult_T" \
	MACRO_TO_STRING(IND_TAG) MACRO_TO_STRING(VAL_TAG) ".dat"
#define PROF_MatMultAndMatTransMult "profile_MBCSR_MatMultAndMatTransMult_T" \
	MACRO_TO_STRING(IND_TAG) MACRO_TO_STRING(VAL_TAG) ".dat"

#define PROF_MatPowMult "profile_MBCSR_MatPowMult_T" \
	MACRO_TO_STRING(IND_TAG) MACRO_TO_STRING(VAL_TAG) ".dat"
#define PROF_MatTransPowMult "profile_MBCSR_MatTransPowMult_T" \
	MACRO_TO_STRING(IND_TAG) MACRO_TO_STRING(VAL_TAG) ".dat"
/*@}*/

static oski_regprofkernel_t *
GetWorkloadKernel (const oski_trace_t W, oski_id_t kernel_id,
		   const char *filename, oski_traceargscmp_funcpt fcmp,
		   size_t bytes, int num_args, ...)
{
  oski_regprofkernel_t *kernel = oski_Malloc (oski_regprofkernel_t, 1);
  int has_profile;
  int i;
  va_list ap;

  if (kernel == NULL)
    return NULL;

  kernel->id = kernel_id;

  has_profile = oski_LoadRegProfile (filename, &(kernel->profile));
  if (!has_profile)
    {
      oski_Free (kernel);
      return NULL;
    }

  kernel->num_flops = 0.0;
  va_start (ap, num_args);
  for (i = 0; i < num_args; i++)
    {
      const void *args = va_arg (ap, const void *);
      kernel->num_flops +=
	oski_CountTraceFlopsPerNz (W, kernel_id, args, bytes, fcmp);
    }
  va_end (ap);

  kernel->has_alt_profile = 0;
  return kernel;
}

static oski_regprofkernel_t *
GetWorkload_MatMult (const oski_trace_t W)
{
  oski_regprofkernel_t *kernel = GetWorkloadKernel (W,
						    OSKI_KERNEL_MatMult,
						    PROF_MatMult,
						    MatchesMatMultOp,
						    sizeof
						    (oski_traceargs_MatMult_t),
						    2,
						    &g_MatMult_OP_NORMAL,
						    &g_MatMult_OP_CONJ);

  if (kernel == NULL)
    return NULL;

  /*  The following kernels are not implemented specially
   *  in MBCSR format, so treat them as SpMV operations
   */
  kernel->num_flops += 0.5 * oski_CountTraceFlopsPerNz (W,
							OSKI_KERNEL_MatTransMatMult,
							&g_MatTransMatMult_OP_A_AT,
							sizeof
							(g_MatTransMatMult_OP_A_AT),
							MatchesMatMultAndMatTransMultOp);
  kernel->num_flops +=
    0.5 * oski_CountTraceFlopsPerNz (W, OSKI_KERNEL_MatTransMatMult,
				     &g_MatTransMatMult_OP_A_AH,
				     sizeof (g_MatTransMatMult_OP_A_AH),
				     MatchesMatMultAndMatTransMultOp);
  kernel->num_flops +=
    oski_CountTraceFlopsPerNz (W, OSKI_KERNEL_MatPowMult,
			       &g_MatPowMult_OP_NORMAL,
			       sizeof (g_MatPowMult_OP_NORMAL),
			       MatchesMatPowMultOp);
  kernel->num_flops +=
    oski_CountTraceFlopsPerNz (W, OSKI_KERNEL_MatPowMult,
			       &g_MatPowMult_OP_CONJ,
			       sizeof (g_MatPowMult_OP_CONJ),
			       MatchesMatPowMultOp);

  kernel->op.mat = OP_NORMAL;
  return kernel;
}

static oski_regprofkernel_t *
GetWorkload_MatTransMult (const oski_trace_t W)
{
  oski_regprofkernel_t *kernel = GetWorkloadKernel (W,
						    OSKI_KERNEL_MatMult,
						    PROF_MatTransMult,
						    MatchesMatMultOp,
						    sizeof
						    (oski_traceargs_MatMult_t),
						    2,
						    &g_MatMult_OP_TRANS,
						    &g_MatMult_OP_CONJ_TRANS);

  if (kernel == NULL)
    return NULL;

  /*  The following kernels are not implemented specially
   *  in MBCSR format, so treat them as SpMV operations
   */
  kernel->num_flops += 0.5 * oski_CountTraceFlopsPerNz (W,
							OSKI_KERNEL_MatTransMatMult,
							&g_MatTransMatMult_OP_A_AT,
							sizeof
							(g_MatTransMatMult_OP_A_AT),
							MatchesMatMultAndMatTransMultOp);
  kernel->num_flops +=
    0.5 * oski_CountTraceFlopsPerNz (W, OSKI_KERNEL_MatTransMatMult,
				     &g_MatTransMatMult_OP_A_AH,
				     sizeof (g_MatTransMatMult_OP_A_AH),
				     MatchesMatMultAndMatTransMultOp);
  kernel->num_flops +=
    oski_CountTraceFlopsPerNz (W, OSKI_KERNEL_MatPowMult,
			       &g_MatPowMult_OP_TRANS,
			       sizeof (g_MatPowMult_OP_TRANS),
			       MatchesMatPowMultOp);
  kernel->num_flops +=
    oski_CountTraceFlopsPerNz (W, OSKI_KERNEL_MatPowMult,
			       &g_MatPowMult_OP_CONJ_TRANS,
			       sizeof (g_MatPowMult_OP_CONJ_TRANS),
			       MatchesMatPowMultOp);

  kernel->op.mat = OP_TRANS;
  return kernel;
}

static oski_regprofkernel_t *
GetWorkload_MatTrisolve (int is_lower, const oski_trace_t W)
{
  oski_regprofkernel_t *kernel = NULL;
  if (is_lower)
    kernel = GetWorkloadKernel (W, OSKI_KERNEL_MatTrisolve,
				PROF_LowerMatTrisolve, MatchesMatTrisolveOp,
				sizeof (oski_traceargs_MatTrisolve_t), 2,
				&g_MatTrisolve_OP_NORMAL,
				&g_MatTrisolve_OP_CONJ);
  else				/* !is_lower */
    kernel = GetWorkloadKernel (W, OSKI_KERNEL_MatTrisolve,
				PROF_LowerMatTrisolve, MatchesMatTrisolveOp,
				sizeof (oski_traceargs_MatTrisolve_t), 2,
				&g_MatTrisolve_OP_NORMAL,
				&g_MatTrisolve_OP_CONJ);

  if (kernel != NULL)
    kernel->op.mat = OP_NORMAL;
  return kernel;
}

static oski_regprofkernel_t *
GetWorkload_MatTransTrisolve (int is_lower, const oski_trace_t W)
{
  oski_regprofkernel_t *kernel = NULL;
  if (is_lower)
    kernel = GetWorkloadKernel (W, OSKI_KERNEL_MatTrisolve,
				PROF_LowerMatTransTrisolve,
				MatchesMatTrisolveOp,
				sizeof (oski_traceargs_MatTrisolve_t), 2,
				&g_MatTrisolve_OP_TRANS,
				&g_MatTrisolve_OP_CONJ_TRANS);
  else				/* !is_lower */
    kernel = GetWorkloadKernel (W, OSKI_KERNEL_MatTrisolve,
				PROF_LowerMatTransTrisolve,
				MatchesMatTrisolveOp,
				sizeof (oski_traceargs_MatTrisolve_t), 2,
				&g_MatTrisolve_OP_TRANS,
				&g_MatTrisolve_OP_CONJ_TRANS);
  if (kernel != NULL)
    kernel->op.mat = OP_TRANS;
  return kernel;
}

static int
LoadProfile_MatMultAndMatTransMult (oski_regprof_t * profile)
{
  if (profile == NULL)
    return 0;
  if (oski_LoadRegProfile (PROF_MatMult, profile))
    {
      oski_regprof_t profile_MatTransMult;
      if (oski_LoadRegProfile (PROF_MatTransMult, &profile_MatTransMult))
	{
	  oski_CalcHarmonicMeanRegProfile (profile, 0.5,
					   &profile_MatTransMult, 0.5);
	  return 1;
	}
      /* Failed to load transpose profile */
      oski_ResetRegProfile (profile);
    }
  /* Failed to load A*x and/or A^T*x profile */
  return 0;
}

static oski_regprofkernel_t *
GetWorkload_MatTransMatMult (const oski_trace_t W)
{
  oski_regprofkernel_t *kernel =
    GetWorkloadKernel (W, OSKI_KERNEL_MatTransMatMult,
		       PROF_MatTransMatMult, MatchesMatTransMatMultOp,
		       sizeof (oski_traceargs_MatTransMatMult_t), 2,
		       &g_MatTransMatMult_OP_AT_A,
		       &g_MatTransMatMult_OP_AH_A);

  if (kernel == NULL)
    return NULL;

  kernel->has_alt_profile =
    LoadProfile_MatMultAndMatTransMult (&(kernel->alt_profile));
  kernel->op.ata = OP_AT_A;
  return kernel;
}

static oski_regprofkernel_t *
GetWorkload_MatMultAndMatMult (const oski_trace_t W)
{
  oski_regprofkernel_t *kernel =
    GetWorkloadKernel (W, OSKI_KERNEL_MatMultAndMatTransMult,
		       PROF_MatMultAndMatMult,
		       MatchesMatMultAndMatTransMultOp,
		       sizeof (oski_traceargs_MatMultAndMatTransMult_t), 2,
		       &g_MatMultAndMatTransMult_OP_NORMAL,
		       &g_MatMultAndMatTransMult_OP_CONJ);

  if (kernel == NULL)
    return NULL;

  if (oski_LoadRegProfile (PROF_MatMult, &(kernel->alt_profile)))
    kernel->has_alt_profile = 1;
  kernel->op.mat = OP_NORMAL;
  return kernel;
}

static oski_regprofkernel_t *
GetWorkload_MatMultAndMatTransMult (const oski_trace_t W)
{
  oski_regprofkernel_t *kernel =
    GetWorkloadKernel (W, OSKI_KERNEL_MatMultAndMatTransMult,
		       PROF_MatMultAndMatTransMult,
		       MatchesMatMultAndMatTransMultOp,
		       sizeof (oski_traceargs_MatMultAndMatTransMult_t), 2,
		       &g_MatMultAndMatTransMult_OP_TRANS,
		       &g_MatMultAndMatTransMult_OP_CONJ_TRANS);

  if (kernel == NULL)
    return NULL;

  kernel->has_alt_profile =
    LoadProfile_MatMultAndMatTransMult (&(kernel->alt_profile));
  kernel->op.mat = OP_TRANS;
  return kernel;
}

/** Compute normalized flop counts */
static void
NormalizeFlops (simplelist_t * workload)
{
  size_t i;
  double tot_flops = 0.0;
  for (i = 1; i <= simplelist_GetLength (workload); i++)
    {
      const oski_regprofkernel_t *kernel = (const oski_regprofkernel_t *)
	simplelist_GetElem (workload, i);
      if (kernel != NULL)
	tot_flops += kernel->num_flops;
    }
  for (i = 1; i <= simplelist_GetLength (workload); i++)
    {
      oski_regprofkernel_t *kernel = (oski_regprofkernel_t *)
	simplelist_GetElem (workload, i);
      if (kernel != NULL)
	kernel->norm_flops = kernel->num_flops / tot_flops;
    }
}

/* ---------------------------------------------------------
 *  Public interface
 */

simplelist_t *
oski_GetRegProfileKernelWorkload (const oski_matcommon_t * A,
				  const oski_trace_t W)
{
  simplelist_t *kernels = simplelist_Create ();
  int is_lower = (A != NULL && A->pattern.is_tri_lower);
  oski_regprofkernel_t *kernel;

  kernel = GetWorkload_MatMult (W);
  if (kernel != NULL)
    simplelist_Append (kernels, kernel);
  kernel = GetWorkload_MatTransMult (W);
  if (kernel != NULL)
    simplelist_Append (kernels, kernel);
  kernel = GetWorkload_MatTrisolve (is_lower, W);
  if (kernel != NULL)
    simplelist_Append (kernels, kernel);
  kernel = GetWorkload_MatTransTrisolve (is_lower, W);
  if (kernel != NULL)
    simplelist_Append (kernels, kernel);
  kernel = GetWorkload_MatTransMatMult (W);
  if (kernel != NULL)
    simplelist_Append (kernels, kernel);
  kernel = GetWorkload_MatMultAndMatMult (W);
  if (kernel != NULL)
    simplelist_Append (kernels, kernel);
  kernel = GetWorkload_MatMultAndMatTransMult (W);
  if (kernel != NULL)
    simplelist_Append (kernels, kernel);

  NormalizeFlops (kernels);
  return kernels;
}

void
oski_DestroyRegProfileKernelWorkload (simplelist_t * L)
{
  size_t i;
  for (i = 1; i <= simplelist_GetLength (L); i++)
    {
      oski_regprofkernel_t *kernel = (oski_regprofkernel_t *)
	simplelist_GetElem (L, i);
      if (kernel != NULL)
	{
	  oski_ResetRegProfile (&(kernel->profile));
	  if (kernel->has_alt_profile)
	    oski_ResetRegProfile (&(kernel->alt_profile));
	  oski_Free (kernel);
	}
    }
  simplelist_Destroy (L);
}

/* eof */
