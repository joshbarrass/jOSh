#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <kernel/memory/constants.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/vmm.h>
#include <kernel/vgadef.h>
#include "aoc_common.h"

#define MAX_LIGHTS (10)
#define MAX_BUTTONS (15)

#define CHAR_LIGHT_OFF '.'
#define CHAR_LIGHT_ON '#'

typedef size_t iter_t;

#define array_malloc(T) static T *malloc_##T##_array (const size_t n) { \
  const size_t bytes = sizeof(T) * n; \
  size_t pages_needed = bytes / PAGE_SIZE; \
  printf("Need %zu pages to allocate " #T "[%zu]\n", pages_needed, n);     \
  const size_t remainder = bytes % PAGE_SIZE; \
  if (remainder != 0) ++pages_needed; \
  const uintptr_t phys_page = (uintptr_t)pmm_alloc_pages(pages_needed); \
  return (T*)vmm_kmap(phys_page, bytes, 0, 0); \
}

typedef int joltage_t;

typedef struct Button {
  size_t n_lights;
  size_t lights[MAX_LIGHTS];
} Button;

typedef struct Machine {
  size_t n_lights;
  bool target_state[MAX_LIGHTS];
  /* bool state[MAX_LIGHTS]; */
  size_t n_buttons;
  Button buttons[MAX_BUTTONS];
  joltage_t joltages[MAX_LIGHTS];
} Machine;

void zero_init_machine(Machine *m) {
  m->n_lights = 0;
  for (size_t i = 0; i < MAX_LIGHTS; ++i) {
    m->target_state[i] = false;
    /* m->state[i] = false; */
    m->joltages[i] = 0;
  }
  m->n_buttons = 0;
  for (size_t i = 0; i < MAX_BUTTONS; ++i) {
    m->buttons[i].n_lights = 0;
    for (size_t j = 0; j < MAX_LIGHTS; ++j) {
      m->buttons[i].lights[j] = 0;
    }
  }
}

array_malloc(Machine);

static size_t count_lines(const char *input, bool ignore_empty_last_line) {
  if (input[0] == 0) return 0;
  size_t lines = 1;
  while (input[0] != 0) {
    if (input[0] == '\n') ++lines;
    ++input;
  }
  if (input[-1] == '\n' && ignore_empty_last_line) --lines;
  return lines;
}

#define ERR_INVALID_CHAR (-1)
#define ERR_TOO_MANY_LIGHTS (-2)
#define ERR_TOO_MANY_BUTTONS (-3)
#define ERR_TOO_MANY_JOLTAGES (-4)

static int parse_machine(const char *input, const char **endptr, Machine *m) {
  zero_init_machine(m);
  if (input[0] != '[') return ERR_INVALID_CHAR;
  ++input;

  // parse the lights
  bool reading_lights = true;
  while (reading_lights) {
    switch (input[0]) {
    case CHAR_LIGHT_ON:
      if (m->n_lights >= (MAX_LIGHTS)) return ERR_TOO_MANY_LIGHTS;
      m->target_state[m->n_lights++] = true;
      break;
    case CHAR_LIGHT_OFF:
      if (m->n_lights >= (MAX_LIGHTS)) return ERR_TOO_MANY_LIGHTS;
      m->target_state[m->n_lights++] = false;
      break;
    case ']':
      reading_lights = false;
      break;
    default:
      return ERR_INVALID_CHAR;
    }
    ++input;
  }

  // read until we hit the start of the joltage
  Button current_button = {0};
  while (input[0] != '{') {
    // skip any non-numerical characters
    if (input[0] < '0' || input[0] > '9') {
      // ...unless it marks the end of a button
      if (input[0] == ')') {
        if (m->n_buttons >= MAX_BUTTONS)
          return ERR_TOO_MANY_BUTTONS;
        m->buttons[m->n_buttons++] = current_button;
        current_button.n_lights = 0;
      }
      ++input;
      continue;
    }

    // parse the number
    if (current_button.n_lights >= (MAX_LIGHTS)) return ERR_TOO_MANY_LIGHTS;
    size_t n = strtoull(input, &input, 10);
    current_button.lights[current_button.n_lights++] = n;
  }

  // finally, read the joltage requirements
  ++input;
  size_t i = 0;
  char *end = input;
  while (input[0] != '}' && input[0] != '\n') {
    if (i >= (MAX_LIGHTS)) return ERR_TOO_MANY_JOLTAGES;
    // parse the number
    size_t n = strtoull(input, &input, 10);
    m->joltages[i++] = n;
    ++input;
  }

  // read until we hit a newline
  char last_char = *(input++);
  while (last_char != '\n' && last_char != 0) {
    last_char = *(input++);
  }
  *endptr = input;
  return 0;
}

