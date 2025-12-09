#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <kernel/memory/constants.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/vmm.h>
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

static char **malloc_array(const size_t n) {
  const size_t bytes = sizeof(char*) * n;
  size_t pages_needed = bytes / PAGE_SIZE;
  const size_t remainder = bytes % PAGE_SIZE;
  if (remainder != 0) ++pages_needed;

  const uintptr_t phys_page = (uintptr_t)pmm_alloc_pages(pages_needed);
  return (char**)vmm_kmap(phys_page, bytes, 0, 0);
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
  const char **lines = malloc_array(n_lines);
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

  val_t sum = 0;
  while (lines[problem_size][0] != 0) {
    // read the operator
    bool safe = true;
    while (lines[problem_size][0] != '*' && lines[problem_size][0] != '+') {
      if (lines[problem_size][0] == 0 || lines[problem_size] == '\n') {
        safe = false;
        break;
      }
      ++lines[problem_size];
    }
    if (!safe) break;
    const char operator = lines[problem_size][0];
    /* printf("Operator: %c\n", operator); */
    val_t total = (operator == '*') ? 1 : 0;
    for (size_t i = 0; i < problem_size; ++i) {
      val_t num = strtoull(lines[i], &lines[i], 10);
      /* printf("Num: %llu\n", num); */
      switch (operator) {
      case '+':
        total += num; break;
      case '*':
        total *= num; break;
      }
    }
    /* printf("Total: %llu\n", total); */
    sum += total;
    ++lines[problem_size];
  }

  printf("Sum total: %llu\n", sum);

  return 0;
}
