#include <gsl/gsl_rng.h>
#include <stdlib.h>
#include "util.h"

gsl_rng *r = 0;

int random_range (int lo, int hi) {
  if (!r) {
    gsl_rng_env_setup();
    const gsl_rng_type *T = gsl_rng_default;
    r = gsl_rng_alloc (T);
  }
  int n = hi - lo;
  return lo + gsl_rng_uniform_int(r, n);//todo this range might not be big enough/type issues may occur here.
}

double random_uniform () {
  if (!r) {
    gsl_rng_env_setup();
    const gsl_rng_type *T = gsl_rng_default;
    r = gsl_rng_alloc (T);
  }
  return gsl_rng_uniform(r);
}

int int_cmp(const void *a, const void *b)
{
    const int *ia = (const int *)a;
    const int *ib = (const int *)b;
    return *ia  - *ib;
}

void quicksort(int *stuff, int n)
{
  if (n < 2) return;

  int pivot = stuff[n / 2];

  int i, j;
  for (i = 0, j = n - 1; ; i++, j--)
  {
    while (stuff[i] < pivot) i++;
    while (stuff[j] > pivot) j--;

    if (i >= j) break;

    int temp = stuff[i];
    stuff[i]     = stuff[j];
    stuff[j]     = temp;
  }

  quicksort(stuff, i);
  quicksort(stuff + i, n - i);
}

void sort (int *stuff, int n) {
  //quicksort(stuff, n);
  qsort(stuff, n, sizeof(int), int_cmp);
}

int search (const int *stuff, int lo, int hi, int key) {
  int len = hi - lo;
  int half;
  int mid;
  while (len > 0) {
    half = len >> 1;
    mid = lo + half;
    if (stuff[mid] >= key) {
      len = half;
    } else {
      lo = mid;
      lo++;
      len = len - half - 1;
    }
  }
  return lo;
}

int search_strict (const int *stuff, int lo, int hi, int key) {
  int len = hi - lo;
  int half;
  int mid;
  while (len > 0) {
    half = len >> 1;
    mid = lo + half;
    if (stuff[mid] > key) {
      len = half;
    } else {
      lo = mid;
      lo++;
      len = len - half - 1;
    }
  }
  return lo;
}
