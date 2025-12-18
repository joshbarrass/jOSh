#include <stdio.h>
#include <stdlib.h>
#include <kernel/memory/constants.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/vmm.h>
#include "aoc_common.h"

#define MAX_LIGHTS (10)
#define MAX_BUTTONS (15)

#define CHAR_LIGHT_OFF '.'
#define CHAR_LIGHT_ON '#'

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
  bool state[MAX_LIGHTS];
  size_t n_buttons;
  Button buttons[MAX_BUTTONS];
  joltage_t joltages[MAX_LIGHTS];
} Machine;

void zero_init_machine(Machine *m) {
  m->n_lights = 0;
  for (size_t i = 0; i < MAX_LIGHTS; ++i) {
    m->target_state[i] = false;
    m->state[i] = false;
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

void reset_machine_state(Machine *m) {
  for (size_t i = 0; i < MAX_LIGHTS; ++i) {
    m->state[i] = false;
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

  return 0;
}
