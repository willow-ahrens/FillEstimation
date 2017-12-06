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
 *  nonzeros. All estimates should be accurate to relative error epsilon with
 *  probability at least (1 - delta).
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
 *  \param[out] *fill Fill ratios for all specified b_r, b_c in order
 *  \param[in] verbose 0 if you should be quiet
 *
 *  Note that the fill ratios should be stored according to the following order:
 *  size_t i = 0;
 *  for (size_t b_r = 1; b_r <= B; b_r++) {
 *    for (size_t b_c = 1; b_c <= B; b_c++) {
 *      //fill[i] = fill for b_r, b_c, o_r, o_c
 *      i++;
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
  assert(n >= 1);
  assert(m >= 1);

  /* blocks + (c - 1) * n stores previously seen column block indicies in the
    current block row when b_c = c */
  int *blocks = (int*)malloc(sizeof(int) * B * n);
  assert(blocks != NULL);
  memset(blocks, 0, sizeof(int) * B * n);

  /* K[(c - 1)] counts distinct column block indicies in the current block row
    when b_c = c */
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

    for (size_t I = 0; I < M; I++) { /* loop over block rows */
      size_t i;
      size_t di;

      if (random_uniform() > delta){
        continue;  /* skip this block row */
      }else{

        /*
         * Count the number of blocks within block-row I, and
         * remember in 'block_count' which of the possible blocks
         * have been 'visited' (i.e., contain at least 1 non-zero).
         */
        for (i = I * r, di = 0; di < r; di++, i++) {
          /*
           * Count the number of additional logical blocks
           * needed to store non-zeros in row i, and mark the
           * blocks in block row I that have been visited.
           */
          for (size_t t = ptr[i]; t < ptr[i + 1]; t++) {
            size_t j = ind[t];  /* column index */

            for (int c = 1; c <= B; c++) {
              size_t J = j / c;  /* block column index */

              if (blocks[(c - 1) * n + J] == 0) {
                /* "create" (count) new block */
                blocks[(c - 1) * n + J] = 1;
                K[c - 1]++;
              }
            }
          }
        }
      }
      S += ptr[i] - ptr[I * r];

      /* POST: S == total # of non-zeros examined so far */
      /* POST: K == total # of new blocks in rows 0..i */
      /*
       * POST: block_count[c,J] == # of non-zeros in block J of
       * block-row I
       */

      /*
       * Reset block_count for next iteration, I+1. This second
       * loop is needed to keep the complexity of phase I to
       * O(nnz).
       */
      for (i = I * r, di = 0; di < r; di++, i++) {

        for (size_t t = ptr[i]; t < ptr[i + 1]; t++) {
          size_t j = ind[t];  /* column index */

          for (int c = 1; c <= B; c++) {
            size_t J = j / c;  /* block column index */
            blocks[(c - 1) * n + J] = 0;
          }
        }
      }
    }
    /* POST: num_blocks == total # of blocks in examined rows. */
    /* POST: S == total # of non-zeros in examined rows. */

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
