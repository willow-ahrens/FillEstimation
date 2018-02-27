#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"

char *name () {
  return "phil";
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
                   double sigma,
                   double *fill,
                   int verbose){
  assert(n >= 1);
  assert(m >= 1);
  int W = 2 * B;
  int Z[W][W];

  /* Compute the necessary number of samples */
  double T = log((2 * B * B) / delta) * B * B * B * B / (2.0 * epsilon * epsilon);
  int s;

  s = min(T, nnz);

  /* Sample s locations of nonzeros */
  int *samples = (int*)malloc(s*sizeof(int));
  assert(samples != NULL);
  int *samples_i = (int*)malloc(s*sizeof(int));
  assert(samples_i != NULL);
  int *samples_j = (int*)malloc(s*sizeof(int));
  assert(samples_j != NULL);

  /* if s == nnz, just compute the fill exactly. Otherwise, sample s nonzeros
   * so that the samples[t]^th nonzero is included in the sample.
   */
  if (s == nnz) {
    for (int t = 0; t < nnz; t++) {
      samples[t] = t;
    }
  } else {
    random_range(samples, s, 0, nnz);
  }

  /* Convert flat samples array to (i, j) pairs in samples_i and samples_j. */
  sort(samples, s);
  {
    int i = 0;
    for (int t = 0; t < s; t++) {
      if (ptr[i + 1] <= samples[t]) {
        i = search_strict(ptr, i, m, samples[t]) - 1;
      }
      samples_i[t] = i;
      samples_j[t] = ind[samples[t]];
    }
  }

  for (int t = 0; t < s; t++) {
    int i = samples_i[t];
    int j = samples_j[t];

    /* Fill Z with 0 */
    for (int r = 0; r < W; r++) {
      for (int c = 0; c < W; c++) {
        Z[r][c] = 0;
      }
    }

    /* Set Z to 1 where there are nonzeros in the neighborhood of (i, j) */
    for (int ii = max(i, B - 1) - (B - 1); ii <= min(i + (B - 1), m - 1); ii++) {
      int r = (B + ii) - i;
      int jj;
      int jj_min = max(j, B - 1) - (B - 1);
      int jj_max = min(j + (B - 1), n - 1);

      int scan = search(ind, ptr[ii], ptr[ii + 1], jj_min);

      while (scan < ptr[ii + 1] && (jj = ind[scan]) <= jj_max) {
        int c = (B + jj) - j;
        Z[r][c] = 1;
        scan++;
      }
    }

    /* These prefix sums set Z[r][c] to the number of nonzeros in the region
     * extending from (i - B + 1, j - B + 1) to (i - B + r, j - B + c) for all
     * r > 0, c > 0.
     */
    for (int r = 1; r < W; r++) {
      for (int c = 1; c < W; c++) {
        Z[r][c] += Z[r][c - 1];
      }
    }

    for (int r = 1; r < W; r++) {
      for (int c = 1; c < W; c++) {
        Z[r][c] += Z[r - 1][c];
      }
    }

    /* Using Z, compute the number of nonzeros in (i, j)'s block for each
     * desired block size.
     */
    int fill_index = 0;
    for (int b_r = 1; b_r <= B; b_r++) {
      int r_hi = B + b_r - 1 - (i % b_r);
      int r_lo = r_hi - b_r;
      for (int b_c = 1; b_c <= B; b_c++) {
        int c_hi = B + b_c - 1 - (j % b_c);
        int c_lo = c_hi - b_c;
        int y_0 = Z[r_hi][c_hi] - Z[r_lo][c_hi] - Z[r_hi][c_lo] + Z[r_lo][c_lo];
        /* Compute the average inverse of the number of nozeros in (i, j)'s
         * block.
         */
        fill[fill_index] += 1.0/y_0;
        fill_index++;
      }
    }
  }

  /* Compute the fill from the average inverses stored in fill array */
  int fill_index = 0;
  for (int b_r = 1; b_r <= B; b_r++) {
    for (int b_c = 1; b_c <= B; b_c++) {
      fill[fill_index] *= b_r * b_c / (double)s;
      fill_index++;
    }
  }

  free(samples);
  free(samples_i);
  free(samples_j);
  return 0;
}
