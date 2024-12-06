#include "common.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))

static bool safe_report(i32s nums) {
  if (nums.len == 0) {
    return true;
  }
  int direction = 0;

  i32 prev = nums.data[0];
  for (i32 i = 1; i < nums.len; ++i) {
    i32 next = nums.data[i];
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

    i32 diff = next - prev;
    prev = next;
    if (ABS(diff) > 3) {
      return false;
    }
  }

  return true;
}

static bool check_with_removed(i32s nums, i32 index, arena scratch) {
  i32s new_nums = {new (&scratch, i32, nums.len), 0, nums.len};
  for (i32 i = 0; i < nums.len; ++i) {
    if (i != index) {
      i32s_push(&new_nums, nums.data[i], &scratch);
    }
  }

  return safe_report(new_nums);
}

static bool safe_report_tolerant(i32s nums, arena scratch) {
  if (nums.len < 2) {
    return true;
  }
  i32s candidates = {new (&scratch, i32, nums.len), 0, nums.len};
  int direction = 0;
  i32 prev = nums.data[0];

  for (i32 i = 1; i < nums.len; ++i) {
    i32 next = nums.data[i];
    i32 diff = next - prev;

    if (diff < 0) {
      if (direction == 1) {
        i32s_push(&candidates, i - 1, &scratch);
        direction = -1;
        prev = next;
        continue;
      }
      direction = -1;
    } else if (diff > 0) {
      if (direction == -1) {
        i32s_push(&candidates, i - 1, &scratch);
        direction = 1;
        prev = next;
        continue;
      }
      direction = 1;
    }

    if (ABS(diff) > 3 || diff == 0) {
      i32s_push(&candidates, i - 1, &scratch);
    }

    prev = next;
  }

  if (candidates.len == 0) {
    return true;
  }

  for (i32 i = 0; i < candidates.len; ++i) {
    i32 candidate = candidates.data[i];
    if (check_with_removed(nums, candidate, scratch) ||
        check_with_removed(nums, candidate + 1, scratch) ||
        check_with_removed(nums, candidate - 1, scratch)) {
      return true;
    }
  }

  return false;
}

static i32s to_ints(str8s tokens, arena *scratch) {
  i32s result = {new (scratch, i32, tokens.len), tokens.len, tokens.len};
  for (i32 i = 0; i < tokens.len; ++i) {
    result.data[i] = str8_to_i64(tokens.data[i]);
  }
  return result;
}

void day2(input_pipe *in, arena a) {
  i32 part1_safe_count = 0;
  i32 part2_safe_count = 0;

  while (!in->eof) {
    arena for_line = a;
    str8 line = input_pipe_getline(in, &for_line).str;
    if (line.len == 0) {
      break;
    }
    i32s nums = to_ints(str8_split(line, &for_line), &for_line);
    if (safe_report(nums)) {
      part1_safe_count++;
    }
    if (safe_report_tolerant(nums, for_line)) {
      part2_safe_count++;
    }
  }

  printf("Day 2, Part 1: safe count = %d\n", part1_safe_count);
  printf("Day 2, Part 2: safe count = %d\n", part2_safe_count);
  assert(part1_safe_count == 369);
  assert(part2_safe_count == 428);
}
