#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <random>

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
                   int verbose);

int test (int m,
          int n,
          int nnz,
          const int *ptr,
          const int *ind,
          int B,
          double epsilon,
          double delta,
          double sigma,
          int trials,
          int clock,
          int results,
          long seed,
          int verbose) {

  double *fill = (double*)malloc(sizeof(double) * B * B * trials);
  for (int i = 0; i < B * B * trials; i++) {
    fill[i] = 0;
  }

  std::seed_seq seeder{seed};

  //Load problem into cache
  estimate_fill(m, n, nnz, ptr, ind, B, epsilon, delta, sigma, fill, seeder, verbose);
  for (int i = 0; i < B * B; i++) {
    fill[i] = 0;
  }

  //Benchmark some runs
  auto tic = std::chrono::high_resolution_clock::now();
  for (int t = 0; t < trials; t++){
    estimate_fill(m, n, nnz, ptr, ind, B, epsilon, delta, sigma, fill + t * B * B, seeder, verbose);
  }
  auto toc = std::chrono::high_resolution_clock::now();
  auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(toc-tic);
  double time = diff.count() * 1e-9;

  printf("{\n");
  int i = 0;
  if (results) {
    printf("  \"results\": [\n");
    for (int t = 0; t < trials; t++) {
      printf("    [\n");
      for (int b_r = 1; b_r <= B; b_r++) {
        printf("      [\n");
        for (int b_c = 1; b_c <= B; b_c++) {
          printf("%.*e%s", DECIMAL_DIG, fill[i], b_c <= B - 1 ? ", " : "");
          i++;
        }
        printf("      ]%s\n", b_r <= B - 1 ? "," : "");
      }
      printf("    ]%s\n", t < trials - 1 ? "," : "");
    }
    printf("  ]%s\n", clock ? "," : "");
  }
  if (clock) {
    printf("  \"total_time\": %.*e,\n", DECIMAL_DIG, time);
    printf("  \"mean_time\": %.*e%s\n", DECIMAL_DIG, time/trials, 0 ? "," : "");
  }
  printf("\n}\n");

  free(fill);
  return 0;
}
