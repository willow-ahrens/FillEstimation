#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include "taco.h"
#include <sys/time.h>

using namespace taco;

#define TIMEOUT 0.1

//Return time time of day as a double-precision floating point value.
double wall_time (void) {
  struct timeval t;
  gettimeofday(&t, NULL);
  return 1.0*t.tv_sec + 1.0e-6*t.tv_usec;
}

extern "C" {
int test (int m,
          int n,
          int r,
          int c,
          int nnz,
          const int *ind,
          const int *ptr,
          const double *data,
          int trials,
          int verbose) {

  Format  csr({Dense,Sparse});
  Format bcsr({Dense,Sparse,Dense,Dense});
  Format  bdv({Dense,Dense});
  Format   dv({Dense});

  int bm = (m + r - 1)/r;
  int bn = (n + c - 1)/c;
  int mm = bm * r;
  int nn = bn * c;

  // Create tensors
  Tensor<double> A({bm, bn, r, c}, bcsr);
  Tensor<double> b({bm, r},   bdv);
  Tensor<double> x({bn, c},   bdv);

  Tensor<double> Ap({m, n}, csr);
  Tensor<double> bp({m}, dv);
  Tensor<double> xp({n}, dv);

  for(int h = 0; h < nnz; h++){
    A.insert({ind[h]/r, ptr[h]/c, ind[h]%r, ptr[h]%c}, data[h]);
  }

  for(int h = 0; h < nnz; h++){
    Ap.insert({ind[h], ptr[h]}, data[h]);
  }

  for(int h = 0; h < nn; h++){
    x.insert({h/c, h%c}, 1.0);
  }

  for(int h = 0; h < n; h++){
    xp.insert({h}, 1.0);
  }

  A.pack();
  Ap.pack();

  x.pack();
  xp.pack();

  // Form a matrix-vector multiplication expression
  IndexVar i, j, k, l;
  b(i, k) = A(i, j, k, l) * x(j,l);
  bp(i) = Ap(i, j) * xp(j);

  // Compile the expression
  b.compile();
  b.assemble();
  bp.compile();
  bp.assemble();

  double mytime;

  if (r == 1 && c == 1) {
    //Load problem into cache
    bp.compute();

    //Benchmark some runs
    mytime = -wall_time();
    for (int t = 0; t < trials; t++){
      bp.compute();
    }
    mytime += wall_time();
  } else {
    //Load problem into cache
    b.compute();

    //Benchmark some runs
    mytime = -wall_time();
    for (int t = 0; t < trials; t++){
      b.compute();
    }
    mytime += wall_time();
  }

  printf("{\n");
  printf("  \"total_time\": %.*e,\n", DECIMAL_DIG, mytime);
  printf("  \"mean_time\": %.*e%s\n", DECIMAL_DIG, mytime/trials, 0 ? "," : "");
  printf("\n}\n");

  return 0;
}
}
