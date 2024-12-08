#include "common.h"

typedef struct {
  i64 accumulated_result;
  int operators_tried;
} state_type;

#define ADDITION 1
#define MULTIPLICATION 2
#define CONCATENATE 4

typedef struct {
  state_type *back;
  i32 in_stack;
  i32 cap;
} stack_type;

static void stack_push(stack_type *stack, state_type next) {
  assert(stack->in_stack < stack->cap);
  *stack->back++ = next;
  stack->in_stack++;
}

static state_type stack_pop(stack_type *stack) {
  assert(stack->in_stack > 0);
  stack->in_stack--;
  return *--stack->back;
}

static const u64 powers_of_10[] = {
    1,           10,           100,           1000,           10000,
    100000,      1000000,      10000000,      100000000,      1000000000,
    10000000000, 100000000000, 1000000000000, 10000000000000, 100000000000000};

static int intlog2(u64 n) {
  _Static_assert(sizeof(u64) == sizeof(unsigned long),
                 "assumes long is 64 bits");
  assert(n != 0);
  int leading_zeros = __builtin_clzl(n);
  return 63 - leading_zeros;
}

static int intlog10(u64 n) {
  assert(n != 0);
  int ceil = ((intlog2(n) + 1) * 1233) >> 12;
  return ceil - (n < powers_of_10[ceil]);
}

static i64 concatenate(i64 first, i64 second) {
  assert(second > 0);
  int shift_left = intlog10(second) + 1;
  return first * powers_of_10[shift_left] + second;
}

static bool check_line(i64 target, const i32 *restrict nums_begin,
                       const i32 *restrict nums_end, stack_type stack) {
  assert(nums_begin != nums_end);
  state_type state = {.accumulated_result = *nums_begin++,
                      .operators_tried = 0};
  stack_push(&stack, state);

  while (stack.in_stack > 0) {
    state = stack_pop(&stack);
    if (nums_begin == nums_end) {
      if (state.accumulated_result == target) {
        return true;
      }
      nums_begin--;
    } else if (state.accumulated_result > target ||
               state.operators_tried == (ADDITION | MULTIPLICATION)) {
      nums_begin--;
    } else if (!(state.operators_tried & ADDITION)) {
      state.operators_tried |= ADDITION;
      state_type next = {.accumulated_result =
                             state.accumulated_result + *nums_begin++,
                         .operators_tried = 0};
      stack_push(&stack, state);
      stack_push(&stack, next);
    } else {
      assert(!(state.operators_tried & MULTIPLICATION));
      state.operators_tried |= MULTIPLICATION;
      state_type next = {.accumulated_result =
                             state.accumulated_result * *nums_begin++,
                         .operators_tried = 0};
      stack_push(&stack, state);
      stack_push(&stack, next);
    }
  }

  return false;
}

static bool check_line2(i64 target, const i32 *restrict nums_begin,
                        const i32 *restrict nums_end, stack_type stack) {
  assert(nums_begin != nums_end);
  state_type state = {.accumulated_result = *nums_begin++,
                      .operators_tried = 0};
  stack_push(&stack, state);

  while (stack.in_stack > 0) {
    state = stack_pop(&stack);
    if (nums_begin == nums_end) {
      if (state.accumulated_result == target) {
        return true;
      }
      nums_begin--;
    } else if (state.accumulated_result > target ||
               state.operators_tried ==
                   (ADDITION | MULTIPLICATION | CONCATENATE)) {
      nums_begin--;
    } else if (!(state.operators_tried & ADDITION)) {
      state.operators_tried |= ADDITION;
      state_type next = {.accumulated_result =
                             state.accumulated_result + *nums_begin++,
                         .operators_tried = 0};
      stack_push(&stack, state);
      stack_push(&stack, next);
    } else if (!(state.operators_tried & MULTIPLICATION)) {
      state.operators_tried |= MULTIPLICATION;
      state_type next = {.accumulated_result =
                             state.accumulated_result * *nums_begin++,
                         .operators_tried = 0};
      stack_push(&stack, state);
      stack_push(&stack, next);
    } else {
      assert(!(state.operators_tried & CONCATENATE));
      state.operators_tried |= CONCATENATE;
      state_type next = {.accumulated_result = concatenate(
                             state.accumulated_result, *nums_begin++),
                         .operators_tried = 0};
      stack_push(&stack, state);
      stack_push(&stack, next);
    }
  }

  return false;
}

typedef struct {
  i64 target;
  i32s nums;
} line_type;

static line_type parse_line(str8 line, arena *persist, arena scratch) {
  line_type result = {0};
  byte *colon = memchr(line.data, ':', line.len);
  assert(colon);
  result.target = str8_to_i64((str8){line.data, colon - line.data});
  assert(colon[1] == ' ');
  str8 rest = {colon + 2, line.len - (colon - line.data) - 2};
  assert(rest.len > 0);
  str8s nums_strs = str8_split(rest, &scratch);
  for (i32 i = 0; i < nums_strs.len; ++i) {
    i32s_push(&result.nums, str8_to_i64(nums_strs.data[i]), persist);
  }

  return result;
}

void day7(input_pipe *pipe, arena a) {
  assert(intlog10(11) == 1);
  assert(intlog10(99) == 1);
  assert(intlog10(100) == 2);
  assert(intlog10(9999999) == 6);
  assert(intlog10(10000000) == 7);
  assert(concatenate(58233868, 6697784) == 582338686697784);
  arena scratch = {.begin = new (&a, byte, 1024), .end = scratch.begin + 1024};
  stack_type stack = {
      .back = new (&a, state_type, 32), .in_stack = 0, .cap = 32};
  i64 part1_sum = 0;
  i64 part2_sum = 0;
  while (!pipe->eof) {
    arena for_line = scratch;
    str8 next_line = input_pipe_getline(pipe, &for_line).str;
    if (next_line.len == 0) {
      break;
    }
    line_type parsed = parse_line(next_line, &a, for_line);
    if (check_line(parsed.target, parsed.nums.data,
                   parsed.nums.data + parsed.nums.len, stack)) {
      part1_sum += parsed.target;
      part2_sum += parsed.target;
    } else if (check_line2(parsed.target, parsed.nums.data,
                           parsed.nums.data + parsed.nums.len, stack)) {
      part2_sum += parsed.target;
    }
  }

  printf("Day 7, Part 1: sum = %ld\n", part1_sum);
  assert(part1_sum == 2314935962622l);
  printf("Day 7, Part 2: sum = %ld\n", part2_sum);
  assert(part2_sum == 401477450831495l);
}
