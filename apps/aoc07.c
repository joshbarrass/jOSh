#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <kernel/vgadef.h>
#include <kernel/memory/constants.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/vmm.h>
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

static char grid_get(const Grid *g, size_t x, size_t y) {
  return *grid_get_ptr(g, x, y); 
}

static void grid_set(Grid *g, size_t x, size_t y, char c) {
  if (x > g->width) return;
  *grid_get_ptr(g, x, y) = c;
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

int main() {
  char *input = get_input();
  Grid g = new_grid(input);
  
  printf("Grid size: %zux%zu\n", g.width, g.height);

  unsigned int times_split = 0;
  for (size_t row = 1; row < g.height; ++row) {
    for (size_t i = 0; i < g.width; ++i) {
      const char above = grid_get(&g, i, row-1);
      if (above == BEAM_CHAR || above == SOURCE_CHAR) {
        if (grid_get(&g, i, row) == SPLITTER_CHAR) {
          grid_set(&g, i-1, row, BEAM_CHAR);
          grid_set(&g, i+1, row, BEAM_CHAR);
          ++times_split;
        } else {
          grid_set(&g, i, row, BEAM_CHAR);
        }
      }
    }
  }

  if (g.width <= VGA_WIDTH) printf("%s\n", input);

  printf("Times split: %u\n", times_split);
  return 0;
}
