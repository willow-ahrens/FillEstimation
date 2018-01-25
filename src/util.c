#include <stdlib.h>
#include "util.h"
#include "mt19937ar.c"

void random_seed(unsigned long s) {
  init_genrand(s);
}

int random_range (int lo, int hi) {
  int n = hi - lo;
  while(1){
    unsigned long r = genrand_int32();
    int offset = (r % n);
    if (0xfffffffful - (r - offset) >= n - 1) return lo + offset;
  }
}

double random_uniform () {
  return genrand_res53();
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
