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
