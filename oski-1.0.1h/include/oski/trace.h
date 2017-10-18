/**
 *  \file oski/trace.h
 *  \ingroup TRACING
 *  \brief Implements a database for keeping track of kernel calls.
 */

#if !defined(INC_OSKI_TRACE_H) && !defined(OSKI_UNBIND)
/** oski/trace.h included. */
#define INC_OSKI_TRACE_H

#include <oski/common.h>
#include <oski/simplelist.h>

/**
 *  \brief A trace record that stores the number of times
 *  a kernel has been (and/or will be) called with a particular
 *  argument list.
 */
typedef struct
{
  oski_id_t id;			 /**< ID# for this record. */

  oski_id_t kernel_id;		    /**< ID# for the kernel called. */
  const void *args;		     /**< Kernel-specific argument list. */
  size_t args_bytes;		     /**< Size of 'args' in bytes */

  size_t num_calls;		     /**< # of times called */
  double time_elapsed;		     /**< elapsed seconds of execution time
								  or < 0 if none available. */
}
oski_tracerec_t;

/**
 *  \brief NULL trace record.
 */
#define INVALID_TRACEREC  ((oski_tracerec_t *)NULL)

/**
 *  \brief Type for a function to compare two trace arguments of
 *  the same type.
 *
 *  This type is a pointer to a comparison function f(a, b)
 *  where a and b are pointers to kernel-specific argument lists
 *  (i.e., the 'args' field of a oski_tracerec_t structure).
 *
 *  When an object of this type is passed to one of the routines
 *  below, then the first argument `a' is ALWAYS the trace argument
 *  list for the record stored in the trace, and second argument
 *  `b' is the trace argument list being sought.
 */
typedef int (*oski_traceargscmp_funcpt) (const void *a, const void *b);

/**
 *  \brief A trace is a list of kernel calls and their argument
 *  signatures.
 */
typedef simplelist_t *oski_trace_t;

/** NULL trace. */
#define INVALID_TRACE  ((oski_trace_t)NULL)

/** Time not available. */
#define TIME_NOT_AVAIL  ((double)(-1.0))

/** \brief Create and return a new trace. */
oski_trace_t oski_CreateTrace (void);

/** \brief Destroy a trace. */
void oski_DestroyTrace (oski_trace_t trace);

/** \brief Save a kernel call to a trace. */
int oski_RecordCalls (oski_trace_t trace, oski_id_t kernel_id,
		      const void *args, size_t args_bytes,
		      oski_traceargscmp_funcpt func_compare,
		      size_t num_new_calls, double time_elapsed);

/** \brief Wrapper around oski_RecordCalls assuming just 1 call. */
#define oski_RecordCall(trace, id, args, args_size, func, time_elapsed) \
	oski_RecordCalls(trace, id, args, args_size, func, 1, time_elapsed)

/** \brief Returns the frequency of a particular kernel call for
 * all matching records. */
size_t oski_GetNumCalls (const oski_trace_t trace,
			 oski_id_t kernel_id, const void *args,
			 size_t args_bytes,
			 oski_traceargscmp_funcpt func_compare);

/**
 *  \brief Returns 1 if a given trace record matches a query request,
 *  or 0 otherwise.
 */
int oski_MatchesTraceRecord (const oski_tracerec_t * rec,
			     oski_id_t kernel_id, const void *args,
			     size_t args_bytes,
			     oski_traceargscmp_funcpt func_compare);

/* ---------- BEGIN Matrix type-dependent section ------------- */
#if defined(IND_TAG_CHAR) && defined(VAL_TAG_CHAR)

#include <oski/mangle.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling */
/*@{*/
#define oski_EstimateTraceTimeTotal MANGLE_(oski_EstimateTraceTimeTotal)
#define oski_EstimateTraceTime MANGLE_(oski_EstimateTraceTime)
#define oski_CountTraceFlopsPerNz  MANGLE_(oski_CountTraceFlopsPerNz)
/*@}*/
#endif

/**
 *  \brief Returns an estimate of the time to execute a trace,
 *  given the estimated time to stream through the matrix data
 *  structure.
 */
double oski_EstimateTraceTimeTotal (const oski_trace_t trace,
				    double time_stream);

/**
 *  \brief Returns an estimate of the time to execute a subset of a
 *  trace, given the estimated time to stream through the matrix
 *  data structure.
 */
double oski_EstimateTraceTime (const oski_trace_t trace,
			       double time_stream, oski_id_t kernel_id,
			       const void *args, size_t args_bytes,
			       oski_traceargscmp_funcpt func_compare);

/**
 *  \brief Returns the number of flops executed per matrix
 *  non-zero, given a trace.
 */
double oski_CountTraceFlopsPerNz (const oski_trace_t trace,
				  oski_id_t kernel_id, const void *args,
				  size_t args_bytes,
				  oski_traceargscmp_funcpt func_compare);

#endif
/* ---------- END Matrix type-dependent section ------------- */
#endif /* !defined(INC_OSKI_TRACE_H) */

#if defined(OSKI_UNBIND)
#  include <oski/mangle.h>
#  undef oski_EstimateTraceTimeTotal
#  undef oski_EstimateTraceTime
#  undef oski_CountTraceFlopsPerNz
#endif

/* eof */
