#include <stdio.h>
#include <stdlib.h>

char *name () {
  return "asx";
}

#ifndef max
    #define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef min
    #define min(a,b) ((a) < (b) ? (a) : (b))
#endif

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

  //initialize Z
  for (int r = 0; r < 2*B; r++) {
    for (int c = 0; c < 2*B; c++) {
      Z[r][c] = 0;
    }
  }

  size_t i = 4;
  size_t j = 4;

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

    size_t scan = ptr[ii];
    size_t len = ptr[ii + 1] - ptr[ii];
    size_t half;
    size_t mid;
    while (len > 0) {
      half = len >> 1;
      mid = scan + half;
      if (ind[mid] >= jj_min) {
        len = half;
      } else {
        scan = mid;
        scan++;
        len = len - half - 1;
      }
    }

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

  return 0;
}
