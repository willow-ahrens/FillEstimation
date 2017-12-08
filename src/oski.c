#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

char * name() {
  return "oski";
}

/**
 *  Given an m by n CSR matrix A, estimates the fill ratio if the matrix were
 *  converted into b_r by b_c BCSR format. The fill ratio is b_r times b_c times
 *  the number of nonzero blocks in the BCSR format divided by the number of
 *  nonzeros. For each setting of b_r, block rows are completely examined with
 *  probability sigma.
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
                   double sigma,
                   double *fill,
                   int verbose){
  assert(n >= 1);
  assert(m >= 1);

  /* blocks + (c - 1) * n stores previously seen column block indicies in the
   * current block row when b_c = c.
   */
  int *blocks = (int*)malloc(sizeof(int) * B * n);
  assert(blocks != NULL);
  memset(blocks, 0, sizeof(int) * B * n);

  /* K[(c - 1)] counts distinct column block indicies in the current block row
   * when b_c = c.
   */
  size_t K[B];

  /* see above note about fill order */
  int fill_index = 0;

  for (int r = 1; r <= B; r++) {

    /* M is the number of block rows */
    size_t M = m / r;

    /* stores the number of examined nonzeros */
    size_t S = 0;

    for (int c = 1; c <= B; c++){
      K[c - 1] = 0;
    }

    /* loop over block rows */
    for (size_t I = 0; I < M; I++) {

      /* examine the block row with probability sigma */
      if (random_uniform() > sigma){
        continue;
      }else{

        /* Count the blocks in block row I, using "blocks" to remember the
         * blocks that have been seen so far for each block column width "c".
         */
        for (size_t i = I * r; i < (I + 1) * r; i++) {
          for (size_t t = ptr[i]; t < ptr[i + 1]; t++) {
            size_t j = ind[t];

            for (int c = 1; c <= B; c++) {
              /* "J" is the block column index */
              size_t J = j / c;

              /* if the block has not yet been seen, count it */
              if (blocks[(c - 1) * n + J] == 0) {
                blocks[(c - 1) * n + J] = 1;
                K[c - 1]++;
              }
            }
          }
        }
      }
      S += ptr[(I + 1) * r] - ptr[I * r];

      /*
       * Reset "blocks" for the next block row. We loop over the nonzeros
       * instead of calling "memset" in order to keep the complexity to O(nnz).
       */
      for (size_t i = I * r; i < (I + 1) * r; i++) {
        for (size_t t = ptr[i]; t < ptr[i + 1]; t++) {
          size_t j = ind[t];

          for (int c = 1; c <= B; c++) {
            /* "J" is the block column index */
            size_t J = j / c;
            blocks[(c - 1) * n + J] = 0;
          }
        }
      }
    }

    /*
     * Compute the fill from the number of blocks and nonzeros that have been
     * seen in the sample.
     */
    for (int c = 1; c <= B; c++) {
      if (!S)
        fill[fill_index] = K[c - 1] ? (1.0 / 0.0) : 1.0;
      else
        fill[fill_index] = ((double)K[c - 1] * r * c) / S;
      fill_index++;
    }
  }

  free(blocks);
  return 0;
}
