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

int test (size_t m,
	  size_t n,
	  size_t r,
	  size_t c,
	  size_t nnz,
	  const size_t *ind,
	  const size_t *ptr,
	  const double *data,
          int trials,
          int verbose) {

  Format  csr({Dense,Sparse});
  Format bcsr({Dense,Sparse,Dense,Dense});
  Format  bdv({Dense,Dense});
  Format   dv({Dense});

  size_t bm = m/r;
  size_t bn = n/c;
  size_t mm = bm * r;
  size_t nn = bn * c;
;
  // Create tensors
  Tensor<double> A({(int)bm, (int)bn, (int)r, (int)c}, bcsr);
  Tensor<double> b({(int)bm,(int)r},   bdv);
  Tensor<double> x({(int)bn,(int)c},     bdv);

  Tensor<double> Ap({(int)m, (int)n}, csr);
  Tensor<double> bp({(int)m}, dv);
  Tensor<double> xp({(int)n}, dv);

  for(size_t h = 0; h < nnz; h++){
    A.insert({(int)(ind[h]/r),
              (int)(ptr[h]/c),
              (int)(ind[h]%r),
              (int)(ptr[h]%c)}, data[h]);
  }

  for(size_t h = 0; h < nnz; h++){
    Ap.insert({(int)(ind[h]),
               (int)(ptr[h])}, data[h]);
  }

  for(size_t h = 0; h < nn; h ++){
    x.insert({(int)(h/c),
              (int)(h%c)}, 1.0);
  }

  for(size_t h = 0; h < n; h ++){
    xp.insert({(int)h}, 1.0);
  }

  A.pack();
  Ap.pack();

  x.pack();
  xp.pack();

  // Form a matrix-vector multiplication expression
  IndexVar i, j, k, l;
  b(i, k) = A(i,j,k,l) * x(j,l);
  bp(i) = Ap(i,j) * xp(j);

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
  printf("  \"time_total\": %.*e,\n", DECIMAL_DIG, time);
  printf("  \"time_mean\": %.*e%s\n", DECIMAL_DIG, mytime/trials, 0 ? "," : "");
  printf("\n}\n");

  return 0;
}
