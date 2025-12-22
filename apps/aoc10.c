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

struct generator {
  const int *max_vals;
  int k;

  int state[MAX_BUTTONS];
  bool started;
};

void generator_init(struct generator *g, const int *max_vals, const int k) {
  g->max_vals = max_vals;
  g->k = k;
  g->started = false;
  for (size_t i = 0; i < MAX_BUTTONS; ++i) {
    g->state[i] = 0;
  }
}

bool generator_next(struct generator *g) {
  if (!g->started) {
    g->started = true;
    return true;
  }
  for (size_t i = 0; i < g->k; ++i) {
    ++g->state[i];
    if (g->state[i] <= g->max_vals[i]) {
      return true;
    }
    g->state[i] = 0;
  }
  return false;
}

#define MAX_MAT_SIZE (MAX_LIGHTS*(MAX_BUTTONS+1))

typedef struct Matrix {
  size_t rows;
  size_t cols;
  int elems[MAX_MAT_SIZE];
} Matrix;

typedef struct LinEq {
  Matrix eqns;
  int max_presses[MAX_BUTTONS];
  size_t original_col[MAX_BUTTONS];
} LinEq;

static void init_matrix_elems(Matrix *m) {
  for (size_t i = 0; i < MAX_MAT_SIZE; ++i) {
    m->elems[i] = 0;
  }
}

static int *matrix_get_ptr(const Matrix *m, const size_t row, const size_t col) {
  if (m->rows == 0 || m->cols == 0) return 0;
  return &m->elems[m->cols*row + col];
}

static int matrix_get(const Matrix *m, const size_t row, const size_t col) {
  if (m->rows == 0 || m->cols == 0) return 0;
  return *(const int*)matrix_get_ptr(m, row, col);
}

static void matrix_set(Matrix *m, const size_t row, const size_t col, const int val) {
  if (m->rows == 0 || m->cols == 0) return;
  *matrix_get_ptr(m, row, col) = val;
}

static Matrix machine_to_matrix(const Machine *m) {
  Matrix mat = {
    .rows = m->n_lights,
    .cols = m->n_buttons + 1
  };
  // zero the elements
  init_matrix_elems(&mat);

  // add the buttons as column vectors
  for (size_t j = 0; j < m->n_buttons; ++j) {
    const Button b = m->buttons[j];
    for (size_t i = 0; i < b.n_lights; ++i) {
      matrix_set(&mat, b.lights[i], j, 1);
    }
  }
  // add the joltage vector
  for (size_t i = 0; i < m->n_lights; ++i) {
    matrix_set(&mat, i, mat.cols-1, m->joltages[i]);
  }
  
  return mat;
}

static void calculate_max_presses(LinEq *lineq) {
  for (size_t button = 0; button < lineq->eqns.cols - 1; ++button) {
    for (size_t row = 0; row < lineq->eqns.rows; ++row) {
      int rowval = matrix_get(&lineq->eqns, row, button);
      if (rowval == 1) {
        int rowmax = matrix_get(&lineq->eqns, row, lineq->eqns.cols - 1);
        if (rowmax < 0) continue;
        if (rowmax < lineq->max_presses[button]) lineq->max_presses[button] = rowmax;
      }
    }
  }
}

static LinEq machine_to_lineq(const Machine *m) {
  LinEq lineq = {
    .eqns = machine_to_matrix(m)
  };
  for (size_t i = 0; i < lineq.eqns.cols - 1; ++i) {
    lineq.max_presses[i] = INT32_MAX;
    lineq.original_col[i] = i;
  }

  calculate_max_presses(&lineq);

  return lineq;
}

static void matrix_swap_rows(Matrix *m, size_t row1, size_t row2) {
  for (size_t col = 0; col < m->cols; ++col) {
    int tmp = matrix_get(m, row2, col);
    matrix_set(m, row2, col, matrix_get(m, row1, col));
    matrix_set(m, row1, col, tmp);
  }
}

static void matrix_roll_rows(Matrix *m, size_t roll) {
  for (size_t i = 0; i < roll; ++i) {
    for (size_t row = 0; row < m->rows - 1; ++row) {
      matrix_swap_rows(m, row, row+1);
    }
  }
}

static void matrix_swap_cols(Matrix *m, size_t col1, size_t col2) {
  for (size_t row = 0; row < m->rows; ++row) {
    int tmp = matrix_get(m, row, col2);
    matrix_set(m, row, col2, matrix_get(m, row, col1));
    matrix_set(m, row, col1, tmp);
  }
}

static void matrix_scale_row(Matrix *m, size_t row, int s) {
  for (size_t col = 0; col < m->cols; ++col) {
    matrix_set(m, row, col, s*matrix_get(m, row, col));
  }
}

static void matrix_add_row(Matrix *m, size_t row, size_t to) {
  for (size_t col = 0; col < m->cols; ++col) {
    matrix_set(m, to, col, matrix_get(m, row, col) + matrix_get(m, to, col));
  }
}

