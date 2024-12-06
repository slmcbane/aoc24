#include "common.h"

#define IND(i, j) ((i) * cols + (j))

typedef struct {
  i32 row_offset;
  i32 col_offset;
} search_dir;

static bool search(str8 str, i32 i, i32 j, i32 rows, i32 cols, search_dir dir) {
  assert(str.data[IND(i, j)] == 'X');
  i += dir.row_offset;
  j += dir.col_offset;
  if (i < 0 || i >= rows || j < 0 || j >= cols) {
    return false;
  } else if (str.data[IND(i, j)] != 'M') {
    return false;
  }

  i += dir.row_offset;
  j += dir.col_offset;
  if (i < 0 || i >= rows || j < 0 || j >= cols) {
    return false;
  } else if (str.data[IND(i, j)] != 'A') {
    return false;
  }

  i += dir.row_offset;
  j += dir.col_offset;
  if (i < 0 || i >= rows || j < 0 || j >= cols) {
    return false;
  } else if (str.data[IND(i, j)] != 'S') {
    return false;
  }

  return true;
}

typedef struct {
  i64 xmas_count;
  i64 x_mas_count;
} counts;

#define check_mas_down(str, i, j)                                              \
  (str.data[IND(i + 1, j + 1)] == 'A' && str.data[IND(i + 2, j + 2)] == 'S')

#define check_mas_up(str, i, j)                                                \
  (str.data[IND(i - 1, j + 1)] == 'A' && str.data[IND(i - 2, j + 2)] == 'S')

#define check_sam_down(str, i, j)                                              \
  (str.data[IND(i + 1, j + 1)] == 'A' && str.data[IND(i + 2, j + 2)] == 'M')

#define check_sam_up(str, i, j)                                                \
  (str.data[IND(i - 1, j + 1)] == 'A' && str.data[IND(i - 2, j + 2)] == 'M')

static counts count_occurrences(str8 str, i32 rows, i32 cols) {
  static const search_dir dirs[] = {{0, 1}, {0, -1}, {1, 0},  {-1, 0},
                                    {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
  i64 xmas_count = 0;
  i64 x_mas_count = 0;
  for (i32 i = 0; i < rows; ++i) {
    for (i32 j = 0; j < cols; ++j) {
      if (str.data[IND(i, j)] == 'X') {
        for (unsigned k = 0; k < 8; ++k) {
          xmas_count += search(str, i, j, rows, cols, dirs[k]);
        }
      } else if (i < rows - 2 && j < cols - 2) {
        if ((str.data[IND(i, j)] == 'M' && check_mas_down(str, i, j)) ||
            (str.data[IND(i, j)] == 'S' && check_sam_down(str, i, j))) {
          if ((str.data[IND(i + 2, j)] == 'M' && check_mas_up(str, i + 2, j)) ||
              (str.data[IND(i + 2, j)] == 'S' && check_sam_up(str, i + 2, j))) {
            x_mas_count++;
          }
        }
      }
    }
  }

  return (counts){xmas_count, x_mas_count};
}

void day4(input_pipe *pipe, arena a) {
  i32 rows = 0;
  i32 cols = 0;
  arena scratch = {.begin = new (&a, byte, 256), scratch.begin + 256};
  str8_builder builder = str8_builder_init(&a, 1024);

  while (!pipe->eof) {
    arena for_line = scratch;
    str8 line = input_pipe_getline(pipe, &for_line).str;
    if (cols == 0) {
      cols = line.len;
    } else if (line.len == 0) {
      break;
    }
    str8_builder_append(&builder, line, &a);
    rows++;
  }

  counts result = count_occurrences(builder.str, rows, cols);
  printf("Day 4, Part 1: count = %ld\n", result.xmas_count);
  printf("Day 4, Part 2: count = %ld\n", result.x_mas_count);
  assert(result.xmas_count == 2599);
  assert(result.x_mas_count == 1948);
}
