#include <stdio.h>
#include <stdlib.h>
#include "util.h"

char *name () {
  return "asx";
}

/**
 *  Given an m by n CSR matrix A, estimates the fill ratio if the matrix were
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
                   double *fill,
                   int verbose){
  size_t W = 2 * B;
  int Z[W][W];

  size_t s = 5000;
  s = min(s, nnz);

  //Sample K items without replacement
  size_t samples[s];
  size_t samples_i[s];
  size_t samples_j[s];
  random_choose(samples, s, 0, nnz);

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

    #if 1
      int fill_index = 0;
      for (int b_r = 1; b_r <= B; b_r++) {
        int r_hi = B + ((i + b_r - 1) % b_r);
        int r_lo = r_hi - b_r;
        for (int b_c = 1; b_c <= B; b_c++) {
          int c_hi = B + ((j + b_c - 1) % b_c);
          int c_lo = c_hi - b_c;
          int y_0 = Z[r_hi][c_hi] - Z[r_lo][c_hi] - Z[r_hi][c_lo] + Z[r_lo][c_lo];
          fill[fill_index] += 1.0/y_0;
          fill_index++;
        }
      }
    #else
      int fill_index = 0
      int Y_0[W];
      int c_hi[B + 1];
      int c_lo[B + 1];
      for (int b_c = 1; b_c <= B; b_c++) {
        c_hi[b_c] = B + ((j + b_c - 1) % b_c);
        c_lo[b_c] = c_hi[b_c] - b_c;
      }
      for (int b_r = 1; b_r <= B; b_r++) {
        int r_hi = B + ((i + b_r - 1) % b_r);
        int r_lo = r_hi - b_r;
        for (int c = 0; c < W; c++) {
          Y_0[c] = Z[r_hi][c] - Z[r_lo][c];
        }
        for (int b_c = 1; b_c <= B; b_c++) {
          int y_1 = Y_0[c_hi[b_c]] - Y_0[c_lo[b_c]];
          fill[fill_index] += 1.0/y_1;
          fill_index++;
        }
      }
    #endif
  }

  int fill_index = 0;
  for (int b_r = 1; b_r <= B; b_r++) {
    for (int b_c = 1; b_c <= B; b_c++) {
      fill[fill_index] *= b_r * b_c / (double)s;
      fill_index++;
    }
  }

  return 0;
}