static void matrix_add_scaled_row(Matrix *m, size_t row, int s, size_t to) {
  for (size_t col = 0; col < m->cols; ++col) {
    matrix_set(m, to, col, s*matrix_get(m, row, col) + matrix_get(m, to, col));
  }
}

static void matrix_add_col(Matrix *m, size_t col, size_t to) {
  for (size_t row = 0; row < m->rows; ++row) {
    matrix_set(m, row, to, matrix_get(m, row, col) + matrix_get(m, row, to));
  }
}

static void matrix_add_scaled_col(Matrix *m, size_t col, int s, size_t to) {
  for (size_t row = 0; row < m->rows; ++row) {
    matrix_set(m, row, to, s*matrix_get(m, row, col) + matrix_get(m, row, to));
  }
}

static void deep_copy_lineq(const LinEq * restrict src, LinEq * restrict dst) {
  for (size_t i = 0; i < MAX_MAT_SIZE; ++i) {
    dst->eqns.elems[i] = src->eqns.elems[i];
  }
  dst->eqns.rows = src->eqns.rows;
  dst->eqns.cols = src->eqns.cols;
  for (size_t i = 0; i < MAX_BUTTONS; ++i) {
    dst->max_presses[i] = src->max_presses[i];
    dst->original_col[i] = src->original_col[i];
  }
}

static void lineq_reduce(LinEq *e) {
  for (size_t row = 0; row < e->eqns.rows; ++row) {
    // check whether the row has a non-zero value on the diagonal
    int diag = matrix_get(&e->eqns, row, row);
    if (diag == 0) {
      // rearrange the rows so we have a non-zero diagonal term
      for (size_t other = row+1; other < e->eqns.rows; ++other) {
        diag = matrix_get(&e->eqns, other, row);
        if (diag != 0) {
          matrix_swap_rows(&(e->eqns), other, row);
          break;
        }
      }
    }
    if (diag == 0) {
      // If we made it here, there are no rows with a non-zero value
      // on the diagonal. In this case, we can swap the columns
      // instead
      for (size_t col = row + 1; col < e->eqns.cols - 1; ++col) {
        diag = matrix_get(&e->eqns, row, col);
        if (diag != 0) {
          matrix_swap_cols(&(e->eqns), col, row);
          // also swap the max_presses columns
          int tmp = e->max_presses[col];
          e->max_presses[col] = e->max_presses[row];
          e->max_presses[row] = tmp;
          // ... and column tracker
          size_t tmp2 = e->original_col[col];
          e->original_col[col] = e->original_col[row];
          e->original_col[row] = tmp2;
          break;
        }
      }
    }
    if (diag == 0) {
      // If we made it here, then this row is all zeros. If there are
      // any non-zero rows below it, we can swap them in
      // instead and repeat the process
      bool found_new_row = false;
      for (size_t other = row + 1; other < e->eqns.rows; ++other) {
        for (size_t col = row + 1; col < e->eqns.cols - 1; ++col) {
          if (matrix_get(&e->eqns, other, col) != 0) {
            matrix_swap_rows(&e->eqns, row, other);
            found_new_row = true;
            break;
          }
        }
        if (found_new_row) break;
      }
      if (found_new_row) {
        // restart the loop for this new row
        --row;
        continue;
      }
      // if we couldn't find a new non-zero row, then we're done!
      return;
    }

    // try to get a diagonal value with unit magnitude
    /* int old_diag = diag; */
    if (diag != 1 && diag != -1) {
      for (size_t col = row + 1; col < e->eqns.cols - 1; ++col) {
        diag = matrix_get(&e->eqns, row, col);
        if (diag == 1 || diag == -1) {
          matrix_swap_cols(&(e->eqns), col, row);
          // also swap the max_presses columns
          int tmp = e->max_presses[col];
          e->max_presses[col] = e->max_presses[row];
          e->max_presses[row] = tmp;
          // ... and column tracker
          size_t tmp2 = e->original_col[col];
          e->original_col[col] = e->original_col[row];
          e->original_col[row] = tmp2;
          break;
        }
      }
      if (diag != 1 && diag != -1) return; // failed to simplify further -- hope for the best?
    }

    // ensure the diagonal is positive
    if (diag < 0) {
      matrix_scale_row(&e->eqns, row, -1);
      diag *= -1;
    }

    // loop through the other rows to see if they're non-zero
    for (size_t other = 0; other < e->eqns.rows; ++other) {
      if (other == row) continue;
      const int off_diag = matrix_get(&e->eqns, other, row);
      if (off_diag != 0) {
        // subtract to cancel
        matrix_add_scaled_row(&e->eqns, row, (-1)*off_diag/diag, other);
      }
    }
  }
}

