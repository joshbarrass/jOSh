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
  if (l == 1) return false;

  // calculate the length of the longest substring that could be
  // repeated
  size_t max_substr_length = 1;
  for (size_t i = 1; i <= l/2; ++i) {
    if (l % i == 0) max_substr_length = i;
  }

  // check all lengths of substring for invalid options
  static char substr[(8 * sizeof(unsigned long long int)) * 30103 / 100000 + 5] = { 0 };
  static char chunk[(8 * sizeof(unsigned long long int)) * 30103 / 100000 + 5] = { 0 };
  for (size_t substr_l = 1; substr_l <= max_substr_length; ++substr_l) {
    if (l % substr_l != 0) continue;

    // clear buffers
    for (size_t i = 0; i < sizeof(substr) / sizeof(substr[0]); ++i) {
      substr[i] = 0;
    }
    for (size_t i = 0; i < sizeof(chunk) / sizeof(chunk[0]); ++i) {
      chunk[i] = 0;
    }

    // copy the substring of this length into a buffer
    for (size_t i = 0; i < substr_l; ++i) {
      substr[i] = buf[i];
    }

    // loop through each chunk of the string, copy that location into
    // a buffer, and compare with the substr
    bool all_chunks_match = true;
    for (size_t i = 0; i < l; i += substr_l) {
      // build a chunk of length substr_l
      for (size_t j = 0; j < substr_l; ++j) {
        chunk[j] = buf[i+j];
      }
      /* printf("Chunk %zu (substr_l=%zu): %s\n", i, substr_l, chunk); */

      // compare the chunk with the substring
      if (strcmp(chunk, substr) != 0) {
        all_chunks_match = false;
        break;
      }
    }
    if (all_chunks_match) return true;
  }
  return false;
}

unsigned long long int sum_invalid_IDs(const Range r) {
  unsigned long long int total = 0;

  for (unsigned long long int i = r.start; i <= r.end; ++i) {
    if (is_num_invalid(i)) {
      total += i;
      /* printf("Invalid: %llu\n", i); */
    }
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
