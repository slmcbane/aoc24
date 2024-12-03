#include "common.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))

static bool safe_report(str8s tokens) {
  if (tokens.len == 0) {
    return true;
  }
  int direction = 0;

  i64 prev = str8_to_i64(tokens.data[0]);
  for (i32 i = 1; i < tokens.len; ++i) {
    i64 next = str8_to_i64(tokens.data[i]);
    if (next < prev) {
      if (direction == 1) {
        return false;
      }
      direction = -1;
    } else if (next > prev) {
      if (direction == -1) {
        return false;
      }
      direction = 1;
    } else {
      return false;
    }

    i64 diff = next - prev;
    prev = next;
    if (ABS(diff) > 3) {
      return false;
    }
  }

  return true;
}

void day2(signal act, str8 next_input, arena *persistent, arena scratch) {
  static i32 part1_safe_count;
  if (act == BEGIN_SIGNAL) {
    part1_safe_count = 0;
    return;
  } else if (act == END_SIGNAL) {
    printf("Day 2, Part 1: safe count = %d\n", part1_safe_count);
    return;
  }

  str8s tokens = str8_split(next_input, &scratch);
  if (safe_report(tokens)) {
    part1_safe_count++;
  }
}
