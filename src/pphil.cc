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
#include <omp.h>

const char *name () {
  return "phil";
}

int chunk(int n, int p, int q){
  return q * (n/p) + std::min(q, n % p);
}

int chunk_lower(int n, int p, int q){
  return chunk(n, p, q);
}

int chunk_upper(int n, int p, int q){
  return chunk(n, p, q + 1);
}

int chunk_size(int n, int p, int q){
  return chunk_upper(n, p, q) - chunk_lower(n, p, q);
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
                   long seed,
                   int trial,
                   int verbose){
  int p = omp_get_max_threads();
  assert(n >= 1);
  assert(m >= 1);
  int W = 2 * B;

  /* Compute the necessary number of samples */
  double T = log((2 * B * B) / delta) * B * B * B * B / (2.0 * epsilon * epsilon);
  int s;

  s = std::min((int)T, nnz);

  /* Seed the random generator */

  std::seed_seq seeder{seed, (long)trial};
  std::mt19937 generator(seeder);

  /* Stratify the samples */

  int s_p[p];
  if (s == nnz) {
    for (int q = 0; q < p; q++){
      s_p[q] = chunk_size(nnz, p, q);
    }
  } else {
    s_p[p - 1] = s;
    for (int q = 0; q < p - 1; q++){
      s_p[q] = std::binomial_distribution<int>(s_p[p - 1], ((double)chunk_size(nnz, p, q))/(nnz - chunk_lower(nnz, p, q)))(generator);
      s_p[p - 1] -= s_p[q];
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

  #pragma omp parallel
  {
    int q = omp_get_thread_num();
    int my_s = s_p[q];
    std::seed_seq my_seeder{seed, (long)trial, (long)q};
    std::mt19937 my_generator(my_seeder);

    /* Sample s locations of nonzeros */
    int *my_samples = new int[my_s];
    assert(my_samples != NULL);
    int *my_samples_i = new int[my_s];
    assert(my_samples_i != NULL);
    int *my_samples_j = new int[my_s];
    assert(my_samples_j != NULL);

    /* Private working memory */
    int Z[W][W];
    double my_fill[B * B];
    int my_fill_index = 0;
    for (int r = 0; r < B; r++){
      for (int c = 0; c < B; c++){
        my_fill[my_fill_index] = 0.0;
        my_fill_index++;
      }
    }

    /* if s == nnz, just compute the fill exactly. Otherwise, sample s nonzeros
     * so that the samples[t]^th nonzero is included in the sample.
     */
    if (s == nnz) {
      for (int t = 0; t < my_s; t++) {
        my_samples[t] = t + chunk_lower(nnz, p, q);
      }
    } else {
      std::uniform_int_distribution<> range(chunk_lower(nnz, p, q), chunk_upper(nnz, p, q) - 1);
      for (int t = 0; t < my_s; t++) {
        my_samples[t] = range(my_generator);
      }
    }

    /* Convert flat samples array to (i, j) pairs in samples_i and samples_j. */
    std::sort(my_samples, my_samples + my_s, std::less<int>());
    {
      int i = 0;
      for (int t = 0; t < my_s; t++) {
        if (ptr[i + 1] <= my_samples[t]) {
          i = (std::upper_bound(ptr + i, ptr + m, my_samples[t]) - ptr) - 1;
        }
        my_samples_i[t] = i;
        my_samples_j[t] = ind[my_samples[t]];
      }
    }

    for (int t = 0; t < my_s; t++) {
      int i = my_samples_i[t];
      int j = my_samples_j[t];

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
      my_fill_index = 0;
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
          my_fill[my_fill_index] += 1.0/y_0;
          my_fill_index++;
        }
      }
    }

    delete[] my_samples;
    delete[] my_samples_i;
    delete[] my_samples_j;

    #pragma omp critical
    {
      /* Add personal fill contribution */
      my_fill_index = 0;
      for (int b_r = 1; b_r <= B; b_r++) {
        for (int b_c = 1; b_c <= B; b_c++) {
          fill[my_fill_index] += my_fill[my_fill_index];
          my_fill_index++;
        }
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
  return 0;
}
