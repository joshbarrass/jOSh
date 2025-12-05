#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "aoc_common.h"

#define FREE_CHAR '.'
#define FULL_CHAR '@'

#define THRESHOLD (4)

typedef struct Grid {
  size_t width;
  size_t stride;
  size_t height;
  char *buf;
} Grid;

static char *grid_get_ptr(const Grid *g, size_t x, size_t y) {
  return &g->buf[(g->width + g->stride)*y + x]; 
}

static char grid_get(const Grid *g, size_t x, size_t y) {
  return *grid_get_ptr(g, x, y); 
}

static const Grid new_grid(char *input) {
  // scan until we get an invalid character to determine the width
  Grid g = {0, 0, 0, input};
  while (input[g.width] == FREE_CHAR || input[g.width] == FULL_CHAR) {
    ++g.width;
  }
  // scan until we get a valid character to determine the stride
  while (input[g.width + g.stride] != FREE_CHAR && input[g.width + g.stride] != FULL_CHAR) {
    ++g.stride;
  }
  g.height = strlen(input) / (g.width + g.stride);
  return g;
}

static int get_full_neighbours(const Grid *g, size_t x, size_t y) {
  bool at_top = y == 0;
  bool at_bottom = y >= (g->height - 1);
  bool at_left = x == 0;
  bool at_right = x >= (g->width - 1);

  bool mask[9] = {
    (!at_top) && (!at_left), !at_top, (!at_top) && (!at_right),
    !at_left, false, !at_right,
    (!at_bottom) && (!at_left), !at_bottom, (!at_bottom) && (!at_right)
  };

  int num_full = 0;
  for (int j = -1; j <= 1; ++j) {
    for (int i = -1; i <= 1; ++i) {
      const int index = 3*(j+1)+(i+1);
      if (!mask[index]) continue;
      if (grid_get(g, x+i, y+j) == FULL_CHAR) ++num_full;
    }
  }

  return num_full;
}

int main() {
  const char *input = get_input();

  Grid g = new_grid(input);
  printf("Grid (%zux%zu, stride=%zu)\n", g.width, g.height, g.stride);

  int prev_num_accessible;
  int num_accessible = 0;
  do {
    prev_num_accessible = num_accessible;
    for (size_t y = 0; y < g.height; ++y) {
      for (size_t x = 0; x < g.width; ++x) {
        if (grid_get(&g, x, y) == FREE_CHAR) continue;
        if (get_full_neighbours(&g, x, y) < THRESHOLD) {
          ++num_accessible;
          *grid_get_ptr(&g, x, y) = FREE_CHAR;
        }
      }
    }
  } while (prev_num_accessible != num_accessible);

  printf("Total: %d\n", num_accessible);
  
  return 0;
}
