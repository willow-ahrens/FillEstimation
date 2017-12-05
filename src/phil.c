#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"

char *name () {
  return "phil";
}

#define REPLACEMENT

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
  size_t W = 2 * B;
  int Z[W][W];

  double T = 2 * log(B/delta) * B * B / (epsilon * epsilon);
  size_t s;

#ifdef REPLACEMENT
  s = T;
#else
  //s = ((T - T/nnz) + sqrt((T - T / nnz) * (T - T / nnz)  + 4 * T * (1 + T / N)))/(2 + 2 * T / nnz);
  s = ((T - T/nnz) + sqrt(T * (T + (2 * T + T / nnz) / nnz + 4)))/(2 + 2 * T / nnz);
#endif
  s = min(s, nnz);

  //Sample s items
  size_t *samples = (size_t*)malloc(s*sizeof(size_t));
  size_t *samples_i = (size_t*)malloc(s*sizeof(size_t));
  size_t *samples_j = (size_t*)malloc(s*sizeof(size_t));

#ifdef REPLACEMENT
  if (s == nnz) {
    for (size_t i = 0; i < nnz; i++) {
      samples[i] = i;
    }
  } else {
    for (size_t i = 0; i < s; i++) {
      samples[i] = random_range(0, nnz);
    }
  }
#else
  random_choose(samples, s, 0, nnz);
#endif

  //Create arrays of i and j
  sort(samples, s);
  {
    size_t i = 0;
    for (size_t t = 0; t < s; t++) {
      if (ptr[i + 1] <= samples[t]) {
        i = search_strict(ptr, i, m, samples[t]) - 1;
      }
      samples_i[t] = i;
      samples_j[t] = ind[samples[t]];
    }
  }

  for (size_t t = 0; t < s; t++) {
    size_t i = samples_i[t];
    size_t j = samples_j[t];

    //compute x for some i, j
    for (int r = 0; r < W; r++) {
      for (int c = 0; c < W; c++) {
        Z[r][c] = 0;
      }
    }

    for (size_t ii = max(i, B - 1) - (B - 1); ii <= min(i + (B - 1), m - 1); ii++) {
      int r = (B + ii) - i;
      size_t jj;
      size_t jj_min = max(j, B - 1) - (B - 1);
      size_t jj_max = min(j + (B - 1), n - 1);

      size_t scan = search(ind, ptr[ii], ptr[ii + 1], jj_min);

      while (scan < ptr[ii + 1] && (jj = ind[scan]) <= jj_max) {
        int c = (B + jj) - j;
        Z[r][c] = 1;
        scan++;
      }
    }

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

    int fill_index = 0;
    for (int b_r = 1; b_r <= B; b_r++) {
      int r_hi = B + b_r - 1 - (i % b_r);
      int r_lo = r_hi - b_r;
      for (int b_c = 1; b_c <= B; b_c++) {
        int c_hi = B + b_c - 1 - (j % b_c);
        int c_lo = c_hi - b_c;
        int y_0 = Z[r_hi][c_hi] - Z[r_lo][c_hi] - Z[r_hi][c_lo] + Z[r_lo][c_lo];
        fill[fill_index] += 1.0/y_0;
        fill_index++;
      }
    }
  }

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