static void press_button_joltage(const Button *b, joltage_t *state) {
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

static bool test_buttons_joltage(const Machine *m, const int *buttons) {
  joltage_t state[MAX_LIGHTS];
  for (size_t i = 0; i < MAX_LIGHTS; ++i) {
    state[i] = 0;
  }
  press_buttons_joltage(m, state, buttons);
  for (size_t i = 0; i < m->n_lights; ++i) {
    if (state[i] != m->joltages[i]) return false;
  }
  return true;
}

static int find_fewest_buttons_joltage(const Machine *m) {
  // convert the machine to a system of linear equations
  LinEq eq;
  size_t diagonal_length;
  size_t dof;
  int roll = 0;

  // aim to get under 4 degrees of freedom, since that's about the
  // limit of tractability. If we end up with a matrix with more dof
  // than that, roll the rows and try again to see if we get something
  // better
  do {
    eq = machine_to_lineq(m);
    matrix_roll_rows(&eq.eqns, roll++);
    lineq_reduce(&eq);

    /* for (size_t i = 0; i < eq.eqns.rows; ++i) { */
    /*   for (size_t j = 0; j < eq.eqns.cols; ++j) { */
    /*     printf("%2d ", matrix_get(&eq.eqns, i, j)); */
    /*   } */
    /*   printf("\n"); */
    /* } */
    /* for (size_t i = 0; i < eq.eqns.cols - 1; ++i) { */
    /*   printf("%2d ", eq.max_presses[i]); */
    /* } */
    /* printf("\n"); */

    // count how large the diagonal section is
    diagonal_length = 0;
    bool is_diag = true;
    for (size_t col = 0; col < eq.eqns.cols-1; ++col) {
      for (size_t row = 0; row < eq.eqns.rows; ++row) {
        const int val = matrix_get(&eq.eqns, row, col);
        if (row == col && val == 0) {
          is_diag = false;
          break;
        }
        if (row != col && val != 0) {
          is_diag = false;
          break;
        }
      }
      if (!is_diag)
        break;
      ++diagonal_length;
    }

    /* printf("Diagonal segment is %zux%zu\n", diagonal_length, diagonal_length); */
    dof = eq.eqns.cols - 1 - diagonal_length;
    /* printf("Have %zu degrees of freedom\n", dof); */
  } while (dof > 4 && roll < eq.eqns.rows);

  // use a generator to brute force just the degrees of freedom
  int min_presses = INT32_MAX;
  struct generator g;
  generator_init(&g, &eq.max_presses[diagonal_length], dof);

  int buttons[MAX_BUTTONS];
  while (generator_next(&g)) { // does this succeed for n=0?
    // make a mutable copy of the lineq
    static LinEq eq_tmp;
    deep_copy_lineq(&eq, &eq_tmp);

    // push the buttons specified by the generator, i.e., add the
    // columns to the end column
    int push_total = 0;
    for (size_t i = diagonal_length; i < eq.eqns.cols - 1; ++i) {
      int pushes = g.state[i - diagonal_length];
      push_total += pushes;
      matrix_add_scaled_col(&eq_tmp.eqns, i, -pushes, eq.eqns.cols-1);
      buttons[eq.original_col[i]] = pushes;
    }
    if (push_total >= min_presses) continue;

    // calculate how many times the remaining buttons need to be
    // pressed -- don't forget to check whether it's actually
    // possible to solve (e.g. if a button contributes an amount
    // with magnitude greater than 1, the target value must be
    // divisible
    bool is_valid = true;
    for (size_t col = 0; col < diagonal_length; ++col) {
      int button_val = matrix_get(&eq_tmp.eqns, col, col);
      if (button_val == 0) {
        is_valid = false;
        break;
      }
      int joltage_val = matrix_get(&eq_tmp.eqns, col, eq_tmp.eqns.cols-1);
      if (joltage_val % button_val != 0) {
        // no number of presses will solve this, so it's an invalid
        // solution
        is_valid = false;
        break;
      }
      int pushes = joltage_val / button_val;
      if (pushes < 0) {
        is_valid = false;
        break;
      }
      push_total += pushes;
      buttons[eq.original_col[col]] = pushes;
    }
    if (!is_valid) continue; 

    // check that the solution works using the original machine
    if (!test_buttons_joltage(m, buttons)) {
      /* printf("\nGenerated invalid state!\n"); */
      continue;
    }

    if (push_total < min_presses) {
      min_presses = push_total;
      /* printf("Min now %d\n", min_presses); */
    }
  }

  return min_presses;
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
  printf("%3d/%3zu[", 0, n_machines);
  for (size_t i = 0; i < (30); ++i) {
    printf("-");
  }
  printf("]");
  for (size_t i = 0; i < n_machines; ++i) {
    // print the progress bar
    size_t n_segs = (i * (30)) / n_machines;
    printf("\r%03zu/%03zu[", i, n_machines);
    for (size_t j = 0; j < n_segs; ++j) {
      printf("=");
    }
    
    const int b = find_fewest_buttons_joltage(&machines[i]);
    if (b == INT32_MAX) {
      printf("Failed on %zu\n", i);
    }
    /* printf("Machine %zu solveable with %d buttons\n", i+1, b); */
    total += b;
  }
  printf("\nPart 2 Total: %ld\n", total);

  /* printf("Min: %d\n", find_fewest_buttons_joltage(&machines[47])); */

  return 0;
}
