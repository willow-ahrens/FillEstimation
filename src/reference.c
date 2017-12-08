#include <stdio.h>
#include <stdlib.h>
#include <gsl/gsl_spmatrix.h>
#include "util.h"

char *name () {
  return "reference";
}

/**
 *  Given an m by n CSR matrix A, computes the fill ratio if the matrix were
 *  converted into b_r by b_c BCSR format. The fill ratio is b_r times b_c times
 *  the number of nonzero blocks in the BCSR format divided by the number of
 *  nonzeros.
 *
 *  The caller supplies this routine with a maximum row and column block size B,
 *  and this routine returns the estimated fill ratios for all
 *  1 <= b_r, b_c <= B.
 *
 *  This routine assumes the CSR matrix uses full storage, and assumes that
 *  column indicies are sorted.
 *
 *  \param[in] m Logical number of matrix rows
 *  \param[in] n Logical number of matrix columns
 *  \param[in] nnz Logical number of matrix nonzeros
 *  \param[in] *ptr CSR row pointers.
 *  \param[in] *ind CSR column indices.
 *  \param[in] B Maximum desired block size
 *  \param[in] epsilon Epsilon
 *  \param[in] delta Delta
 *  \param[in] sigma Sigma
 *  \param[out] *fill Fill ratios for all specified b_r, b_c in order
 *  \param[in] verbose 0 if you should be quiet
 *
 *  Note that the fill ratios should be stored according to the following order:
 *  int fill_index = 0;
 *  for (int b_r = 1; b_r <= B; b_r++) {
 *    for (int b_c = 1; b_c <= B; b_c++) {
 *      fill[fill_index] = fill for b_r, b_c
 *      fill_index++;
 *    }
 *  }
 *
 *  \returns On success, returns 0. On error, returns an error code.
 */
int estimate_fill (size_t m,
                   size_t n,
                   size_t nnz,
                   const size_t *ptr,
                   const size_t *ind,
                   size_t B,
                   double epsilon,
                   double delta,
                   double *fill,
                   int verbose){
  gsl_spmatrix *blocks = gsl_spmatrix_alloc_nzmax(m, n, nnz, GSL_SPMATRIX_TRIPLET);
  size_t fill_index = 0;
  for (size_t b_r = 1; b_r <= B; b_r++) {
    for (size_t b_c = 1; b_c <= B; b_c++) {
      gsl_spmatrix_set_zero(blocks);
      size_t i = 0;
      size_t j = 0;
      size_t nnzb = 0;
      for (size_t t = 0; t < nnz; t++){
        while (ptr[i + 1] <= t) {
          i++;
        }
        j = ind[t];
        size_t block_i = (i/b_r) + 1;
        size_t block_j = (j/b_c) + 1;
	gsl_spmatrix_set(blocks, block_i, block_j, 1.0);
      }
      fill[fill_index] = b_r * b_c * gsl_spmatrix_nnz(blocks) / (double)nnz;
      fill_index++;
    }
  }
  gsl_spmatrix_free(blocks);
  return 0;
}
