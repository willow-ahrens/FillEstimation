#include <gsl/gsl_rng.h>
#include <stdlib.h>
#include "util.h"

/*
#include <glib.h>
hash_t *hash_create () {
  return (char*) g_hash_table_new(g_direct_hash, g_direct_equal);
}

void hash_set (hash_t *table, size_t key, size_t value) {
  g_hash_table_replace((GHashTable*)table, (gpointer)key, (gpointer)value);
}

size_t hash_get (hash_t *table, size_t key, size_t value) {
  size_t orig_key;
  size_t result;
  if (g_hash_table_lookup_extended((GHashTable*)table, (gpointer)key, (gpointer*)&orig_key, (gpointer*)&result)) {
    return result;
  }
  return value;
}

void hash_clear (hash_t *table) {
  return g_hash_table_remove_all((GHashTable*)table);
}

void hash_destroy (hash_t *table) {
  return g_hash_table_destroy((GHashTable*)table);
}

void random_choose (size_t *samples, size_t n, size_t lo, size_t hi) {
  hash_t *table = hash_create();
  for (size_t i = 0; i < n; i++) {
    size_t j = random_range(lo, hi);
    samples[i] = hash_get(table, j, j);
    hash_set(table, j, hash_get(table, lo, lo));
    lo++;
  }
  hash_destroy(table);
}
*/

gsl_rng *r = 0;

size_t random_range (size_t lo, size_t hi) {
  if (!r) {
    gsl_rng_env_setup();
    const gsl_rng_type *T = gsl_rng_default;
    r = gsl_rng_alloc (T);
  }
  size_t n = hi - lo;
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

int size_t_cmp(const void *a, const void *b)
{
    const size_t *ia = (const size_t *)a;
    const size_t *ib = (const size_t *)b;
    return *ia  - *ib;
}

void quicksort(size_t *stuff, size_t n)
{
  if (n < 2) return;

  size_t pivot = stuff[n / 2];

  size_t i, j;
  for (i = 0, j = n - 1; ; i++, j--)
  {
    while (stuff[i] < pivot) i++;
    while (stuff[j] > pivot) j--;

    if (i >= j) break;

    size_t temp = stuff[i];
    stuff[i]     = stuff[j];
    stuff[j]     = temp;
  }

  quicksort(stuff, i);
  quicksort(stuff + i, n - i);
}

void sort (size_t *stuff, size_t n) {
  //quicksort(stuff, n);
  qsort(stuff, n, sizeof(size_t), size_t_cmp);
}

size_t search (const size_t *stuff, size_t lo, size_t hi, size_t key) {
  size_t len = hi - lo;
  size_t half;
  size_t mid;
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

size_t search_strict (const size_t *stuff, size_t lo, size_t hi, size_t key) {
  size_t len = hi - lo;
  size_t half;
  size_t mid;
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
