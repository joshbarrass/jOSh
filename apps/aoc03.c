#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <multiboot.h>

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

int get_bank_max(const char *input, const char **endptr) {
  int total = 0;

  size_t bank_length = 0;
  while (input[bank_length] >= '0' && input[bank_length] <= '9') {
    ++bank_length;
  }
  *endptr = input + bank_length + 1;
  if (bank_length == 0)
    return 0;

  // There are much faster ways of doing this than just trialling
  // every combination (e.g. finding the largest digit, and then
  // finding the largest digit to the right of it), but this solves
  // the problem, and I paid for that sprintf, I'm going to get full
  // use out of it!
  static char buf[3] = { 0 };
  int max = 0;
  for (size_t start_pos = 0; start_pos < bank_length - 1; ++start_pos) {
    for (size_t target = start_pos + 1; target < bank_length; ++target) {
      sprintf(buf, "%c%c", (int)input[start_pos], (int)input[target]);
      int sum = atoi(buf);
      if (sum > max) max = sum;
    }
  }

  return max;
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
