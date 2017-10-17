/**
 *  \file oski/CSR/module.h
 *  \ingroup MATTYPE_CSR
 *  \brief Compressed sparse row implementation.
 */

#if !defined(INC_OSKI_CSR_MODULE_H)
/** oski/CSR/module.h included. */
#define INC_OSKI_CSR_MODULE_H

#include <oski/matmodexport.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define oski_WrapCSR  MANGLE_MOD_(oski_WrapCSR)
#define oski_CheckCSR  MANGLE_MOD_(oski_CheckCSR)
#define oski_StreamMatRepr  MANGLE_MOD_(oski_StreamMatRepr)
#define oski_SortIndices  MANGLE_MOD_(oski_SortIndices)
#define oski_ExpandCSRToFull  MANGLE_MOD_(oski_ExpandCSRToFull)
#define oski_ConditionallyExpandCSRToFull  \
	MANGLE_MOD_(oski_ConditionallyExpandCSRToFull)
#define oski_ChangeIndexBase  MANGLE_MOD_(oski_ChangeIndexBase)
#define oski_TransposeCSR  MANGLE_MOD_(oski_TransposeCSR)
#define oski_CountZeroRowsCSR  MANGLE_MOD_(oski_CountZeroRowsCSR)
/*@}*/
#endif

/**
 *  \brief Verify that a CSR matrix representation satisfies
 *  asserted properties.
 */
int oski_CheckCSR (const oski_index_t * ptr, const oski_index_t * ind,
		   oski_index_t m, oski_index_t n,
		   oski_inmatpropset_t * props);

/**
 *  \brief Stream through a matrix.
 *
 *  This routine streams through the matrix data structure, performs
 *  some dummy computations, and returns the results. We use this
 *  routine to help estimate the cost of a matrix-vector multiply
 *  operation on a given matrix without actually having to perform
 *  matrix-vector multiply.
 */
double oski_StreamMatRepr (const void *mat, const oski_matcommon_t * props);

/**
 *  \brief Convert a half-storage symmetric or Hermitian matrix to
 *  full storage. 
 */
oski_matCSR_t *oski_ExpandCSRToFull (const oski_matCSR_t * S,
				     const oski_matcommon_t * props);

/**
 *  \brief Conditionally convert a half-storage symmetric or
 *  Hermitian matrix to full storage. 
 */
int oski_ConditionallyExpandCSRToFull (const oski_matCSR_t * S,
				       const oski_matcommon_t * props,
				       oski_matCSR_t ** p_mat_full);

/**
 *  \brief Change the index base of a matrix.
 */
void oski_ChangeIndexBase (oski_matCSR_t * A,
			   const oski_matcommon_t * props,
			   const oski_index_t new_base);

/**
 *  \brief Sort the column indices of a CSR matrix with m rows
 *  using b-based indexing.
 */
int oski_SortIndices (oski_matCSR_t * A, oski_index_t m);

/** 
 *  \brief Given a matrix \f$A\f$ in CSR format, returns a copy
 *  of its explicit transpose \f$A^T\f$ in CSR format.
 */
oski_matCSR_t *oski_TransposeCSR (const oski_matCSR_t * A,
				  const oski_matcommon_t * props);

/**
 *  \brief Returns the number of rows in a given CSR matrix
 *  that contain no structural non-zeros.
 */
oski_index_t oski_CountZeroRowsCSR (const oski_matCSR_t * A,
				    const oski_matcommon_t * props);

/**
 *  \name CSR wrapper.
 */
/*@{*/

/**
 *  \brief Create a new CSR-specific wrapper around
 *  a raw CSR array representation.
 */
oski_matCSR_t *oski_WrapCSR (oski_matcommon_t * out_props,
			     oski_index_t * Aptr, oski_index_t * Aind,
			     oski_value_t * Aval, oski_index_t num_rows,
			     oski_index_t num_cols,
			     oski_inmatpropset_t * props,
			     oski_copymode_t mode);
/*@}*/

