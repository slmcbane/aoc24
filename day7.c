#include "common.h"

typedef struct {
  i64 accumulated_result;
  int operators_tried;
} state_type;

#define ADDITION 1
#define MULTIPLICATION 2

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
  arena scratch = {.begin = new (&a, byte, 1024), .end = scratch.begin + 1024};
  stack_type stack = {
      .back = new (&a, state_type, 32), .in_stack = 0, .cap = 32};
  i64 part1_sum = 0;
  while (!pipe->eof) {
    arena for_line = scratch;
    str8 next_line = input_pipe_getline(pipe, &for_line).str;
    if (next_line.len == 0) {
      break;
    }
    line_type parsed = parse_line(next_line, &a, for_line);
    if (check_line(parsed.target, parsed.nums.data,
                   parsed.nums.data + parsed.nums.len, stack)) {
      assert(stack.in_stack == 0 && stack.cap == 32);
      part1_sum += parsed.target;
    }
  }

  printf("Day 7, Part 1: sum = %ld\n", part1_sum);
  assert(part1_sum == 2314935962622l);
}
