#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#define offsets

char *name () {
  return "asx";
}

/**
 *  Given an m by n CSR matrix A, estimates the fill ratio if the matrix were
 *  converted into b_r by b_c (offset by o_r and o_c) BCSR format. The fill
 *  ratio is b_r times b_c times the number of nonzero blocks in the BCSR format
 *  divided by the number of nonzeros.
 *
 *  The caller supplies this routine with a maximum row and column block size B,
 *  and this routine returns the estimated fill ratios for all
 *  1 <= b_r, b_c <= B and 0 <= o_r < b_r and 0 <= o_c < b_c
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
 *  \param[out] *fill Fill ratios for all specified b_r, b_c, o_r, o_c in order
 *  \param[in] verbose 0 if you should be quiet
 *
 *  Note that the fill ratios should be stored according to the following order:
 *  size_t i = 0;
 *  for (size_t b_r = 1; b_r <= B; b_r++) {
 *    for (size_t b_c = 1; b_c <= B; b_c++) {
 *      for (size_t o_r = 0; o_r < b_r; o_r++) {
 *        for (size_t o_c = 0; o_c < b_c; o_c++) {
 *          //fill[i] = fill for b_r, b_c, o_r, o_c
 *          i++;
 *        }
 *      }
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
  int Z[2*B][2*B];
  int Y_1[B][2 * B];
  int Y_2[B][B];

  size_t s = 10000;

  //Sample K items without replacement
  s = min(s, nnz);
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
    for (int r = 1; r < 2*B; r++) {
      for (int c = 1; c < 2*B; c++) {
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

    for (int r = 1; r < 2*B; r++) {
      for (int c = 1; c < 2*B; c++) {
        Z[r][c] += Z[r][c - 1];
      }
    }

    for (int c = 1; c < 2*B; c++) {
      for (int r = 1; r < 2*B; r++) {
        Z[r][c] += Z[r - 1][c];
      }
    }

    size_t fill_index = 0;
#ifdef offsets
    for (size_t b_r = 1; b_r <= B; b_r++) {
      for (int r = B; r < B + b_r; r++) {
        size_t o_r = (i + r + 1 - B) % b_r;
        for (int c = 0; c < 2*B; c++) {
          Y_1[o_r][c] = Z[r][c] - Z[r - b_r][c];
        }
      }
      for (size_t b_c = 1; b_c <= B; b_c++) {
        for (int c = B; c < B + b_c; c++) {
          size_t o_c = (j + c + 1 - B) % b_c;
          for (size_t o_r = 0; o_r < b_r; o_r++) {
            Y_2[o_r][o_c] = Y_1[o_r][c] - Y_1[o_r][c - b_c];
          }
        }
        for (size_t o_r = 0; o_r < b_r; o_r++) {
          for (size_t o_c = 0; o_c < b_c; o_c++) {
            fill[fill_index] += 1.0/Y_2[o_r][o_c];
            fill_index++;
          }
        }
      }
    }
#else
    for (size_t b_r = 1; b_r <= B; b_r++) {
      for (size_t b_c = 1; b_c <= B; b_c++) {
        size_t r_hi = B + ((i + b_r - 1) % b_r);
        size_t r_lo = r_hi - b_r;
        size_t c_hi = B + ((j + b_c - 1) % b_c);
        size_t c_lo = c_hi - b_c;
        size_t y = Z[r_hi][c_hi] - Z[r_hi][c_lo] - Z[r_lo][c_hi] + Z[r_lo][c_lo];
        fill[fill_index] += 1.0/y;
        fill_index++;
      }
    }
#endif
  }

  return 0;
}
