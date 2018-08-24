/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2017, Peter Ahrens All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <random>
#include <algorithm>

const char *name () {
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
                   std::seed_seq &seeder,
                   int verbose){
  assert(n >= 1);
  assert(m >= 1);
  int W = 2 * B;
  int Z[W][W];

  /* Compute the necessary number of samples */
  double T = log((2 * B * B) / delta) * B * B * B * B / (2.0 * epsilon * epsilon);
  int s;

  s = std::min((int)T, nnz);

  /* Sample s locations of nonzeros */
  int *samples = new int[s];
  assert(samples != NULL);
  int *samples_i = new int[s];
  assert(samples_i != NULL);
  int *samples_j = new int[s];
  assert(samples_j != NULL);

  /* Seed the random generator */

  std::mt19937 generator(seeder);

  /* if s == nnz, just compute the fill exactly. Otherwise, sample s nonzeros
   * so that the samples[t]^th nonzero is included in the sample.
   */
  if (s == nnz) {
    for (int t = 0; t < nnz; t++) {
      samples[t] = t;
    }
  } else {
    std::uniform_int_distribution<> range(0, nnz - 1);
    for (int t = 0; t < s; t++) {
      samples[t] = range(generator);
    }
  }

  /* Convert flat samples array to (i, j) pairs in samples_i and samples_j. */
  std::sort(samples, samples + s, std::less<int>());
  {
    int i = 0;
    for (int t = 0; t < s; t++) {
      if (ptr[i + 1] <= samples[t]) {
        i = (std::upper_bound(ptr + i, ptr + m, samples[t]) - ptr) - 1;
      }
      samples_i[t] = i;
      samples_j[t] = ind[samples[t]];
    }
  }

  /* Zero out the fill */
  int fill_index = 0;
  for (int b_r = 1; b_r <= B; b_r++) {
    for (int b_c = 1; b_c <= B; b_c++) {
      fill[fill_index] = 0.0;
      fill_index++;
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
    for (int ii = std::max(i, B - 1) - (B - 1); ii <= std::min(i + (B - 1), m - 1); ii++) {
      int r = (B + ii) - i;
      int jj;
      int jj_min = std::max(j, B - 1) - (B - 1);
      int jj_max = std::min(j + (B - 1), n - 1);

      int scan = (std::lower_bound(ind + ptr[ii], ind + ptr[ii + 1], jj_min) - ind);

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
  fill_index = 0;
  for (int b_r = 1; b_r <= B; b_r++) {
    for (int b_c = 1; b_c <= B; b_c++) {
      fill[fill_index] *= b_r * b_c / (double)s;
      fill_index++;
    }
  }

  delete[] samples;
  delete[] samples_i;
  delete[] samples_j;
  return 0;
}
