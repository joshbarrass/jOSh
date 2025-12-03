#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <multiboot.h>

#define NUM_VALS (2)

extern const MIS *mis;

static const char *empty_string = "\0";

static char *get_input() {
  const Mod *mods = get_mods(mis);
  for (size_t i = 0; i < mis->mods_count; ++i) {
    const Mod *mod = mods + i;
    if (strcmp("input", get_mod_string(mod)) == 0) {
      return (char*)get_mod_start(mod);
    }
  }
  return empty_string;
}

int get_highest_in_N(const char *input, const size_t N, const char **endptr) {
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

int get_bank_max(const char *input, const char **endptr) {
  int total = 0;

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

  int sum = 0;
  while (*input != '\0') {
    sum += get_bank_max(input, &input);
  }

  printf("Total: %d\n", sum);

  return 0;
}
