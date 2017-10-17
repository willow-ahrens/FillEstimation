/**
 *  \file oski/inmatprop.h
 *  \brief Input matrix properties collection and inspection.
 *
 *  This module implements facilities that help collect, record,
 *  and examine the input matrix properties specified by the
 *  user when she creates the matrix handle.
 *
 *  Header file dependencies:
 *    - oski/userconst.h
 */

#if !defined(INC_OSKI_INMATPROP_H)
/** inmatprop.h has been included. */
#define INC_OSKI_INMATPROP_H

#include <assert.h>
#include <stdarg.h>
#include <oski/userconst.h>

/**
 *  \brief Property undefined.
 *
 *  This macro defines a special value used to denote that an input
 *  matrix property has not yet been defined.
 *
 *  \see oski/userconst.h
 *  \see oski_inmatprop_t
 */
#define INMATPROP_UNDEFINED (int)(INMATPROP_NULL)

/** Smallest value of the enumerated type, oski_inmatprop_t. */
#define MIN_INMATPROP MAT_GENERAL
/** Largest value of the enumerated type, oski_inmatprop_t. */
#define MAX_INMATPROP (INMATPROP_MAX-1)

/**
 *  \brief Collected input matrix property settings.
 *
 *  On matrix handle creation, the implementation parses the
 *  user's variable argument list specifying input matrix
 *  properties, and stores them in a 'canonical form' in an
 *  instance of this data structure.
 *
 *  \see oski_inmatprop_t, oski_InitMatPropSet()
 */
typedef struct
{
  oski_inmatprop_t pattern;	  /**< [DEFAULT=MAT_GENERAL]. */
  int has_unit_diag_implicit;	    /**< 0 or 1 [DEFAULT=0]. */
  int index_base;	/**< 0 or 1 [DEFAULT=1]. */
  int has_sorted_indices;	/**< 0 or 1 [DEFAULT=0]. */
  int has_unique_indices;	/**< 0 or 1 [DEFAULT=0]. */
}
oski_inmatpropset_t;

/**
 *  \brief Initialize property set with "unset" values.
 */
void oski_ClearInMatPropSet (oski_inmatpropset_t * props);

/**
 *  \brief Complete unitialized properties with default values.
 */
void oski_CompleteDefaultInMatPropSet (oski_inmatpropset_t * props);

/**
 *  \brief Checks an input matrix property set for consistency.
 */
int oski_IsInMatPropSetConsistent (const oski_inmatpropset_t * props);

/**
 *  \brief Display an input matrix's asserted properties (FOR DEBUGGING ONLY).
 */
void oski_DisplayInMatPropSet (const oski_inmatpropset_t * props);

/**
 *  \brief Collect input matrix properties.
 *
 *  Assuming the caller has a variable input argument list, this
 *  macro parses these arguments and collects them in an instance
 *  of oski_inmatpropset_t.
 *
 *  \param[in] k The number of remaining arguments.
 *  \param[out] props A pointer to an instance of oski_inmatpropset_t.
 *  Its values will be overwritten by the call to this macro.
 *
 *  \returns A cleared 'props' structure, and returns 0 if no error occurs.
 *
 *  \see oski_inmatpropset_t, oski_inmatprop_t.
 */
int oski_CollectInMatProps (oski_inmatpropset_t* props, int k, ...);

/**
 *  \brief Explicit variable argument version of oski_CollectInMatProps().
 */
int oski_CollectInMatProps_va (oski_inmatpropset_t* props,
                               int k,
                               va_list ap);

/**
 *  \brief Explicit property-array version of oski_CollectInMatProps().
 */
int oski_CollectInMatProps_arr (oski_inmatpropset_t* props,
				int k, const oski_inmatprop_t* in_props);

#endif /* !defined(INC_OSKI_INMATPROP_H) */

/* eof */
