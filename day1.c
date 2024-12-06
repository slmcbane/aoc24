#include "common.h"

static int i32_cmp(const void *a, const void *b) {
  return *(i32 *)a - *(i32 *)b;
}

#define ABS(x) ((x) < 0 ? -(x) : (x))

static i64 finalize_part2(i32s list1, i32s list2) {
  i64 part2_similarity_score = 0;
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

  return part2_similarity_score;
}

typedef struct {
  i64 part1_sum;
  i64 part2_similarity_score;
} results;

static results finalize(i32s list1, i32s list2) {
  assert(list1.len == list2.len);
  i64 part1_sum = 0;
  qsort(list1.data, list1.len, sizeof(i32), i32_cmp);
  qsort(list2.data, list2.len, sizeof(i32), i32_cmp);

  for (i32 i = 0; i < list1.len; ++i) {
    i32 first = list1.data[i];
    i32 second = list2.data[i];
    i32 diff = first - second;
    part1_sum += ABS(diff);
  }

  return (results){part1_sum, finalize_part2(list1, list2)};
}

void day1(input_pipe *in, arena a) {
  i32s list1 = {0};
  i32s list2 = {0};

  arena scratch = {.begin = new (&a, byte, 256), .end = scratch.begin + 256};

  while (!in->eof) {
    arena for_line = scratch;
    str8 line = input_pipe_getline(in, &for_line).str;
    if (line.len == 0) {
      break;
    }
    str8s tokens = str8_split(line, &for_line);
    assert(tokens.len == 2);
    i32 first = str8_to_i64(tokens.data[0]);
    i32 second = str8_to_i64(tokens.data[1]);
    i32s_push(&list1, first, &a);
    i32s_push(&list2, second, &a);
  }

  results result = finalize(list1, list2);
  printf("Day 1, Part 1: sum = %ld\n", result.part1_sum);
  printf("Day 1, Part 2: similarity score = %ld\n",
         result.part2_similarity_score);
  assert(result.part1_sum == 2756096);
  assert(result.part2_similarity_score == 23117829l);
}
