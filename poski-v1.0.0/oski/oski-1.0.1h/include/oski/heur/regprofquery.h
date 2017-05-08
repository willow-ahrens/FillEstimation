/**
 *  \file include/oski/heur/regprofquery.h
 *  \brief Workload query interface for register blocking heuristic.
 */

#if !defined(INC_HEUR_REGPROFQUERY_H)
/** oski/heur/regprofquery.h included. */
#define INC_HEUR_REGPROFQUERY_H

#include <oski/common.h>
#include <oski/simplelist.h>
#include <oski/mangle.h>
#include <oski/matrix.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define oski_regprofkernel_t MANGLE_(oski_regprofkernel_t)
#define oski_GetRegProfileKernelWorkload \
	MANGLE_(oski_GetRegProfielKernelWorkload)
#define oski_DestroyRegProfileKernelWorkload \
	MANGLE_(oski_DestroyRegProfielKernelWorkload)
/*@}*/
#endif

/** Stores kernel-specific register profile information. */
typedef struct
{
  oski_id_t id;	      /**< ID# of this kernel */
	/** Transpose option, if any */
  union
  {
    oski_matop_t mat;		  /**< MatMult and related kernels */
    oski_ataop_t ata;		  /**< MatTransMatMult */
  } op;

  double num_flops;	  /**< Raw flops per nz due to this kernel */
  double norm_flops;	   /**< Normalized flop count over workload */
  oski_regprof_t profile;	/**< Raw register profile data */

  int has_alt_profile;	     /**< Non-zero <==> has alternate profile */
  oski_regprof_t alt_profile;	    /**< Profile of alternate implementation */
} oski_regprofkernel_t;

/** Preprocesses a trace for the register blocking heuristic. */
simplelist_t *oski_GetRegProfileKernelWorkload (const oski_matcommon_t * A,
						const oski_trace_t W);

/** Free memory associated with a kernel workload specification */
void oski_DestroyRegProfileKernelWorkload (simplelist_t * L);

#endif

/* eof */
