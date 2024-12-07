#include "common.h"

#include <time.h>

static u64 nanoseconds_count() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * 1000000000 + ts.tv_nsec;
}

extern void day1(input_pipe *in, arena a);
extern void day2(input_pipe *in, arena a);
extern void day3(input_pipe *in, arena a);
extern void day4(input_pipe *in, arena a);
extern void day5(input_pipe *in, arena a);

#define SOLVE_DAY(n)                                                           \
  do {                                                                         \
    input_pipe pipe;                                                           \
    input_pipe_init(&pipe, "inputs/day" #n ".txt");                            \
    u64 start = nanoseconds_count();                                           \
    day##n(&pipe, main_arena);                                                 \
    u64 end = nanoseconds_count();                                             \
    printf("  time for day " #n ": %f us\n", (end - start) / 1000.0);          \
    input_pipe_deinit(&pipe);                                                  \
  } while (0);

int main() {
  arena main_arena = {.begin = malloc(1 << 16),
                      .end = main_arena.begin + (1 << 16)};

  SOLVE_DAY(1);
  SOLVE_DAY(2);
  SOLVE_DAY(3);
  SOLVE_DAY(4);
  SOLVE_DAY(5);

  free(main_arena.begin);
}
