#include <stdio.h>
#include <stdlib.h>
#include <kernel/memory/constants.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/vmm.h>
#include "aoc_common.h"

#define N_MERGES (1000)

typedef int coord_t;
typedef unsigned long long int  dist_t;

typedef struct Vec3 {
  coord_t x;
  coord_t y;
  coord_t z;
} Vec3;

static Vec3 *malloc_vec3_array(const size_t n) {
  const size_t bytes = sizeof(Vec3) * n;
  size_t pages_needed = bytes / PAGE_SIZE;
  const size_t remainder = bytes % PAGE_SIZE;
  if (remainder != 0) ++pages_needed;

  const uintptr_t phys_page = (uintptr_t)pmm_alloc_pages(pages_needed);
  return (Vec3*)vmm_kmap(phys_page, bytes, 0, 0);
}

static dist_t *malloc_dist_array(const size_t n) {
  const size_t bytes = sizeof(dist_t) * n;
  size_t pages_needed = bytes / PAGE_SIZE;
  const size_t remainder = bytes % PAGE_SIZE;
  if (remainder != 0) ++pages_needed;

  const uintptr_t phys_page = (uintptr_t)pmm_alloc_pages(pages_needed);
  return (dist_t*)vmm_kmap(phys_page, bytes, 0, 0);
}

static size_t *malloc_sizet_array(const size_t n) {
  const size_t bytes = sizeof(size_t) * n;
  size_t pages_needed = bytes / PAGE_SIZE;
  const size_t remainder = bytes % PAGE_SIZE;
  if (remainder != 0) ++pages_needed;

  const uintptr_t phys_page = (uintptr_t)pmm_alloc_pages(pages_needed);
  return (size_t*)vmm_kmap(phys_page, bytes, 0, 0);
}

static size_t count_lines(const char *input, bool ignore_empty_last_line) {
  if (input[0] == 0) return 0;
  size_t lines = 1;
  while (input[0] != 0) {
    if (input[0] == '\n') ++lines;
    ++input;
  }
  if (input[-1] == '\n' && ignore_empty_last_line) --lines;
  return lines;
}

static Vec3 parse_coord(const char *input, const char **endptr) {
  const char *tmp = input;
  Vec3 coord;
  coord.x = (coord_t)strtoull(tmp, &tmp, 10);
  ++tmp;
  coord.y = (coord_t)strtoull(tmp, &tmp, 10);
  ++tmp;
  coord.z = (coord_t)strtoull(tmp, &tmp, 10);
  if (*tmp == '\n') {
    ++tmp;
    *endptr = tmp;
    return coord;
  }
  // we clearly didn't parse a full line here
  coord.x = 0;
  coord.y = 0;
  coord.z = 0;
  *endptr = input;
  return coord;
}

static dist_t compute_distance(const Vec3 v1, const Vec3 v2) {
  const dist_t x = (v1.x > v2.x) ? v1.x - v2.x : v2.x - v1.x;
  const dist_t y = (v1.y > v2.y) ? v1.y - v2.y : v2.y - v1.y;
  const dist_t z = (v1.z > v2.z) ? v1.z - v2.z : v2.z - v1.z;
  return x*x + y*y + z*z;
}

static size_t get_condensed_index(size_t i, size_t j, const size_t n) {
  // https://stackoverflow.com/a/36867493
  if (i < j) {
    const size_t k = j;
    j = i;
    i = k;
  }
  return n*j - j*(j+1)/2 + i - 1 - j;
}

static dist_t get_dist(const dist_t *m, const size_t i, const size_t j, const size_t n) {
  if (i == j) return 0;
  const size_t index = get_condensed_index(i, j, n);
  return m[index];
}

static dist_t find_shortest_distance(const size_t n, const dist_t *m, size_t *i, size_t *j) {
  size_t z = 0;
  dist_t closest = (1ULL<<(sizeof(dist_t)*8-1)) - 1;
  if (i != NULL) *i = -1;
  if (j != NULL) *j = -1;
  for (size_t I = 0; I < n; ++I) {
    for (size_t J = I + 1; J < n; ++J) {
      dist_t d = m[z++];
      if (d > 0 && d < closest) {
        if (i != NULL) *i = I;
        if (j != NULL) *j = J;
        closest = d;
      }
    }
  }
  return closest;
}

int main() {
  const char *input = get_input();
  const size_t n_lights = count_lines(input, true);
  printf("Input contains %zu lights\n", n_lights);

  Vec3 *coords = malloc_vec3_array(n_lights);
  for (size_t i = 0; i < n_lights; ++i) {
    coords[i] = parse_coord(input, &input);
  }
  printf("Parsed coords!\n");

  // compute the reduced distance array
  const size_t combinations = n_lights*(n_lights-1)/2;
  dist_t *distances = malloc_dist_array(combinations);
  size_t z = 0;
  for (size_t i = 0; i < n_lights; ++i) {
    for (size_t j = i + 1; j < n_lights; ++j) {
      distances[z++] = compute_distance(coords[i], coords[j]);
    }
  }
  printf("Calculated distance matrix\n");

  // allocate and initialise the connection array
  size_t *group = malloc_sizet_array(n_lights);
  for (size_t i = 0; i < n_lights; ++i) {
    group[i] = i;
  }

  // start clustering
  size_t i; size_t j;
  for (size_t merge = 0; merge < N_MERGES; ++merge) {
    find_shortest_distance(n_lights, distances, &i, &j);

    // i and j now contain the two closest lights
    // merge them
    size_t new_group = group[i];
    size_t old_group = group[j];
    for (size_t k = 0; k < n_lights; ++k) {
      if (group[k] == old_group) {
        // put them in the same group and invalidate the distance
        // between them
        group[k] = new_group;
        distances[get_condensed_index(i, j, n_lights)] = 0;
      }
    }
  }
  printf("Finished clustering!\n");

  // count the groups
  size_t *group_counts = malloc_sizet_array(n_lights);
  for (size_t i = 0; i < n_lights; ++i) {
    group_counts[i] = 0;
  }
  for (size_t i = 0; i < n_lights; ++i) {
    ++group_counts[group[i]];
  }

  // calculate the required result
  unsigned long long int total = 1;
  for (size_t N = 0; N < 3; ++N) {
    long long int biggest = -1;
    size_t biggest_i = 0;
    for (size_t i = 0; i < n_lights; ++i) {
      if ((long long int)group_counts[i] > biggest) {
        biggest = group_counts[i];
        biggest_i = i;
      }
    }
    if (biggest < 1) break;
    group_counts[biggest_i] = 0;
    total *= biggest;
  }

  printf("Total: %llu\n", total);

  return 0;
}
