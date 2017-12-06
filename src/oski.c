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
  int r;
  size_t *nb_est;
  int err;
  int j = 0;

  nb_est = malloc(sizeof(size_t) * B);
  if (nb_est == NULL)
    return -1;

  for (r = 1; r <= B; r++) {
    int c;

    memset(nb_est, 0, sizeof(size_t) * B);

    /* block dimensions */
    size_t M;

    /* stores total number of non-zero blocks */
    size_t S = 0;

    /* auxiliary storage: reused for each block-row */
    size_t *block_count;	/* size N */
    size_t I;		/* block-row iteration variable */


    M = m / r;		/* # of full block-rows */

    if (n == 0) {
      S = 0;
      break;	/* Quick return */
    }
    /*
     * ---------------------------------------------------- Allocate
     * temporary space.
     */

    assert(n >= 1);
    block_count = malloc(sizeof(size_t) * B * n);
    if (block_count == NULL) {
      return -1;
    }
    memset(block_count, 0, sizeof(size_t) * B * n);

    /** Get the block count for block column size c, block column J. */
    #define GET_BC(A, c, J) (A)[((c)-1)*n + (J)]
    /** Increment the block count for block column size c, block column J. */
    #define INC_BC(A, c, J) (A)[((c)-1)*n + (J)]++
    /** Set the block count for block column size c, block column J, to zero. */
    #define ZERO_BC(A, c, J) (A)[((c)-1)*n + (J)] = 0

    /*
     * ---------------------------------------------------- Phase I:
     * Count the number of new blocks to create.
     */
    memset(nb_est, 0, sizeof(size_t) * B);
    S = 0;
    for (I = 0; I < M; I++) {	/* loop over block rows */
      size_t i;
      size_t di;

      if (random_uniform() > delta){
        continue;	/* skip this block row */
      }else{

        /*
         * Count the number of blocks within block-row I, and
         * remember in 'block_count' which of the possible blocks
         * have been 'visited' (i.e., contain at least 1 non-zero).
         */
        for (i = I * r, di = 0; di < r; di++, i++) {
          size_t k;

          /*
           * Invariant: block_count[J] == # of non-zeros
           * encountered in rows I*r .. I*r+di that should be
           * stored in column-block J (i.e., that have column
           * indices J*c <= j < (J+1)*c).
           */

          /*
           * Count the number of additional logical blocks
           * needed to store non-zeros in row i, and mark the
           * blocks in block row I that have been visited.
           */
          for (k = ptr[i]; k < ptr[i + 1]; k++) {
            size_t j = ind[k];	/* column index */
            size_t c;

            for (c = 1; c <= B; c++) {
              size_t J = j / c;	/* block column index */

              if (GET_BC(block_count, c, J) == 0) {
                /* "create" (count) new block */
                INC_BC(block_count, c, J);
                nb_est[c - 1]++;
              }
            }
          }
        }
      }
      S += ptr[i] - ptr[I * r];

      /* POST: S == total # of non-zeros examined so far */
      /* POST: num_blocks == total # of new blocks in rows 0..i */
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
        size_t k;

        for (k = ptr[i]; k < ptr[i + 1]; k++) {
          size_t j = ind[k];	/* column index */
          size_t c;

          for (c = 1; c <= B; c++) {
            size_t J = j / c;	/* block column index */
            ZERO_BC(block_count, c, J);
          }
        }
      }
    }
    /* POST: num_blocks == total # of blocks in examined rows. */
    /* POST: S == total # of non-zeros in examined rows. */

    free(block_count);

    if (err) {
      free(nb_est);
      return err;
    }
    for (c = 1; c <= B; c++) {
      size_t nb_nnz = nb_est[c - 1] * r * c;
      double ratio;
      if (!S)
        ratio = nb_nnz ? (1.0 / 0.0) : 1.0;
      else
        ratio = (double)nb_nnz / S;
      fill[j] = ratio;
      j++;
    }
  }

  free(nb_est);
  return 0;
}
