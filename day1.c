#include "common.h"

static i64 part1_sum;
static i64 part2_similarity_score;
static i32s list1 = {0};
static i32s list2 = {0};

static int i32_cmp(const void *a, const void *b) {
  return *(i32 *)a - *(i32 *)b;
}

#define ABS(x) ((x) < 0 ? -(x) : (x))

static void finalize_part2() {
  part2_similarity_score = 0;
  i32 i1 = 0;
  i32 i2 = 0;
  i32 prev = -1;
  i64 prev_added = -1;

  for (; i1 < list1.len; ++i1) {
    i32 curr = list1.data[i1];
    if (curr != prev) {
      prev = curr;
      i32 same_count = 0;
      for (; i2 < list2.len && list2.data[i2] < curr; ++i2) {
        continue;
      }
      for (; i2 < list2.len && list2.data[i2] == curr; ++i2) {
        ++same_count;
      }
      prev_added = (i64)same_count * curr;
    }
    part2_similarity_score += prev_added;
  }
}

static void finalize() {
  assert(list1.len == list2.len);
  part1_sum = 0;
  qsort(list1.data, list1.len, sizeof(i32), i32_cmp);
  qsort(list2.data, list2.len, sizeof(i32), i32_cmp);

  for (i32 i = 0; i < list1.len; ++i) {
    i32 first = list1.data[i];
    i32 second = list2.data[i];
    i32 diff = first - second;
    part1_sum += ABS(diff);
  }

  finalize_part2();
}

void day1(signal act, str8 next_input, arena *a, arena scratch) {
  if (act == BEGIN_SIGNAL) {
    list1 = (i32s){0};
    list2 = (i32s){0};
    return;
  } else if (act == END_SIGNAL) {
    finalize();
    printf("Day 1, Part 1: sum = %ld\n", part1_sum);
    printf("Day 1, Part 2: similarity score = %ld\n", part2_similarity_score);
    assert(part1_sum == 2756096);
    assert(part2_similarity_score == 23117829l);
    return;
  }

  str8s tokens = str8_split(next_input, &scratch);
  assert(tokens.len == 2);

  i32 first = str8_to_i64(tokens.data[0]);
  i32 second = str8_to_i64(tokens.data[1]);
  i32s_push(&list1, first, a);
  i32s_push(&list2, second, a);
}
