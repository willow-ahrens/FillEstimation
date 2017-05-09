/**
 *  \file src/trace_query.c
 *  \ingroup TRACING
 *  \brief Implements a database system for tracking kernel calls.
 */

#include <string.h>

#include <oski/config.h>

#include <oski/trace.h>
#include <oski/matrix.h>
#include <oski/matmult.h>
#include <oski/a_and_at.h>
#include <oski/ata.h>
#include <oski/trisolve.h>
#include <oski/matpow.h>

/** Retrieves a field of a given name from an argument signature
* of a given base type. */
#define GET_ARGS(args, typ, field) \
	((const oski_traceargs_##typ##_t *)(args))-> field

double
oski_EstimateTraceTimeTotal (const oski_trace_t trace, double time_stream)
{
  return oski_EstimateTraceTime (trace, time_stream, INVALID_ID,
				 NULL, 0, NULL);
}

/**
 *  \brief
 *
 *  This implementation allows arbitrary queries on a trace
 *  through a user-defined comparison function of type
 *  #oski_traceargscmp_funcpt.
 *
 *  \param[in] trace Trace on which to compute an estimated
 *  running time.
 *  \param[in] time_stream The estimated time to stream through
 *  the matrix.
 *  \param[in] kernel_id Kernel to match, or INVALID_ID to
 *  match all kernels.
 *  \param[in] args Arguments of the given kernel. (Ignored if
 *  kernel_id == INVALID_ID.)
 *  \param[in] args_bytes Size of 'args' in bytes. (Ignored if
 *  kernel_id == INVALID_ID.)
 *  \param[in] func_compare A pointer to a user-defined function
 *  \f$f(a, b)\f$ to compare two kernel-specific argument lists,
 *  \f$a\f$ and \f$b\f$. If func_compare == NULL, then byte-wise
 *  comparisons are used by default.
 *  (Ignored if kernel_id == INVALID_ID.)
 *
 *  \returns The estimated time to execute all matching kernel
 *  records in the trace.
 */
double
oski_EstimateTraceTime (const oski_trace_t trace, double time_stream,
			oski_id_t kernel_id, const void *args,
			size_t args_bytes,
			oski_traceargscmp_funcpt func_compare)
{
  double time_elapsed;
  size_t i;
  size_t len;

  if (trace == INVALID_TRACE)
    return 0.0;
#if HAVE_C99_FPCLASSIFY
  if (fpclassify (time_stream) == FP_ZERO)
    return 0.0;
#else
  if (time_stream == 0.0)
    return 0.0;
#endif

  time_elapsed = 0.0;
  len = simplelist_GetLength (trace);
  for (i = 1; i <= len; i++)
    {
      const oski_tracerec_t *rec = simplelist_GetElem (trace, i);

      /* Skip this record if it does not match */
      if (!oski_MatchesTraceRecord (rec, kernel_id, args, args_bytes,
				    func_compare))
	continue;

      /* Matches, so accumulate time. */
      if (rec->time_elapsed >= 0.0)	/* Use actual measured time */
	time_elapsed += rec->time_elapsed;
      else			/* Simulate time */
	{
	  /* Approximate number of SpMV operations per kernel call */
	  double spmvs_per_call = 0.0;
	  switch (rec->kernel_id)
	    {
	    case OSKI_KERNEL_MatMult:
	      spmvs_per_call = GET_ARGS (rec->args, MatMult, num_vecs);
	      break;

	    case OSKI_KERNEL_MatTrisolve:
	      spmvs_per_call = GET_ARGS (rec->args, MatTrisolve, num_vecs);
	      break;

	    case OSKI_KERNEL_MatMultAndMatTransMult:
	      spmvs_per_call =
		2 * GET_ARGS (rec->args, MatMultAndMatTransMult, num_vecs);
	      break;

	    case OSKI_KERNEL_MatTransMatMult:
	      spmvs_per_call =
		2 * GET_ARGS (rec->args, MatTransMatMult, num_vecs);
	      break;

	    case OSKI_KERNEL_MatPowMult:
	      spmvs_per_call =
		GET_ARGS (rec->args, MatPowMult, power)
		* GET_ARGS (rec->args, MatPowMult, num_vecs);
	      break;
	    }

	  time_elapsed += spmvs_per_call * time_stream * rec->num_calls;
	}
    }

  return time_elapsed;
}

/**
 *  \brief
 *
 *  This implementation allows arbitrary queries on a trace
 *  through a user-defined comparison function of type
 *  #oski_traceargscmp_funcpt.
 *
 *  \param[in] trace Trace on which to compute an estimated
 *  running time.
 *  \param[in] kernel_id Kernel to match, or INVALID_ID to
 *  match all kernels.
 *  \param[in] args Arguments of the given kernel. (Ignored if
 *  kernel_id == INVALID_ID.)
 *  \param[in] args_bytes Size of 'args' in bytes. (Ignored if
 *  kernel_id == INVALID_ID.)
 *  \param[in] func_compare A pointer to a user-defined function
 *  \f$f(a, b)\f$ to compare two kernel-specific argument lists,
 *  \f$a\f$ and \f$b\f$. If func_compare == NULL, then byte-wise
 *  comparisons are used by default.
 *  (Ignored if kernel_id == INVALID_ID.)
 *
 *  \returns The number of flops needed per matrix non-zero entry
 *  to execute all matching kernel records in the trace.
 */
double
oski_CountTraceFlopsPerNz (const oski_trace_t trace,
			   oski_id_t kernel_id, const void *args,
			   size_t args_bytes,
			   oski_traceargscmp_funcpt func_compare)
{
  double flops_per_nz = 0.0;
  size_t i;

  if (trace == INVALID_TRACE)
    return 0.0;

  for (i = 1; i <= simplelist_GetLength (trace); i++)
    {
      const oski_tracerec_t *rec = simplelist_GetElem (trace, i);

      /* Skip this record if it does not match */
      if (!oski_MatchesTraceRecord (rec, kernel_id, args, args_bytes,
				    func_compare))
	continue;

      /* Matches, so accumulate flops. */
      switch (rec->kernel_id)
	{
	case OSKI_KERNEL_MatMult:
	  flops_per_nz += 2.0
	    * GET_ARGS (rec->args, MatMult, num_vecs) * rec->num_calls;
	  break;

	case OSKI_KERNEL_MatTrisolve:
	  flops_per_nz += 2.0
	    * GET_ARGS (rec->args, MatTrisolve, num_vecs) * rec->num_calls;
	  break;

	case OSKI_KERNEL_MatMultAndMatTransMult:
	  flops_per_nz += 4.0
	    * GET_ARGS (rec->args, MatMultAndMatTransMult, num_vecs)
	    * rec->num_calls;
	  break;

	case OSKI_KERNEL_MatTransMatMult:
	  flops_per_nz += 4.0
	    * GET_ARGS (rec->args, MatTransMatMult, num_vecs)
	    * rec->num_calls;
	  break;

	case OSKI_KERNEL_MatPowMult:
	  flops_per_nz += 2.0
	    * GET_ARGS (rec->args, MatPowMult, power)
	    * GET_ARGS (rec->args, MatPowMult, num_vecs) * rec->num_calls;
	  break;

	default:
	  break;
	}
    }

  return flops_per_nz;
}

/* eof */