static void print_button(const Button *b) {
  printf("(");
  for (size_t i = 0; i < b->n_lights - 1; ++i) {
    printf("%zu,", b->lights[i]);
  }
  printf("%zu)", b->lights[b->n_lights - 1]);
}

static void print_machine(const Machine *m) {
  printf("[");
  for (size_t i = 0; i < m->n_lights; ++i) {
    if (m->target_state[i]) printf("%c", CHAR_LIGHT_ON);
    else printf("%c", CHAR_LIGHT_OFF);
  }
  printf("] ");
  for (size_t i = 0; i < m->n_buttons; ++i) {
    print_button(&m->buttons[i]);
    printf(" ");
  }
  printf("{");
  for (size_t i = 0; i < m->n_lights - 1; ++i) {
    printf("%d,", m->joltages[i]);
  }
  printf("%d}\n", m->joltages[m->n_lights-1]);
}

static void press_button(const Button *b, bool *state) {
  for (size_t i = 0; i < b->n_lights; ++i) {
    state[b->lights[i]] = !state[b->lights[i]];
  }
}

static void press_buttons(const Machine *m, bool *state, iter_t buttons) {
  size_t i = 0;
  while (buttons > 0 && i < m->n_buttons) {
    if ((buttons & 1) == 1) press_button(&m->buttons[i], state);
    buttons >>= 1;
    i += 1;
  }
}

static bool is_state_correct(const Machine *m, const bool *state) {
  for (size_t i = 0; i < m->n_lights; ++i) {
    if (state[i] != m->target_state[i]) return false;
  }
  return true;
}

static bool test_buttons(const Machine *m, iter_t buttons) {
  bool state[MAX_LIGHTS] = { false };
  press_buttons(m, state, buttons);
  return is_state_correct(m, state);
}

static int find_fewest_buttons(const Machine *m) {
  int fewest_buttons = INT32_MAX;
  for (iter_t iterator = 1; iterator < (1 << m->n_buttons); ++iterator) {
    if (test_buttons(m, iterator)) {
      int buttons_used = __builtin_popcountll(iterator);
      if (buttons_used < fewest_buttons) fewest_buttons = buttons_used;
    }
  }
  return fewest_buttons;
}

/* Part 2 */

// stack frame for the generator to emulate the recursive generator
// for the weak compositions
struct generator_frame {
  int n;
  int k;
  int v;
  int i; // index into the state array
};

#define GENERATOR_STACK_SIZE (MAX_BUTTONS+1)
#define ERR_STACK_OVERFLOW 1

struct generator {
  int N;
  int n;
  int k;

  int state[MAX_BUTTONS];
  struct generator_frame stack[GENERATOR_STACK_SIZE];
  size_t sp;
  bool started;
};

void generator_init(struct generator *g, int n, int k) {
  g->N = n;
  g->n = n;
  g->k = k;
  g->sp = 0;
  g->started = false;
}

// Based on https://stackoverflow.com/a/59131521
bool generator_next(struct generator *g) {
  if (!g->started) {
    // push initial frame
    g->stack[0] = (struct generator_frame){ g->n, g->k, 0, 0 };
    g->sp = 1;
    g->started = true;
  }

  while (g->sp > 0) {
    struct generator_frame *f = &g->stack[g->sp - 1];

    // base case: no slots left
    if (f->k == 0) {
      if (f->n == 0) {
        g->sp--;
        return true;
      }
      g->sp--;
      continue;
    }

    // skip solutions that will not satisfy the constraint
    if (f->n < 0 || f->n > f->k * g->N) {
      g->sp--;
      continue;
    }

    // try next value
    if (f->v <= g->N) {
      int v = f->v++;
      g->state[f->i] = v;

      // check stack isn't full!
      if (g->sp >= GENERATOR_STACK_SIZE) {
        errno = ERR_STACK_OVERFLOW;
        return false;
      }

      // push next stack frame
      g->stack[g->sp++] = (struct generator_frame){
        .n = f->n - v,
        .k = f->k - 1,
        .v = 0,
        .i = f->i + 1
      };
    } else {
      // hit maximum v -- end of loop
      g->sp--;
    }
  }

  return false;
}

static void
press_button_joltage(const Button *b, joltage_t *state) {
  for (size_t i = 0; i < b->n_lights; ++i) {
    ++state[b->lights[i]];
  }
}

