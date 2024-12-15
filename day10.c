#include "common.h"

typedef struct {
  str8 contents;
  i32 rows;
  i32 cols;
} input_type;

#define INDEX(i, j, cols) ((i) * (cols) + (j))

static input_type parse_input(input_pipe *pipe, arena *a) {
  str8 contents = input_pipe_getline(pipe, a).str;
  assert(contents.len != 0);
  i32 rows = 1;
  i32 cols = contents.len;
  while (!pipe->eof) {
    str8 next_line = input_pipe_getline(pipe, a).str;
    if (next_line.len == 0) {
      break;
    }
    contents = str8_concat(contents, next_line, a);
    rows++;
  }
  return (input_type){contents, rows, cols};
}

static int cmp(const void *a, const void *b) {
  return *((i32 *)a) - *((i32 *)b);
}

static void make_unique(i32s *is) {
  qsort(is->data, is->len, sizeof(i32), cmp);
  i32 i = 0;
  for (i32 j = 1; j < is->len; ++j) {
    if (is->data[j] != is->data[j - 1]) {
      is->data[++i] = is->data[j];
    }
  }
  is->len = i + 1;
}

#define IN_BOUNDS(i, j, rows, cols)                                            \
  ((i) >= 0 && (i) < (rows) && (j) >= 0 && (j) < (cols))

static void push_neighbors(str8 map, i32 rows, i32 cols, i32 i, i32 j,
                           char next_level, i32s *q, arena *a) {
  if (IN_BOUNDS(i - 1, j, rows, cols) &&
      map.data[INDEX(i - 1, j, cols)] == next_level) {
    i16 coords[2] = {i - 1, j};
    i32 to_push;
    memcpy(&to_push, coords, sizeof(i16) * 2);
    i32s_push(q, to_push, a);
  }
  if (IN_BOUNDS(i + 1, j, rows, cols) &&
      map.data[INDEX(i + 1, j, cols)] == next_level) {
    i16 coords[2] = {i + 1, j};
    i32 to_push;
    memcpy(&to_push, coords, sizeof(i16) * 2);
    i32s_push(q, to_push, a);
  }
  if (IN_BOUNDS(i, j - 1, rows, cols) &&
      map.data[INDEX(i, j - 1, cols)] == next_level) {
    i16 coords[2] = {i, j - 1};
    i32 to_push;
    memcpy(&to_push, coords, sizeof(i16) * 2);
    i32s_push(q, to_push, a);
  }
  if (IN_BOUNDS(i, j + 1, rows, cols) &&
      map.data[INDEX(i, j + 1, cols)] == next_level) {
    i16 coords[2] = {i, j + 1};
    i32 to_push;
    memcpy(&to_push, coords, sizeof(i16) * 2);
    i32s_push(q, to_push, a);
  }
}

static i32 score_trailhead(str8 map, i32 rows, i32 cols, i32 i, i32 j,
                           arena a) {
  i32s next_levels[2] = {0};
  push_neighbors(map, rows, cols, i, j, '1', &next_levels[0], &a);
  int which_queue = 0;
  char this_level = '1';
  while (this_level < '9') {
    int next_queue = (which_queue + 1) % 2;
    for (i32 qi = 0; qi < next_levels[which_queue].len; ++qi) {
      i16 inds[2] = {0};
      i32 front = next_levels[which_queue].data[qi];
      memcpy(inds, &front, sizeof(i16) * 2);
      i = inds[0];
      j = inds[1];
      push_neighbors(map, rows, cols, inds[0], inds[1], this_level + 1,
                     &next_levels[next_queue], &a);
    }
    next_levels[which_queue].len = 0;
    make_unique(&next_levels[next_queue]);
    which_queue = next_queue;
    this_level++;
  }
  return next_levels[which_queue].len;
}

static i32 score_trailhead2(str8 map, i32 rows, i32 cols, i32 i, i32 j,
                            arena a) {
  i32s next_levels[2] = {0};
  push_neighbors(map, rows, cols, i, j, '1', &next_levels[0], &a);
  int which_queue = 0;
  char this_level = '1';
  while (this_level < '9') {
    int next_queue = (which_queue + 1) % 2;
    for (i32 qi = 0; qi < next_levels[which_queue].len; ++qi) {
      i16 inds[2] = {0};
      i32 front = next_levels[which_queue].data[qi];
      memcpy(inds, &front, sizeof(i16) * 2);
      i = inds[0];
      j = inds[1];
      push_neighbors(map, rows, cols, inds[0], inds[1], this_level + 1,
                     &next_levels[next_queue], &a);
    }
    next_levels[which_queue].len = 0;
    which_queue = next_queue;
    this_level++;
  }
  return next_levels[which_queue].len;
}

static i64 solve_part1(str8 map, i32 rows, i32 cols, arena a) {
  i64 sum_scores = 0;
  for (i32 i = 0; i < rows; ++i) {
    for (i32 j = 0; j < cols; ++j) {
      if (map.data[INDEX(i, j, cols)] == '0') {
        sum_scores += score_trailhead(map, rows, cols, i, j, a);
      }
    }
  }

  return sum_scores;
}

static i64 solve_part2(str8 map, i32 rows, i32 cols, arena a) {
  i64 sum_scores = 0;
  for (i32 i = 0; i < rows; ++i) {
    for (i32 j = 0; j < cols; ++j) {
      if (map.data[INDEX(i, j, cols)] == '0') {
        sum_scores += score_trailhead2(map, rows, cols, i, j, a);
      }
    }
  }

  return sum_scores;
}

void day10(input_pipe *pipe, arena a) {
  input_type input = parse_input(pipe, &a);
  i64 part1_result = solve_part1(input.contents, input.rows, input.cols, a);
  printf("Day 10, Part 1: sum = %ld\n", part1_result);
  i64 part2_result = solve_part2(input.contents, input.rows, input.cols, a);
  printf("Day 10, Part 2: sum = %ld\n", part2_result);
}
