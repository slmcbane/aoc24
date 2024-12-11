#include "common.h"

typedef struct {
  const byte *next;
  i32 blocks_used;
  i32 block_id;
} input_iterator;

typedef struct occupied_block occupied_block;

struct occupied_block {
  occupied_block *next;
  i32 block_id;
  i32 size;
};

typedef struct free_block free_block;

struct free_block {
  free_block *prev;
  free_block *next;
  free_block *parent;
  free_block *child;
  occupied_block *next_occupied;
};

typedef struct {
  free_block *head;
  free_block *tail;
} free_block_list;

static free_block_list free_blocks_by_size[10];
static occupied_block *occupied_blocks;

static free_block *insert_free_block_helper(int size, free_block *parent,
                                            arena *a) {
  free_block *new_block = new (a, free_block, 1);
  new_block->next = NULL;
  new_block->parent = parent;
  new_block->next_occupied = NULL;

  free_block *prev = free_blocks_by_size[size].tail;
  if (!prev) {
    free_blocks_by_size[size].head = free_blocks_by_size[size].tail = new_block;
    new_block->prev = NULL;
  } else {
    assert(!prev->next);
    prev->next = new_block;
    new_block->prev = prev;
    free_blocks_by_size[size].tail = new_block;
  }

  if (size > 1) {
    new_block->child = insert_free_block_helper(size - 1, new_block, a);
  } else {
    new_block->child = NULL;
  }

  return new_block;
}

static free_block *insert_free_block(int size, arena *a) {
  if (size) {
    insert_free_block_helper(size, NULL, a);
    return free_blocks_by_size[1].tail;
  } else {
    return NULL;
  }
}

static i64 process_input(str8 input, arena a) {
  memset(free_blocks_by_size, 0, sizeof(free_blocks_by_size));
  occupied_blocks = NULL;

  i64 curr_index = 0;
  i64 checksum = 0;
  input_iterator first = {input.data, input.data[0] - '0', 0};
  input_iterator last = {input.data + input.len - 1,
                         input.data[input.len - 1] - '0', input.len / 2};

  free_block *prev_free_block = NULL;
  occupied_block **prev_occupied_link = &occupied_blocks;
  bool first_is_free = false;
  while (first.next <= last.next) {
    if (first_is_free) {
      prev_free_block = insert_free_block(first.blocks_used, &a);
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
      if (first.blocks_used) {
        occupied_block *new_occupied_block = new (&a, occupied_block, 1);
        new_occupied_block->next = NULL;
        new_occupied_block->block_id = first.block_id;
        new_occupied_block->size = first.blocks_used;
        *prev_occupied_link = new_occupied_block;
        prev_occupied_link = &new_occupied_block->next;
        if (prev_free_block) {
          prev_free_block->next_occupied = new_occupied_block;
        }
        if (first.next == last.next) {
          assert(first.block_id == last.block_id);
          first.blocks_used = last.blocks_used;
        }
        while (first.blocks_used--) {
          checksum += first.block_id * curr_index;
          curr_index++;
        }
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
  i64 part1_checksum = process_input(input, a);
  printf("Day 9, Part 1: checksum = %ld\n", part1_checksum);
  assert(part1_checksum == 6242766523059l);
  // i64 part2_checksum = solve_part2(occupied_blocks, free_blocks_by_size, 1);
  //  printf("Day 9, Part 2: checksum = %ld\n", part2_checksum);
}
