#include <stdio.h>
#include <stddef.h>
#include "aoc_common.h"

#define NUM_VALS (12)

static int get_highest_in_N(const char *input, const size_t N, const char **endptr) {
  int max = 0;
  for (size_t i = 0; i < N; ++i) {
    char val = input[i] - '0';
    if (val > max) {
      max = val;
      *endptr = input + i + 1;
    }
  }
  return max;
}

static long long int get_bank_max(const char *input, const char **endptr) {
  long long int total = 0;

  size_t bank_length = 0;
  while (input[bank_length] >= '0' && input[bank_length] <= '9') {
    ++bank_length;
  }
  *endptr = input + bank_length + 1;
  if (bank_length == 0) return 0;

  const char *prev_ptr = input;
  size_t digits_remaining = NUM_VALS;
  while (digits_remaining-- > 0) {
    total *= 10;
    size_t N = bank_length - digits_remaining;
    total += get_highest_in_N(input, N, &input);
    bank_length -= (input - prev_ptr);
    prev_ptr = input;
  }
  
  return total;
}

int main() {
  const char *input = get_input();

  long long int sum = 0;
  while (*input != '\0') {
    sum += get_bank_max(input, &input);
  }

  printf("Total: %lld\n", sum);

  return 0;
}
