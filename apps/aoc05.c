#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "aoc_common.h"

typedef struct Range {
  unsigned long long int start;
  unsigned long long int end;
} Range;

static Range parse_range(const char *buf, const char **endptr) {
  const char *first_num_end;
  Range r = {0, 0};
  r.start = strtoull(buf, &first_num_end, 10);
  if (*(first_num_end++) != '-') {
    r.start = 0;
    return r;
  }
  r.end = strtoull(first_num_end, endptr, 10);
  const char lastchar = *(*endptr)++;
  if (lastchar != '\n' && lastchar != 0) {
    r.start = 0;
    r.end = 0;
    return r;
  }
  return r;
}

array_malloc(Range);

static size_t count_ranges(const char *input) {
  size_t lines = 0;
  if (input[0] != '\n' || input[1] != '\n') ++lines;
  while (input[0] != '\n' || input[1] != '\n') {
    if (input[0] == '\n') ++lines;
    ++input;
  }
  return lines;
}

static bool is_in_range(const unsigned long long int num, const Range r) {
  return (num >= r.start) && (num <= r.end);
}

static bool condense_ranges(Range *r1, Range *r2) {
  if (r1 == r2) return false;
  if (r1->start == 0 && r1->end == 0) return false;
  if (r2->start == 0 && r2->end == 0) return false;
  if (is_in_range(r1->start, *r2) || is_in_range(r1->end, *r2)) {
    Range *temp = r2;
    r2 = r1;
    r1 = temp;
  }
  if (is_in_range(r2->start, *r1) || is_in_range(r2->end, *r1)) {
    // r2 has overlap with r1 -- merge r2 into r1

    // if r2 starts earlier than r1, shift r1 down
    if (r2->start < r1->start) r1->start = r2->start;
    // else do nothing -- we are keeping r1 and nulling r2

    // if r2 ends later than r1, shift r1 up
    if (r2->end > r1->end) r1->end = r2->end;

    r2->start = 0;
    r2->end = 0;

    return true;
  }
  return false;
}

static bool condense_all_ranges(Range *ranges, const size_t N) {
  bool any_changed;
  do {
    any_changed = false;
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < N; ++j) {
        if (condense_ranges(&ranges[i], &ranges[j])) any_changed = true;
      }
    }
  } while (any_changed);
}

static unsigned long long int range_length(const Range r) {
  if (r.start == r.end && r.start == 0) return 0;
  return r.end - r.start + 1;
}

int main() {
  const char *input = get_input();
  const size_t n_ranges = count_ranges(input);
  Range *ranges = malloc_Range_array(n_ranges);

  for (size_t i = 0; i < n_ranges; ++i) {
    ranges[i] = parse_range(input, &input);
  }
  ++input;

  condense_all_ranges(ranges, n_ranges);
  printf("Condensed all ranges!\n");

  size_t n_fresh = 0;
  while (input[0] != 0) {
    const unsigned long long int num = strtoull(input, &input, 10);
    bool is_fresh = false;
    for (size_t i = 0; i < n_ranges; ++i) {
      if (is_in_range(num, ranges[i])) {
        is_fresh = true;
        break;
      }
    }
    if (is_fresh) ++n_fresh;
    if (input[0] == '\n') ++input;
  }

  printf("Number fresh: %zu\n", n_fresh);

  unsigned long long int total_fresh_IDs = 0;
  for (size_t i = 0; i < n_ranges; ++i) {
    total_fresh_IDs += range_length(ranges[i]);
  }
  printf("Total number of fresh IDs: %llu\n", total_fresh_IDs);

  return 0;
}
