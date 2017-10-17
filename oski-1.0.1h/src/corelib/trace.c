/**
 *  \file src/trace.c
 *  \ingroup TRACING
 *  \brief Implements a database system for tracking kernel calls.
 */

#include <string.h>

#include <oski/config.h>

#include <oski/common.h>
#include <oski/trace.h>

#if HAVE_MEMCMP
	/** \brief Returns 0 <==> a and b match. */
#define COMPARE_BYTES(a, b, n)  \
		((int)memcmp((const void *)(a), (const void *)(b), (size_t)(n)))
#elif HAVE_BCMP
	/** \brief Returns 0 <==> a and b match. */
#define COMPARE_BYTES(a, b, n)  \
		((int)bcmp((const void *)(a), (const void *)(b), (size_t)(n)))
#elif HAVE_STRNCMP
	/** \brief Returns 0 <==> a and b match. */
#define COMPARE_BYTES(a, b, n)  \
		((int)strncmp((const char *)(a), (const char *)(b), (size_t)(n)))
#else
	/** \brief Returns 0 <==> a and b match. */
static int
compare_bytes (const void *a, const void *b, size_t num_bytes)
{
  size_t i;
  const unsigned char *pa = (const unsigned char *) a;
  const unsigned char *pb = (const unsigned char *) b;

  for (i = 0; i < num_bytes; i++)
    if (pa[0] < pb[0])
      return -1;
    else if (pa[0] > pb[0])
      return 1;

  return 0;
}

	/** \brief Returns 0 <==> a and b match. */
#define COMPARE_BYTES(a, b, n) \
		(int)compare_bytes((const void *)(a), (const void *)(b), (size_t)(n))
#endif

/**
 *  \brief
 *
 *  \param[in] rec Trace record to test.
 *  \param[in] kernel_id Kernel to match, or INVALID_ID to match
 *  any kernel.
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
 *  \returns 1 if 'rec' matches the query request,
 *  ('kernel_id', 'args', 'args_bytes', 'func_compare'),
 *  or 0 otherwise. If 'rec' == INVALID_TRACREC, this routine
 *  returns 0.
 */
int
oski_MatchesTraceRecord (const oski_tracerec_t * rec,
			 oski_id_t kernel_id, const void *args,
			 size_t args_bytes,
			 oski_traceargscmp_funcpt func_compare)
{
  if (rec == INVALID_TRACEREC)
    return 0;
  if (kernel_id == INVALID_ID)
    return 1;
  if (rec->kernel_id == kernel_id)
    {
      if (args == NULL || rec->args == NULL)
	return 1;
      if (func_compare == NULL)
	return COMPARE_BYTES (rec->args, args, args_bytes) == 0;
      else
	return (*func_compare) (rec->args, args);
    }
  return 0;
}

oski_trace_t
oski_CreateTrace (void)
{
  return (oski_trace_t) simplelist_Create ();
}

/**
 *  \brief
 */
void
oski_DestroyTrace (oski_trace_t trace)
{
  simplelist_t *trace_list = (simplelist_t *) trace;
  size_t i;

  for (i = 1; i <= simplelist_GetLength (trace_list); i++)
    {
      oski_tracerec_t *rec =
	(oski_tracerec_t *) simplelist_GetElem (trace_list, i);

      if (rec == INVALID_TRACEREC)
	continue;

      oski_Free ((void *) rec->args);
      oski_Free (rec);
    }
  simplelist_Destroy ((simplelist_t *) trace);
}

/**
 *  \brief
 *
 *  \param[in,out] trace Trace to search and update.
 *  \param[in] kernel_id ID# of the kernel.
 *  \param[in] args Kernel-specific argument list. If set to
 *  NULL, then the caller requests that the update occur to
 *  the first record matching just kernel_id.
 *  \param[in] args_bytes Size of the data pointed to by args,
 *  in bytes.
 *  \param[in] func_compare Function to perform record-matching
 *  comparison. If set to NULL, then a default function that
 *  performs bit-wise equality comparison is used.
 *  \param[in] num_new_calls Number of calls to record.
 *  \param[in] time_elapsed Number of seconds of execution time
 *  due to these calls, or #TIME_NOT_AVAIL.
 *
 *  This routine searches for the first record (one with lowest
 *  ID#) in the trace that "matches" the given kernel. A match
 *  means that the following two conditions hold:
 *  -# func_compare(record->args, args) == 0 OR record->args == NULL
 *  OR args == NULL.
 *  -# (time_elapsed < 0) == (record->time_elapsed < 0)
 *
 *  If a matching record cannot be found, then a new one is
 *  created.
 *
 *  \returns 0 on success.
 *  If trace of kernel_id are invalid, returns an error code
 *  (ERR_BAD_ARG) and takes no action. If there is not enough
 *  memory to create a new trace record, returns
 *  ERR_OUT_OF_MEMORY.
 */