#if defined(DO_NAME_MANGLING)
/** \name CSR-based sparse triangular solve kernel driver mangling. */
/*@{*/
#define CSR_MatTrisolveLower MANGLE_(CSR_MatTrisolveLower)
#define CSR_MatTransTrisolveLower MANGLE_(CSR_MatTransTrisolveLower)
#define CSR_MatConjTrisolveLower MANGLE_(CSR_MatConjTrisolveLower)
#define CSR_MatConjTransTrisolveLower MANGLE_(CSR_MatConjTransTrisolveLower)
#define CSR_MatTrisolveUpper MANGLE_(CSR_MatTrisolveUpper)
#define CSR_MatTransTrisolveUpper MANGLE_(CSR_MatTransTrisolveUpper)
#define CSR_MatConjTrisolveUpper MANGLE_(CSR_MatConjTrisolveUpper)
#define CSR_MatConjTransTrisolveUpper MANGLE_(CSR_MatConjTransTrisolveUpper)
/*@}*/
#endif

/** \name CSR-based sparse triangular solve kernel drivers. */
/*@{*/
void CSR_MatTrisolveLower (oski_index_t m, oski_index_t indbase,
			   int is_unit, int is_sorted,
			   const oski_index_t * ptr, const oski_index_t * ind,
			   const oski_value_t * val, oski_value_t alpha,
			   oski_value_t * x, oski_index_t num_vecs,
			   oski_index_t rowinc, oski_index_t colinc);

void CSR_MatConjTrisolveLower (oski_index_t m, oski_index_t indbase,
			       int is_unit, int is_sorted,
			       const oski_index_t * ptr,
			       const oski_index_t * ind,
			       const oski_value_t * val, oski_value_t alpha,
			       oski_value_t * x, oski_index_t num_vecs,
			       oski_index_t rowinc, oski_index_t colinc);

void CSR_MatTransTrisolveLower (oski_index_t m, oski_index_t indbase,
				int is_unit, int is_sorted,
				const oski_index_t * ptr,
				const oski_index_t * ind,
				const oski_value_t * val, oski_value_t alpha,
				oski_value_t * x, oski_index_t num_vecs,
				oski_index_t rowinc, oski_index_t colinc);

void CSR_MatConjTransTrisolveLower (oski_index_t m, oski_index_t indbase,
				    int is_unit, int is_sorted,
				    const oski_index_t * ptr,
				    const oski_index_t * ind,
				    const oski_value_t * val,
				    oski_value_t alpha, oski_value_t * x,
				    oski_index_t num_vecs,
				    oski_index_t rowinc, oski_index_t colinc);

void CSR_MatTrisolveUpper (oski_index_t m, oski_index_t indbase,
			   int is_unit, int is_sorted,
			   const oski_index_t * ptr, const oski_index_t * ind,
			   const oski_value_t * val, oski_value_t alpha,
			   oski_value_t * x, oski_index_t num_vecs,
			   oski_index_t rowinc, oski_index_t colinc);

void CSR_MatTransTrisolveUpper (oski_index_t m, oski_index_t indbase,
				int is_unit, int is_sorted,
				const oski_index_t * ptr,
				const oski_index_t * ind,
				const oski_value_t * val, oski_value_t alpha,
				oski_value_t * x, oski_index_t num_vecs,
				oski_index_t rowinc, oski_index_t colinc);

void CSR_MatConjTrisolveUpper (oski_index_t m, oski_index_t indbase,
			       int is_unit, int is_sorted,
			       const oski_index_t * ptr,
			       const oski_index_t * ind,
			       const oski_value_t * val, oski_value_t alpha,
			       oski_value_t * x, oski_index_t num_vecs,
			       oski_index_t rowinc, oski_index_t colinc);

void CSR_MatConjTransTrisolveUpper (oski_index_t m, oski_index_t indbase,
				    int is_unit, int is_sorted,
				    const oski_index_t * ptr,
				    const oski_index_t * ind,
				    const oski_value_t * val,
				    oski_value_t alpha, oski_value_t * x,
				    oski_index_t num_vecs,
				    oski_index_t rowinc, oski_index_t colinc);
/*@}*/

#endif

/* eof */
