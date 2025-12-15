#include <stdio.h>
#include <stdlib.h>
#include <kernel/memory/constants.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/vmm.h>
#include "aoc_common.h"

#define array_malloc(T) static T *malloc_##T##_array (const size_t n) { \
  const size_t bytes = sizeof(T) * n; \
  size_t pages_needed = bytes / PAGE_SIZE; \
  const size_t remainder = bytes % PAGE_SIZE; \
  if (remainder != 0) ++pages_needed; \
  const uintptr_t phys_page = (uintptr_t)pmm_alloc_pages(pages_needed); \
  return (T*)vmm_kmap(phys_page, bytes, 0, 0); \
}

typedef int coord_t;
typedef unsigned long int  area_t;

typedef struct Vec2 {
  coord_t x;
  coord_t y;
  coord_t z;
} Vec2;

array_malloc(Vec2)

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

static Vec2 parse_coord(const char *input, const char **endptr) {
  const char *tmp = input;
  Vec2 coord;
  coord.x = (coord_t)strtoull(tmp, &tmp, 10);
  ++tmp;
  coord.y = (coord_t)strtoull(tmp, &tmp, 10);
  if (*tmp == '\n') {
    ++tmp;
    *endptr = tmp;
    return coord;
  }
  // we clearly didn't parse a full line here
  coord.x = 0;
  coord.y = 0;
  *endptr = input;
  return coord;
}

static area_t compute_area(const Vec2 a, const Vec2 b) {
  const area_t dx = ((a.x > b.x) ? a.x - b.x : b.x - a.x) + 1;
  const area_t dy = ((a.y > b.y) ? a.y - b.y : b.y - a.y) + 1;
  return dx*dy;
}

int main() {
  const char *input = get_input();
  const size_t n_tiles = count_lines(input, true);
  printf("Input contains %zu tiles\n", n_tiles);

  Vec2 *coords = malloc_Vec2_array(n_tiles);
  for (size_t i = 0; i < n_tiles; ++i) {
    coords[i] = parse_coord(input, &input);
  }
  printf("Parsed coords!\n");

  area_t biggest_area = 0;
  for (size_t i = 0; i < n_tiles; ++i) {
    for (size_t j = i + 1; j < n_tiles; ++j) {
      area_t area = compute_area(coords[i], coords[j]);
      if (area > biggest_area) biggest_area = area;
    }
  }

  printf("Biggest area: %lu\n", biggest_area);

  return 0;
}