int
oski_RecordCalls (oski_trace_t trace, oski_id_t kernel_id,
		  const void *args, size_t args_bytes,
		  oski_traceargscmp_funcpt func_compare,
		  size_t num_new_calls, double time_elapsed)
{
  simplelist_t *trace_list = (simplelist_t *) trace;
  size_t i;

  oski_tracerec_t *new_rec;
  oski_id_t new_rec_id;

  if ((trace == INVALID_TRACE) || (kernel_id == INVALID_ID))
    return ERR_BAD_ARG;

  for (i = 1; i <= simplelist_GetLength (trace_list); i++)
    {
      oski_tracerec_t *rec =
	(oski_tracerec_t *) simplelist_GetElem (trace_list, i);
      /* Cast overrides 'const' qualifier. */

      /* Check first condition, i.e., that args match. */
      if (!oski_MatchesTraceRecord (rec, kernel_id, args, args_bytes,
				    func_compare))
	continue;

      /* Check second condition, i.e., that elapsed time data is
       * compatible. */
      if ((rec->time_elapsed < 0) == (time_elapsed < 0))
	{
	  rec->num_calls += num_new_calls;
	  if (time_elapsed > 0)
	    rec->time_elapsed += time_elapsed;
	  return 0;
	}
    }

  /* Not found; create a new trace record */
  new_rec = oski_Malloc (oski_tracerec_t, 1);
  if (new_rec == NULL)
    return ERR_OUT_OF_MEMORY;

  /* Copy argument list */
  new_rec->args = (const void *) oski_Malloc (unsigned char, args_bytes);
  if (new_rec->args == NULL && args_bytes > 0)
    {
      oski_Free (new_rec);
      return ERR_OUT_OF_MEMORY;
    }

  new_rec_id = simplelist_Append (trace_list, new_rec);
  if (new_rec_id == INVALID_ID)
    {
      oski_Free ((void *) new_rec->args);
      oski_Free (new_rec);
      return ERR_OUT_OF_MEMORY;
    }

  new_rec->id = new_rec_id;
  new_rec->kernel_id = kernel_id;
  new_rec->num_calls = num_new_calls;
  new_rec->time_elapsed = time_elapsed;

  oski_CopyMem (new_rec->args, args, unsigned char, args_bytes);
  new_rec->args_bytes = args_bytes;

  return 0;
}

/**
 *  \brief
 *
 *  \param[in] trace Trace to search.
 *  \param[in] kernel_id ID# of the kernel to find.
 *  \param[in] args Kernel-specific argument list. If set to NULL,
 *  then match any record with kernel ID# kernel_id.
 *  \param[in] args_bytes Size of object pointed to by 'args' in bytes.
 *  \param[in] func_compare Function to perform record-matching
 *  comparison. If NULL, then do a byte-wise comparison.
 *  \returns The total number of calls across all records
 *  that match according to the comparison function, func_compare.
 */
size_t
oski_GetNumCalls (const oski_trace_t trace,
		  oski_id_t kernel_id, const void *args, size_t args_bytes,
		  oski_traceargscmp_funcpt func_compare)
{
  const simplelist_t *trace_list = (const simplelist_t *) trace;
  size_t i;
  size_t total_calls;

  total_calls = 0;
  for (i = 1; i <= simplelist_GetLength (trace_list); i++)
    {
      const oski_tracerec_t *rec =
	(const oski_tracerec_t *) simplelist_GetElem (trace_list, i);

      if (oski_MatchesTraceRecord (rec, kernel_id, args, args_bytes,
				   func_compare))
	total_calls += rec->num_calls;
    }

  return total_calls;
}

/* eof */
