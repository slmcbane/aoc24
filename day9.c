#include "common.h"

typedef struct {
  const byte *next;
  i32 blocks_used;
  i32 block_id;
} input_iterator;

static i64 solve_part1(str8 input, arena a) {
  i64 curr_index = 0;
  i64 checksum = 0;
  input_iterator first = {input.data, input.data[0] - '0', 0};
  input_iterator last = {input.data + input.len - 1,
                         input.data[input.len - 1] - '0', input.len / 2};

  bool first_is_free = false;
  while (first.next <= last.next) {
    if (first_is_free) {
      while (first.blocks_used && first.next <= last.next) {
        if (last.blocks_used) {
          checksum += last.block_id * curr_index;
          curr_index++;
          first.blocks_used--;
          last.blocks_used--;
        } else {
          last.next -= 2;
          last.blocks_used = last.next[0] - '0';
          last.block_id--;
        }
      }

      first.next++;
      first.blocks_used = first.next[0] - '0';
      first.block_id++;
      first_is_free = false;
    } else {
      if (first.next == last.next) {
        assert(first.block_id == last.block_id);
        first.blocks_used = last.blocks_used;
      }
      while (first.blocks_used--) {
        checksum += first.block_id * curr_index;
        curr_index++;
      }
      first.next++;
      first.blocks_used = first.next[0] - '0';
      first_is_free = true;
    }
  }
  return checksum;
}

void day9(input_pipe *pipe, arena a) {
  str8 input = input_pipe_getline(pipe, &a).str;
  i64 part1_checksum = solve_part1(input, a);
  printf("Day 9, Part 1: checksum = %ld\n", part1_checksum);
  assert(part1_checksum == 6242766523059l);
}
