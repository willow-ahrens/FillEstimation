#include <stdio.h>
#include <stdlib.h>
#include <taco.h>
#include "util.h"

extern "C" {

const char *name () {
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
int estimate_fill (int m,
                   int n,
                   int nnz,
                   const int *ptr,
                   const int *ind,
                   int B,
                   double epsilon,
                   double delta,
                   double *fill,
                   int verbose){
  taco::Tensor<double> blocks({m, n}, taco::CSR);
  int fill_index = 0;
  for (int b_r = 1; b_r <= B; b_r++) {
    for (int b_c = 1; b_c <= B; b_c++) {
      blocks.zero();
      int i = 0;
      int j = 0;
      int nnzb = 0;
      for (int t = 0; t < nnz; t++){
        while (ptr[i + 1] <= t) {
          i++;
        }
        j = ind[t];
        int block_i = (i/b_r) + 1;
        int block_j = (j/b_c) + 1;
        blocks.insert({block_i, block_j}, 1.0);
      }
      blocks.pack();
      fill[fill_index] = b_r * b_c * blocks.getStorage().getValues().getSize() / (double)nnz;
      fill_index++;
    }
  }
  return 0;
}

}
