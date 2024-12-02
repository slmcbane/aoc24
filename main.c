#include "common.h"

static str8 read_line(FILE *in, arena *a) {
  str8_builder line = str8_builder_init(a, 16);
  int next = 0;

  while ((next = fgetc(in)) != EOF) {
    unsigned char c = (unsigned char)next;
    if (c == '\n') {
      return line.str;
    } else {
      str8_builder_push(&line, c, a);
    }
  }

  return line.str;
}

extern void day1(signal act, str8 next_input, arena *a);

#define SOLVE_DAY(n)                                                           \
  do {                                                                         \
    arena work = main_arena;                                                   \
    FILE *day##n##_input = fopen("inputs/day" #n ".txt", "rb");                \
    assert(day##n##_input);                                                    \
    day##n(BEGIN_SIGNAL, (str8){0}, NULL);                                     \
    str8 input = read_line(day##n##_input, &work);                             \
    while (input.len) {                                                        \
      day##n(DATA_SIGNAL, input, &work);                                       \
      input = read_line(day##n##_input, &work);                                \
    }                                                                          \
    day##n(END_SIGNAL, (str8){0}, NULL);                                       \
  } while (0);

int main() {
  arena main_arena = {.begin = malloc(1 << 16),
                      .end = main_arena.begin + (1 << 16)};

  SOLVE_DAY(1);

  free(main_arena.begin);
}
