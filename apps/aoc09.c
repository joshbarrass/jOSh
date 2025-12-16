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

typedef struct Edge {
  const Vec2 *a;
  const Vec2 *b;
} Edge;

array_malloc(Vec2);
array_malloc(Edge);

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

static inline area_t compute_dx(const Vec2 a, const Vec2 b) {
  return ((a.x > b.x) ? a.x - b.x : b.x - a.x);
}

static inline area_t compute_dy(const Vec2 a, const Vec2 b) {
  return ((a.y > b.y) ? a.y - b.y : b.y - a.y);
}

static area_t compute_area(const Vec2 a, const Vec2 b) {
  const area_t dx = compute_dx(a, b) + 1;
  const area_t dy = compute_dy(a, b) + 1;
  return dx*dy;
}

static bool coord_lies_on_edge(const Vec2 coord, const Edge e) {
  if (e.a->x == e.b->x) {
    // edge runs vertically
    if (coord.x != e.a->x) return false;
    if (e.a->y > e.b->y) {
      return (coord.y >= e.b->y && coord.y <= e.a->y);
    }
    return (coord.y >= e.a->y && coord.y <= e.b->y);
  } // else {
  // edge runs horizontally
  if (coord.y != e.a->y) return false;
  if (e.a->x > e.b->x) {
    return (coord.x >= e.b->x && coord.x <= e.a->x);
  }
  return (coord.x >= e.a->x && coord.x <= e.b->x);
}

static bool ray_intersects_edge(const Vec2 start, const Edge e) {
  // We will cast a ray along the x axis to the right. If the edge is
  // horizontal, we should always ignore it.
  if (e.a->y == e.b->y) return false;

  // If the edge is left of the ray's start position, it cannot ever
  // intersect
  if (start.x > e.a->x) return false;

  // finally, we need the start position to be in between the edge
  // endpoints
  if (e.a->y > e.b->y) return (start.y >= e.b->y && start.y < e.a->y);
  return (start.y >= e.a->y && start.y < e.b->y);
}

static bool coord_inside_polygon(const Vec2 coord, const Edge *es,
                                 const size_t N) {
  // Cast rays and count how many times it intersects. Odd = inside,
  // even = outside.
  size_t intersections = 0;
  for (size_t i = 0; i < N; ++i) {
    // short circuit if we're actually on the edge!
    if (coord_lies_on_edge(coord, es[i])) return true;
    if (ray_intersects_edge(coord, es[i])) {
      ++intersections;
    }
  }
  return ((intersections % 2) == 1);
}

static bool edge_intersects_edge(Edge a, Edge b) {
  if (a.a->y == a.b->y && b.a->y == b.b->y) {
    return false;
  }
  if (a.a->x == a.b->x && b.a->x == b.b->x) {
    return false;
  }

  if (a.a->x == a.b->x) {
    // a is vertical, b must be horizontal

    // first, ensure a is ordered so that a.a->y < a.b->y
    if (a.a->y > a.b->y) {
      const Vec2 *tmp = a.a;
      a.a = a.b;
      a.b = tmp;
    }
    // ensure b is ordered so that b.a->x < b.b->x
    if (b.a->x > b.b->x) {
      const Vec2 *tmp = b.a;
      b.a = b.b;
      b.b = tmp;
    }

    // check b lies between the endpoints of a
    if (b.a->y <= a.a->y || b.a->y >= a.b->y) return false;
    // check a lies between the endpoints of b
    return (a.a->x > b.a->x && a.a->x < b.b->x);
  }
  // a is horizontal, b must be vertical
  // first, ensure a is ordered so that a.a->x < a.b->x
  if (a.a->x > a.b->x) {
    const Vec2 *tmp = a.a;
    a.a = a.b;
    a.b = tmp;
  }
  // ensure b is ordered so that b.a->y < b.b->y
  if (b.a->y > b.b->y) {
    const Vec2 *tmp = b.a;
    b.a = b.b;
    b.b = tmp;
  }
  // check b lies between the endpoints of a
  if (b.a->x <= a.a->x || b.a->x >= a.b->x) return false;
  // check a lies between the endpoints of b
  return (a.a->y > b.a->y && a.a->y < b.b->y);
}

static bool edge_intersects_any(const Edge e, const Edge *es, const size_t N) {
  for (size_t i = 0; i < N; ++i) {
    if (edge_intersects_edge(e, es[i])) return true;
  }
  return false;
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

  // pre-generate an array of edges so we can just work with those
  // directly
  Edge *edges = malloc_Edge_array(n_tiles);
  for (size_t i = 0; i < (n_tiles - 1); ++i) {
    edges[i].a = &coords[i];
    edges[i].b = &coords[i+1];
  }
  edges[n_tiles-1].a = &coords[n_tiles-1];
  edges[n_tiles-1].b = &coords[0];

  area_t biggest_area = 0;
  Vec2 chosen_a = {0, 0};
  Vec2 chosen_b = {0, 0};
  Vec2 chosen_c = { 0, 0 };
  Vec2 chosen_d = { 0, 0 };
  for (size_t i = 0; i < n_tiles; ++i) {
    for (size_t j = i + 1; j < n_tiles; ++j) {
      area_t area = compute_area(coords[i], coords[j]);
      if (area <= biggest_area) continue;
      
      // determine the two other corners
      const Vec2 a = coords[i]; const Vec2 b = coords[j];
      const Vec2 extra1 = {
        .x = a.x,
        .y = b.y
      };
      const Vec2 extra2 = {
        .x = b.x,
        .y = a.y
      };
      if (!coord_inside_polygon(extra1, edges, n_tiles)) continue;
      if (!coord_inside_polygon(extra2, edges, n_tiles)) continue;

      // construct the four edges
      const Edge edge1 = {&a, &extra1};
      const Edge edge2 = {&extra1, &b};
      const Edge edge3 = {&b, &extra2};
      const Edge edge4 = {&extra2, &a};
      if (edge_intersects_any(edge1, edges, n_tiles)) continue;
      if (edge_intersects_any(edge2, edges, n_tiles)) continue;
      if (edge_intersects_any(edge3, edges, n_tiles)) continue;
      if (edge_intersects_any(edge4, edges, n_tiles)) continue;

      biggest_area = area;
      chosen_a = a;
      chosen_b = b;
      chosen_c = extra1;
      chosen_d = extra2;
    }
  }

  printf("Biggest area: %lu\n", biggest_area);
  printf("(%d, %d) and (%d, %d)\n", chosen_a.x, chosen_a.y, chosen_b.x, chosen_b.y);
  printf("(%d, %d) and (%d, %d)\n", chosen_c.x, chosen_c.y, chosen_d.x, chosen_d.y);

  return 0;
}
