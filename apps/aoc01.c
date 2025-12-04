#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "aoc_common.h"

enum Direction {
  LEFT = -1, RIGHT = 1
};

int main() {
  const char *input = get_input();

  int position = 50;
  bool reading_num = false;
  const char *numbers_start = input;
  enum Direction dir = LEFT;

  int times_passed = 0;
  
  while (*input != 0) {
    if (*input == 'L') {
      dir = LEFT;
      reading_num = true;
      ++input;
      numbers_start = input++;
      continue;
    } else if (*input == 'R') {
      dir = RIGHT;
      reading_num = true;
      ++input;
      numbers_start = input++;
      continue;
    } else if (*input == '\n' || *input == 0) {
      reading_num = false;
      // read the number and compute the new position
      int num = atoi(numbers_start);
      for (int i = 0; i < num; ++i) {
        position = (position + dir) % 100;
        if (position == 0) ++times_passed;
      }
    }
    ++input;
  }

  printf("Times passed zero: %d\n", times_passed);

  return 0;
}
