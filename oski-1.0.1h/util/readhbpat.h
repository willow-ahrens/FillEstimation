/**
 *  \file readhbpat.h
 *  \brief Defines utility routine to read a matrix pattern from a
 *  Harwell-Boeing formatted file.
 */

#if !defined(INC_READHBPAT_H)
/** readhbpat.h included. */
#define INC_READHBPAT_H

#include <oski/common.h>
#include <oski/mangle.h>

#if defined(DO_NAME_MANGLING)
/** \name Name mangling. */
/*@{*/
#define readhb_pattern MANGLE_(readhb_pattern)
#define readhb_pattern_matrix MANGLE_(readhb_pattern_matrix)
#define readhb_pattern_matrix_nnz MANGLE_(readhb_pattern_matrix_nnz)
#define readhb_expand_symm MANGLE_(readhb_expand_symm)
/*@}*/
#endif

/**
 *  \brief Read the raw pattern of a sparse matrix stored in
 *  Harwell-Boeing formatted file in compressed-sparse column
 *  format.
 */
int readhb_pattern (const char *filename,
		    oski_index_t * p_m, oski_index_t * p_n,
		    oski_index_t * p_nnz, oski_index_t ** p_ptr,
		    oski_index_t ** p_ind, char *mattype);

/**
 *  \brief Expand a raw CSR/CSC matrix to full storage.
 */
int readhb_expand_symm (oski_index_t n, oski_index_t base, int is_herm,
                        const oski_index_t* Sptr, const oski_index_t* Sind,
                        const oski_value_t* Sval,
                        oski_index_t** p_Aptr, oski_index_t** p_Aind,
                        oski_value_t** p_Aval);

#include <oski/matrix.h>

/**
 *  \brief Read the pattern of a sparse matrix from a Harwell-Boeing
 *  formatted file, and return it as a tunable matrix handle.
 */
oski_matrix_t readhb_pattern_matrix (const char *filename,
				     oski_index_t * p_m, oski_index_t * p_n,
				     char *p_mattype,
                                     int expand_symm);

/**
 *  \brief Read the pattern of a sparse matrix from a Harwell-Boeing
 *  formatted file, and return it as a tunable matrix handle.
 */
oski_matrix_t readhb_pattern_matrix_nnz (const char *filename,
					 oski_index_t * p_m,
					 oski_index_t * p_n,
					 oski_index_t * p_nnz_stored,
					 oski_index_t * p_nnz_true,
					 char *p_mattype,
                                         int expand_symm);

#endif

/* eof */
