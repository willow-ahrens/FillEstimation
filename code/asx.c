#include <stdio.h>
#include <stdlib.h>
#include "util.h"

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
  size_t W = 2 * B;
  int Z[W][W];

  size_t s = 1000;
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

  //Create a shifted scaled fill array
  int fill_index = 0;
  for (int b_r = 1; b_r <= B; b_r++) {
    for (int b_c = 1; b_c <= B; b_c++) {
      fill_index += b_r * b_c;
    }
  }
  double ssfill[fill_index];
  fill_index = 0;
  for (int b_r = 1; b_r <= B; b_r++) {
    for (int b_c = 1; b_c <= B; b_c++) {
      for (int o_r = 0; o_r < b_r; o_r++) {
        for (int o_c = 0; o_c < b_c; o_c++) {
          ssfill[fill_index] = 0;
          fill_index++;
        }
      }
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

    fill_index = 0;
    int Y_0[B][W];
    int Y_1[B][B];
    for (int b_r = 1; b_r <= B; b_r++) {
      for (int r = 0; r < b_r; r++) {
        int o_r = (i + r) % b_r;
        int r_hi = B + r;
        int r_lo = r_hi - b_r;
        for (int c = 0; c < W; c++) {
          Y_0[o_r][c] = Z[r_hi][c] - Z[r_lo][c];
        }
      }
      for (int b_c = 1; b_c <= B; b_c++) {
        for (int o_r = 0; o_r < b_r; o_r++) {
          for (int c = 0; c < b_c; c++) {
            int o_c = ((j + c) % b_c);
            int c_hi = B + c;
            int c_lo = c_hi - b_c;
            Y_1[o_r][o_c] = Y_0[o_r][c_hi] - Y_0[o_r][c_lo];
          }
          for (int o_c = 0; o_c < b_c; o_c++) {
            ssfill[fill_index] += 1.0/Y_1[o_r][o_c];
            fill_index++;
          }
        }
      }
    }
  }

  fill_index = 0;
  for (int b_r = 1; b_r <= B; b_r++) {
    for (int b_c = 1; b_c <= B; b_c++) {
      double factor = b_r * b_c / (double)s;
      for (int r = 0; r < b_r; r++) {
        int o_r = (b_r - r - 1) % b_r;
        for (int c = 0; c < b_c; c++) {
          int o_c = (b_c - c - 1) % b_c;
          fill[fill_index + o_r * b_c + o_c] = ssfill[fill_index + r * b_c + c] * factor;
        }
      }
      fill_index += b_r * b_c;
    }
  }

  return 0;
}