static void press_buttons_joltage(const Machine *m, joltage_t *state, const int *buttons) {
  for (size_t i = 0; i < m->n_buttons; ++i) {
    for (size_t j = 0; j < buttons[i]; ++j) {
      press_button_joltage(&m->buttons[i], state);
    }
  }
}

static bool are_joltages_correct(const Machine *m, const joltage_t *state) {
  for (size_t i = 0; i < m->n_lights; ++i) {
    if (state[i] != m->joltages[i]) return false;
  }
  return true;
}

static bool test_buttons_joltages(const Machine *m, int *buttons) {
  joltage_t state[MAX_LIGHTS] = { 0 };
  press_buttons_joltage(m, state, buttons);
  return are_joltages_correct(m, state);
}

static void gen_buttons_array(int *buttons, int N, int k, int index, int remaining) {
  if (index == k - 1) {
    // Last part gets the remainder
    buttons[index] = remaining;
    return;
  }

  for (int x = 0; x <= remaining; x++) {
    buttons[index] = x;
    gen_buttons_array(buttons, N, k, index + 1, remaining - x);
  }
}

static long long int factorial(long long int n) {
  if (n == 0) return 1;
  long long int total = 1;
  for (long long int i = 2; i <= n; ++i) {
    total *= n;
  }
  return total;
}

static long long int choose(long long int n, long long int k) {
  return factorial(n) / (factorial(n-k) * factorial(k));
}

static int find_fewest_buttons_joltage(const Machine *m) {
  struct generator g;

  // find the largest number in the joltage requirements. That will be
  // the absolute minimum number of button presses, otherwise the
  // counter could never reach that value!
  int n = 0;
  for (size_t i = 0; i < m->n_lights; ++i) {
    if (m->joltages[i] > n) n = m->joltages[i];
  }
  --n; // we increment at the start of the loop, so we must start one lower

  bool done = false;
  while (!done) {
    ++n;
    generator_init(&g, n, m->n_buttons);
    generator_next(&g);
    while (generator_next(&g)) {
      if (test_buttons_joltages(m, g.state)) {
        done = true;
        break;
      }
    }
    if (errno != 0) {
      switch (errno) {
      case ERR_STACK_OVERFLOW:
        printf("Error in generator: stack overflow\n");
        break;
      }
      errno = 0;
      return -2;
    }
  }
  return n;
}

int main() {
  const char *input = get_input();
  const size_t n_machines = count_lines(input, true);
  printf("Number of machines: %zu\n", n_machines);
  Machine *machines = malloc_Machine_array(n_machines);

  for (size_t i = 0; i < n_machines; ++i) {
    int err = parse_machine(input, &input, &machines[i]);
    if (err != 0) {
      switch (err) {
      case ERR_INVALID_CHAR:
        printf("Encountered an invalid char!\n");
        break;
      case ERR_TOO_MANY_LIGHTS:
        printf("Encountered too many lights for current type!\n");
        break;
      case ERR_TOO_MANY_BUTTONS:
        printf("Encountered too many buttons for current type!\n");
        break;
      case ERR_TOO_MANY_JOLTAGES:
        printf("Encountered too many joltages for current type!\n");
        break;
      }
      return -1;
    }
  }
  printf("Successfully parsed all machines!\n");

  /* for (size_t i = 0; i < n_machines; ++i) { */
  /*   print_machine(machines+i); */
  /* } */
  long int total = 0;
  for (size_t i = 0; i < n_machines; ++i) {
    const int b = find_fewest_buttons(&machines[i]);
    /* printf("Machine %zu solveable with %d buttons\n", i+1, b); */
    total += b;
  }
  printf("Part 1 Total: %ld\n", total);

  total = 0;
  // add a progress bar
  printf("%3d/%3d[", 0, n_machines);
  for (size_t i = 0; i < (30); ++i) {
    printf("-");
  }
  printf("]");
  for (size_t i = 0; i < n_machines; ++i) {
    // print the progress bar
    size_t n_segs = (i * (30)) / n_machines;
    printf("\r%03d/%03d[", i, n_machines);
    for (size_t j = 0; j < n_segs; ++j) {
      printf("=");
    }

    const int b = find_fewest_buttons_joltage(&machines[i]);
    /* printf("Machine %zu joltage solveable with %d buttons\n", i+1, b); */
    total += b;
  }
  printf("\nPart 2 Total: %ld\n", total);

  return 0;
}
