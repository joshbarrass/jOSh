#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "aoc_common.h"

typedef unsigned long long int   val_t;

static size_t count_lines(const char *input, bool *last_line_empty) {
  if (input[0] == 0) return 0;
  size_t lines = 1;
  while (input[0] != 0) {
    if (input[0] == '\n') ++lines;
    ++input;
  }
  if (last_line_empty != NULL) *last_line_empty = (input[-1] == '\n');
  return lines;
}

typedef char* string;

array_malloc(string);
array_malloc(char);

static char *malloc_string(const size_t n) {
  return malloc_char_array(n);
}

static bool is_just_spaces(const char *s) {
  while (*s != 0) {
    if (*s != ' ') return false;
    ++s;
  }
  return true;
}

int main() {
  char *input = get_input();
  
  bool last_line_empty;
  const size_t n_lines = count_lines(input, &last_line_empty);
  printf("File has %zu lines\n", n_lines);

  size_t problem_size = n_lines - 1; // subtract 1 for the operations
  if (last_line_empty) --problem_size;

  printf("Problem size: %zu\n", problem_size);

  // allocate an array to store the pointers to the starts of the lines
  const char **lines = malloc_string_array(n_lines);
  {
    size_t i = 1;
    lines[0] = input;
    while (input[0] != 0) {
      if (input[0] == '\n') {
        lines[i++] = input + 1;
        input[0] = 0;
      }
      ++input;
    }
    printf("Populated array of %zu lines (should be %zu)\n", i, n_lines);
    if (i != n_lines) return -1;
  }

  // allocate an array for the list of digits
  char *digits = malloc_string(problem_size+1);

  val_t sum = 0;
  while (lines[problem_size][0] != 0) {
    // read the operator
    bool safe = true;
    while (lines[problem_size][0] != '*' && lines[problem_size][0] != '+') {
      if (lines[problem_size][0] == 0) {
        safe = false;
        break;
      }
      ++lines[problem_size];
    }
    if (!safe) break;
    const char operator = lines[problem_size][0];
    /* printf("Operator: %c\n", operator); */
    val_t total = (operator == '*') ? 1 : 0;

    // wipe the digits string
    for (size_t i = 0; i < (problem_size + 1); ++i) {
      digits[i] = 0;
    }

    // copy into the digits and parse until we're left with all spaces
    do {
      for (size_t i = 0; i < problem_size; ++i) {
        digits[i] = lines[i][0];
        ++lines[i];
      }
      if (is_just_spaces(digits)) break;
      const val_t num = strtoull(digits, NULL, 10);
      /* printf("Digits: %s, Num: %llu\n", digits, num); */
      switch (operator) {
      case '*': total *= num; break;
      case '+': total += num; break;
      }
    } while (!is_just_spaces(digits));

    /* printf("Total: %llu\n", total); */
    sum += total;
    ++lines[problem_size];
  }

  printf("Sum total: %llu\n", sum);

  return 0;
}
