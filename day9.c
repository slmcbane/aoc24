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

typedef struct free_block free_block;
typedef struct occupied_block occupied_block;

/*struct free_block {
  free_block *next;
  i32 block_start;
}; */

struct occupied_block {
  occupied_block *prev;
  i32 block_start;
  i32 block_size;
  i32 block_id;
};

// static free_block *free_blocks_by_size[10];
static occupied_block *occupied_blocks;

/*static void process_input_for_day_2(str8 input, arena *a) {
  bool occupied = true;
  occupied_block *prev_occupied_block = NULL;
  free_block **free_links[10];
  for (int i = 1; i < 10; ++i) {
    free_links[i] = free_blocks_by_size + i;
  }

  i32 block_start = 0;
  i32 block_id = 0;
  for (i32 i = 0; i < input.len; ++i) {
    i32 this_block_size = input.data[i] - '0';
    if (this_block_size) {
      if (occupied) {
        occupied_block *new_occupied = new (a, occupied_block, 1);
        new_occupied->prev = prev_occupied_block;
        prev_occupied_block = new_occupied;
        new_occupied->block_start = block_start;
        new_occupied->block_size = this_block_size;
        new_occupied->block_id = block_id++;
      } else {
        free_block *new_free = new (a, free_block, 1);
        new_free->next = NULL;
        new_free->block_start = block_start;
        *free_links[this_block_size] = new_free;
        free_links[this_block_size] = &new_free->next;
      }
    }
    occupied = !occupied;
    block_start += this_block_size;
  }
  occupied_blocks = prev_occupied_block;
}

static void remove_and_reinsert(i32 from_list, i32 block_size) {
  printf("  remove_and_reinsert(%d, %d)\n", from_list, block_size);
  free_block *head = free_blocks_by_size[from_list];
  free_blocks_by_size[from_list] = head->next;
  head->next = NULL;
  i32 new_block_size = from_list - block_size;
  assert(new_block_size >= 0);
  if (new_block_size > 0) {
    head->block_start += block_size;
    printf("    new_block_start = %d\n", head->block_start);
    printf("    new_block_size = %d\n", new_block_size);
    free_block **link = free_blocks_by_size + new_block_size;
    free_block *before = *link;
    while (before && (before->block_start < head->block_start)) {
      link = &before->next;
      before = before->next;
    }
    if (before) {
      printf("      inserting before block at %d\n", before->block_start);
    } else {
      printf("      inserting at head\n");
    }
    head->next = before;
    *link = head;
  }
}

static void defrag(occupied_block *block) {
  while (block) {
    for (i32 block_size = block->block_size; block_size < 10; ++block_size) {
      free_block *next_free = free_blocks_by_size[block_size];
      if (next_free && (next_free->block_start < block->block_start)) {
        printf("Relocating block %d of size %d to %d\n", block->block_id,
               block->block_size, next_free->block_start);
        block->block_start = next_free->block_start;
        remove_and_reinsert(block_size, block->block_size);
        break;
      }
    }

    block = block->prev;
  }
} */

struct free_block {
  free_block *next;
  i32 block_start;
  i32 block_size;
};

free_block *free_list = NULL;

static void process_input_for_day_2(str8 input, arena *a) {
  occupied_block *prev_occupied_block = NULL;
  free_block **free_link = &free_list;

  i32 block_start = 0;
  i32 block_id = 0;
  for (i32 i = 0; i < input.len; ++i) {
    bool occupied = (i % 2 == 0);
    i32 block_size = input.data[i] - '0';
    if (block_size) {
      if (occupied) {
        occupied_block *new_occupied = new (a, occupied_block, 1);
        new_occupied->prev = prev_occupied_block;
        prev_occupied_block = new_occupied;
        new_occupied->block_start = block_start;
        new_occupied->block_size = block_size;
        new_occupied->block_id = block_id++;
      } else {
        free_block *new_free = new (a, free_block, 1);
        *free_link = new_free;
        new_free->next = NULL;
        new_free->block_start = block_start;
        new_free->block_size = block_size;
        free_link = &new_free->next;
      }
    }
    block_start += block_size;
  }

  occupied_blocks = prev_occupied_block;
}

static i32 find_free_block(i32 block_size, i32 block_start) {
  // printf("find_free_block(%d)\n", block_size);
  free_block **link = &free_list;
  while (*link) {
    free_block *next = *link;
    if (next->block_start > block_start) {
      break;
    }
    if (next->block_size >= block_size) {
      if (next->block_size == block_size) {
        // printf("Removing block at %d from freelist\n", next->block_start);
        *link = next->next;
        return next->block_start;
      } else {
        // printf("Shrinking block at %d; new size = %d, new start = %d\n",
        //       next->block_start, next->block_size - block_size,
        //      next->block_start + block_size);
        next->block_size -= block_size;
        next->block_start += block_size;
        return next->block_start - block_size;
      }
    }
    link = &next->next;
  }
  return -1;
}

static void defrag(occupied_block *block) {
  for (; block; block = block->prev) {
    i32 new_block_start =
        find_free_block(block->block_size, block->block_start);
    if (new_block_start != -1) {
      // printf("Relocating block at %d to %d\n", block->block_start,
      // new_block_start);
      block->block_start = new_block_start;
    }
  }
}

void day9(input_pipe *pipe, arena a) {
  str8 input = input_pipe_getline(pipe, &a).str;
  i64 part1_checksum = solve_part1(input, a);
  printf("Day 9, Part 1: checksum = %ld\n", part1_checksum);
  // assert(part1_checksum == 6242766523059l);
  process_input_for_day_2(input, &a);

  defrag(occupied_blocks);

  /*occupied_block *curr_occupied = occupied_blocks;
  while (curr_occupied) {
    printf("Occupied block: block_start = %d, block_size = %d, block_id = %d\n",
           curr_occupied->block_start, curr_occupied->block_size,
           curr_occupied->block_id);
    curr_occupied = curr_occupied->prev;
  }*/

  i64 part2_checksum = 0;
  for (occupied_block *curr = occupied_blocks; curr; curr = curr->prev) {
    i64 index = curr->block_start;
    for (i32 i = 0; i < curr->block_size; ++i, ++index) {
      part2_checksum += index * curr->block_id;
    }
  }

  printf("Day 9, Part 2: checksum = %ld\n", part2_checksum);
}
