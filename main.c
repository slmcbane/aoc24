#include "common.h"

#include <time.h>

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

static u64 nanoseconds_count() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * 1000000000 + ts.tv_nsec;
}

extern void day1(signal act, str8 next_input, arena *a, arena scratch);
extern void day2(signal act, str8 next_input, arena *a, arena scratch);
extern void day3(signal act, str8 next_input, arena *a, arena scratch);
extern void day4(signal act, str8 next_input, arena *a, arena scratch);

#define SOLVE_DAY(n)                                                           \
  do {                                                                         \
    arena persistent = main_arena;                                             \
    arena scratch = scratch_arena;                                             \
    FILE *day##n##_input = fopen("inputs/day" #n ".txt", "rb");                \
    assert(day##n##_input);                                                    \
    u64 start = nanoseconds_count();                                           \
    day##n(BEGIN_SIGNAL, (str8){0}, &persistent, scratch);                     \
    str8 input = read_line(day##n##_input, &scratch);                          \
    while (input.len) {                                                        \
      day##n(DATA_SIGNAL, input, &persistent, scratch);                        \
      scratch = scratch_arena;                                                 \
      input = read_line(day##n##_input, &scratch);                             \
    }                                                                          \
    day##n(END_SIGNAL, (str8){0}, &persistent, scratch_arena);                 \
    u64 end = nanoseconds_count();                                             \
    printf("  time for day " #n ": %f us\n", (end - start) / 1000.0);          \
  } while (0);

int main() {
  arena main_arena = {.begin = malloc(1 << 16),
                      .end = main_arena.begin + (1 << 16)};
  arena scratch_arena = {.begin = malloc(1 << 13),
                         .end = scratch_arena.begin + (1 << 13)};

  SOLVE_DAY(1);
  SOLVE_DAY(2);
  SOLVE_DAY(3);
  SOLVE_DAY(4);

  free(main_arena.begin);
  free(scratch_arena.begin);
}
