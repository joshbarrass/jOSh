#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <kernel/vgadef.h>
#include "aoc_common.h"

#define SOURCE_CHAR 'S'
#define FREE_CHAR '.'
#define SPLITTER_CHAR '^'
#define BEAM_CHAR '\xb3'

typedef struct Grid {
  size_t width;
  size_t stride;
  size_t height;
  char *buf;
} Grid;

typedef unsigned long long int count_t;

typedef struct CountGrid {
  size_t width;
  size_t height;
  count_t *buf;
} CountGrid;

static bool is_valid_char(const char c) {
  switch (c) {
  case FREE_CHAR:
  case SOURCE_CHAR:
  case SPLITTER_CHAR:
    return true;
  }
  return false;
}

static char *grid_get_ptr(const Grid *g, size_t x, size_t y) {
  return &g->buf[(g->width + g->stride)*y + x]; 
}

static count_t *cgrid_get_ptr(const CountGrid *g, size_t x, size_t y) {
  return &g->buf[(g->width)*y + x];
}

static char grid_get(const Grid *g, size_t x, size_t y) {
  return *grid_get_ptr(g, x, y); 
}

static count_t cgrid_get(const CountGrid *g, size_t x, size_t y) {
  return *cgrid_get_ptr(g, x, y);
}

static void grid_set(Grid *g, size_t x, size_t y, char c) {
  if (x > g->width || y > g->height) return;
  *grid_get_ptr(g, x, y) = c;
}

static void cgrid_set(CountGrid *g, size_t x, size_t y, count_t c) {
  if (x > g->width || y > g->height) return;
  *cgrid_get_ptr(g, x, y) = c;
}

static const Grid new_grid(char *input) {
  // scan until we get an invalid character to determine the width
  Grid g = {0, 0, 0, input};
  while (is_valid_char(input[g.width])) {
    ++g.width;
  }
  // scan until we get a valid character to determine the stride
  while (!is_valid_char(input[g.width + g.stride])) {
    ++g.stride;
  }
  g.height = strlen(input) / (g.width + g.stride);
  return g;
}

array_malloc(count_t);

static const CountGrid new_cgrid(const Grid *g) {
  CountGrid cg = {g->width, g->height, NULL};

  // allocate the buffer
  cg.buf = malloc_count_t_array(cg.width * cg.height);

  // zero it
  for (size_t y = 0; y < cg.height; ++y) {
    for (size_t x = 0; x < cg.width; ++x) {
      cgrid_set(&cg, x, y, 0);
    }
  }

  return cg;
}

int main() {
  char *input = get_input();
  Grid g = new_grid(input);
  CountGrid cg = new_cgrid(&g);

  printf("Grid size: %zux%zu\n", g.width, g.height);

  // set the initial count on the source char
  for (size_t i = 0; i < g.width; ++i) {
    if (grid_get(&g, i, 0) == SOURCE_CHAR) cgrid_set(&cg, i, 0, 1);
  }

  unsigned int times_split = 0;
  for (size_t row = 1; row < g.height; ++row) {
    for (size_t i = 0; i < g.width; ++i) {
      const char above = grid_get(&g, i, row-1);
      if (above == BEAM_CHAR || above == SOURCE_CHAR) {
        if (grid_get(&g, i, row) == SPLITTER_CHAR) {
          grid_set(&g, i-1, row, BEAM_CHAR);
          grid_set(&g, i+1, row, BEAM_CHAR);
          ++times_split;

          count_t count_above = cgrid_get(&cg, i, row-1);
          for (size_t i_ = i - 1; i_ <= i + 1; i_ += 2) {
            count_t count_current = cgrid_get(&cg, i_, row);
            cgrid_set(&cg, i_, row, count_current + count_above);
          }
        } else {
          grid_set(&g, i, row, BEAM_CHAR);

          count_t count_current = cgrid_get(&cg, i, row);
          count_t count_above = cgrid_get(&cg, i, row-1);
          cgrid_set(&cg, i, row, count_current + count_above);
        }
      }
    }
  }

  if (g.width <= VGA_WIDTH) printf("%s\n", input);

  printf("Times split: %u\n", times_split);

  // sum the bottom row to get the total timelines
  unsigned long long int timelines = 0;
  for (size_t i = 0; i < cg.width; ++i) {
    timelines += cgrid_get(&cg, i, cg.height-1);
  }
  printf("Timelines: %llu\n", timelines);
  return 0;
}
