#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <multiboot.h>

extern const MIS *mis;

const char *empty_string = "\0";

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

typedef struct Range {
  unsigned long long int start;
  unsigned long long int end;
} Range;

Range parse_range(const char *buf, const char **endptr) {
  const char *first_num_end;
  Range r = {0, 0};
  r.start = strtoull(buf, &first_num_end, 10);
  if (*(first_num_end++) != '-') {
    r.start = 0;
    return r;
  }
  r.end = strtoull(first_num_end, endptr, 10);
  const char lastchar = *(*endptr)++;
  if (lastchar != ',' && lastchar != '\n' && lastchar != 0) {
    r.start = 0;
    r.end = 0;
    return r;
  }
  return r;
}

bool is_num_invalid(const unsigned long long int num) {
  // convert to a string
  char buf[(8 * sizeof(unsigned long long int)) * 30103 / 100000 + 5] = { 0 };
  sprintf(buf, "%lld", num);
  
  const size_t l = strlen(buf);
  if (l % 2 != 0) return false;

  // copy the first half of the string to its own buffer
  char front[(8 * sizeof(unsigned long long int)) * 30103 / 100000 + 5] = { 0 };
  for (size_t i = 0; i < l / 2; ++i) {
    front[i] = buf[i];
  }

  return strcmp(buf + l/2, front) == 0;
}

unsigned long long int sum_invalid_IDs(const Range r) {
  unsigned long long int total = 0;

  for (unsigned long long int i = r.start; i < r.end; ++i) {
    if (is_num_invalid(i)) total += i;
  }

  return total;
}

int main() {
  const char *input = get_input();

  unsigned long long int total = 0;
  while (*input != '\n' && *input != '\0') {
    Range r = parse_range(input, &input);
    if (!(r.start == 0 && r.end == 0)) {
      printf("Range: %llu - %llu\n", r.start, r.end);
      total += sum_invalid_IDs(r);
    }
  }
  printf("Total: %llu\n", total);
  /* printf("Total: %016llx%016llx\n", (uint64_t)(total>>64), (uint64_t)total); */
}
