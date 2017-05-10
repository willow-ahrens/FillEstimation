#include <float.h>
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
          int trials,
          int clock,
          int results,
          int verbose) {

  size_t nfill = 0;
  for (size_t b_r = 1; b_r <= B; b_r++) {
    for (size_t b_c = 1; b_c <= B; b_c++) {
      for (size_t o_r = 0; o_r < b_r; o_r++) {
        for (size_t o_c = 0; o_c < b_c; o_c++) {
          nfill++;
        }
      }
    }
  }

  double *fills = (double*)malloc(sizeof(double) * nfill * trials);

  //Load problem into cache
  for (size_t i = 0; i < nfill; i++) {
    fills[i] = 0;
  }
  estimate_fill(m, n, nnz, ptr, ind, B, fills, verbose);

  //Zero the output again
  for (size_t i = 0; i < nfill * trials; i++) {
    fills[i] = 0;
  }

  //Benchmark some runs
  double time = -wall_time();
  for (int t = 0; t < trials; t++){
    estimate_fill(m, n, nnz, ptr, ind, B, fills + t * nfill, verbose);
  }
  time += wall_time();

  printf("{\n");
  size_t i = 0;
  if (results) {
    printf("  \"output\": [\n");
    for (int t = 0; t < trials; t++) {
      printf("    [\n");
      for (size_t b_r = 1; b_r <= B; b_r++) {
        printf("      [\n");
        for (size_t b_c = 1; b_c <= B; b_c++) {
          printf("        [\n");
          for (size_t o_r = 0; o_r < b_r; o_r++) {
            printf("          [");
            for (size_t o_c = 0; o_c < b_c; o_c++) {
              printf("%.*e, ", DECIMAL_DIG, fills[i]);
              i++;
            }
            printf("],\n");
          }
          printf("        ],\n");
        }
        printf("      ],\n");
      }
      printf("    ],\n");
    }
    printf("  ],\n");
  }
  if (clock) {
    printf("  \"time_total\": %.*e,\n", DECIMAL_DIG, time);
    printf("  \"time_mean\": %.*e,\n", DECIMAL_DIG, time/trials);
  }
  printf("}\n");

  return 0;
}
