/**
 *  \file testvec.h
 *  \brief Test vector generation and checking.
 *
 *  \todo testmat_ChooseDim and ChooseDivisible do not actually need
 *  to be build-type dependent.
 */

#if !defined(INC_UTIL_TESTVEC_H)
/** tests/test_util.h included. */
#define INC_UTIL_TESTVEC_H

#include <stdlib.h>

#include <oski/common.h>
#include <oski/matrix.h>
#include <oski/matcreate.h>
#include <oski/vecview.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define testvec_Create        MANGLE_(testvec_Create)
#define testvec_CalcDataSize  MANGLE_(testvec_CalcDataSize)
#define testvec_Clone         MANGLE_(testvec_Clone)
#define testvec_Destroy       MANGLE_(testvec_Destroy)

#define testmat_ChangeSizeDenseCSR \
	MANGLE_(testmat_ChangeSizeDenseCSR)
#define testmat_ChangeSizeDenseTriCSR \
	MANGLE_(testmat_ChangeSizeDenseTriCSR)
#define testmat_GenDenseCSR   MANGLE_(testmat_GenDenseCSR)
#define testmat_GenRandomCSR  MANGLE_(testmat_GenRandomCSR)
#define testmat_GenTriCSR   MANGLE_(testmat_GenTriCSR)
#define testmat_GenTriCSC   MANGLE_(testmat_GenTriCSC)
#define testmat_GenBlockedBandedCSR MANGLE_(testmat_GenBlockedBandedCSR)

#define check_MatMult_instance  \
	MANGLE_(check_MatMult_instance)
#define check_MatTrisolve_instance \
	MANGLE_(check_MatTrisolve_instance)
#define check_MatTransMatMult_instance  \
	MANGLE_(check_MatTransMatMult_instance)
#define check_MatMultAndMatTransMult_instance \
	MANGLE_(check_MatMultAndMatTransMult_instance)

#define testmat_ChooseDim  MANGLE_(testmat_ChooseDim)
#define ChooseDivisible MANGLE_(ChooseDivisible)
/*@}*/
#endif

/**
 *  \brief Allocate a dense 2-D array, fill it with random values, and
 *  return it wrapped in a newly allocated vector view object.
 */
oski_vecview_t testvec_Create (oski_index_t len, oski_index_t num_vecs,
			       oski_storage_t orient, int use_minstride);

/**
 *  \brief Based on its storage orientation and stride, returns the
 *  minimum size (in elements) of the underlying storage array.
 */
size_t testvec_CalcDataSize (const oski_vecview_t x);

/**
 *  \brief Clone an object created using testvec_Create().
 */
oski_vecview_t testvec_Clone (const oski_vecview_t x);

/**
 *  \brief Destroy a test vector created via testvec_Create().
 */
void testvec_Destroy (oski_vecview_t x);

/**
 *  \brief Select the dimension for a square, dense matrix whose size in
 *  Bytes exceeds nz_volume.
 */
size_t testmat_ChooseDim (size_t nz_volume, size_t elem_size);

/**
 *  \brief Returns the smallest integer \f$n' >= n\f$ such that
 *  \f$d | n'\f$.
 */
size_t ChooseDivisible (size_t n, size_t d);

/**
 *  \brief Generates a dense, non-symmetric matrix in sparse CSR format.
 *
 *  The values are chosen randomly.
 */
void testmat_GenDenseCSR (oski_index_t m, oski_index_t n,
			  oski_index_t ** p_ptr, oski_index_t ** p_ind,
			  oski_value_t ** p_val);

/**
 *  \brief Changes the dimensions (and pattern) of a previously allocated
 *  dense CSR test matrix.
 */
void testmat_ChangeSizeDenseCSR (oski_index_t m_new, oski_index_t n_new,
				 oski_index_t * ptr, oski_index_t * ind);

/**
 *  \brief Changes the pattern of a previously allocated dense CSR test
 *  matrix to a dense triangular pattern.
 */
void testmat_ChangeSizeDenseTriCSR (oski_index_t n_new,
				    int make_lower, int cond_for_trisolve,
				    oski_index_t * ptr, oski_index_t * ind,
				    oski_value_t * val);

/**
 *  \brief Generates a random, well-conditioned sparse triangular
 *  matrix in CSR format with the prescribed attributes for testing
 *  triangular solve.
 */
void testmat_GenTriCSR (oski_index_t m, oski_index_t max_nnz_row,
			oski_inmatprop_t shape, int is_unitdiag,
			int is_sorted, int indbase, oski_index_t ** p_ptr,
			oski_index_t ** p_ind, oski_value_t ** p_val);

/**
 *  \brief Generates a random, well-conditioned sparse triangular
 *  matrix in CSC format with the prescribed attributes for testing
 *  triangular solve.
 */
void testmat_GenTriCSC (oski_index_t m, oski_index_t max_nnz_row,
			oski_inmatprop_t shape, int is_unitdiag,
			int is_sorted, int indbase, oski_index_t ** p_ptr,
			oski_index_t ** p_ind, oski_value_t ** p_val);

/**
 *  \brief Generates a matrix with a banded shape, where each row
 *  contains dense blocks of a given size.
 */
void testmat_GenBlockedBandedCSR (oski_index_t k_min, oski_index_t row_min,
				  oski_index_t r, oski_index_t c,
				  oski_index_t * p_n,
				  oski_index_t ** p_ptr,
				  oski_index_t ** p_ind,
				  oski_value_t ** p_val);
/**
 *  \brief Check an instance of matrix-vector multiply.
 */
int check_MatMult_instance (const oski_matrix_t A0, const oski_matrix_t A1,
			    oski_matop_t opA, oski_value_t alpha,
			    const oski_vecview_t x, oski_value_t beta,
			    oski_vecview_t y0);

/**
 *  \brief Check an instance of sparse triangular solve.
 */
int check_MatTrisolve_instance (const oski_matrix_t T0,
				const oski_matrix_t T1, oski_matop_t opT,
				oski_value_t alpha, const oski_vecview_t b);

/**
 *  \brief Check an instance of sparse \f$A^TAx\f$.
 */
int check_MatTransMatMult_instance (const oski_matrix_t A0,
				    const oski_matrix_t A1, oski_ataop_t opA,
				    oski_value_t alpha,
				    const oski_vecview_t x, oski_value_t beta,
				    oski_vecview_t y0, oski_vecview_t t0);

/**
 *  \brief Check and instance of simultaneous multiplication by
 *  sparse \f$A\f$ and \f$\mathrm{op}(A)\f$.
 */
int check_MatMultAndMatTransMult_instance (const oski_matrix_t A0,
					   const oski_matrix_t A1,
					   oski_value_t alpha,
					   const oski_vecview_t x,
					   oski_value_t beta,
					   oski_vecview_t y, oski_matop_t opA,
					   oski_value_t omega,
					   const oski_vecview_t w,
					   oski_value_t zeta,
					   oski_vecview_t z);

/**
 *  \brief Create a raw matrix pattern and matrix object corresponding
 *  to the caller-specified dimensions and shape.
 */
oski_matrix_t testmat_GenRandomCSR (oski_index_t m, oski_index_t n,
				    oski_index_t min_nnz_row,
				    oski_index_t max_nnz_row,
				    oski_inmatprop_t shape,
				    int implicit_diag, int index_base,
				    oski_copymode_t copymode,
				    oski_index_t ** p_ptr,
				    oski_index_t ** p_ind,
				    oski_value_t ** p_val);

#endif

/* eof */
