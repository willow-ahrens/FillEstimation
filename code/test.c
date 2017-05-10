#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define TIMEOUT 0.1

//Return time time of day as a double-precision floating point value.
double wall_time (void) {
  struct timeval t;
  gettimeofday(&t, NULL);
  return 1.0*t.tv_sec + 1.0e-6*t.tv_usec;
}

int estimate_fill (size_t m,
                   size_t n,
                   size_t nnz,
                   const size_t *ptr,
                   const size_t *ind,
                   size_t B,
                   double *fill,
                   int verbose);

int test (size_t m,
          size_t n,
          size_t nnz,
          const size_t *ptr,
          const size_t *ind,
          size_t B,
          int verbose) {

  size_t nfill = 0;
  for (int b_r = 1; b_r <= B; b_r++) {
    for (int b_c = 1; b_c <= B; b_c++) {
      for (int o_r = 0; o_r < b_r; o_r++) {
        for (int o_c = 0; o_c < b_c; o_c++) {
          nfill++;
        }
      }
    }
  }

  double *fill = (double*)malloc(sizeof(double) * nfill);

  double test_time = 0;
  int trials = 0;

  //We must run the benchmarking application for a sufficient length of time
  //to avoid small variations in processing speed. We do this by running an
  //increasing number of trials until it takes at least TIMEOUT seconds.
  for (trials = 1; test_time < TIMEOUT; trials *= 2) {

    //Unless you want to measure the cache warm-up time, it is usually a good
    //idea to run the problem for one iteration first to load the problem
    //into cache.
    for (size_t i = 0; i < nfill; i++) {
      fill[i] = 0;
    }
    estimate_fill (m, n, nnz, ptr, ind, B, fill, verbose);

    //Benchmark "trials" runs.
    test_time = -wall_time();
    for (int i = 0; i < trials - 1; ++i){
      for (size_t i = 0; i < nfill; i++) {
        fill[i] = 0;
      }
      estimate_fill (m, n, nnz, ptr, ind, B, fill, verbose);
    }
    test_time += wall_time();
  }
  trials /= 2;
  test_time /= trials;

  return 0;
}


