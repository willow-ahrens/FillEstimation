#include <stdlib.h>
#include <random>

extern "C" {

#include "util.h"

std::random_device rd;
std::mt19937 eng(rd());

void random_seed(unsigned long s) {
  eng.seed(s);
}

int random_range (int* stuff, int n, int lo, int hi) {
  std::uniform_int_distribution<int> intdistr(lo, hi - 1);
  for (int i = 0; i < n; i++){
    stuff[i] = intdistr(eng);
  }
}

std::uniform_real_distribution<double> fldistr(0.0, 1.0);
double random_uniform () {
  return fldistr(eng);
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

}
