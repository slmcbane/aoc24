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

static i64 count_occurrences(str8 str, i32 rows, i32 cols) {
  static const search_dir dirs[] = {{0, 1}, {0, -1}, {1, 0},  {-1, 0},
                                    {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
  i64 count = 0;
  for (i32 i = 0; i < rows; ++i) {
    for (i32 j = 0; j < cols; ++j) {
      if (str.data[IND(i, j)] == 'X') {
        for (unsigned k = 0; k < 8; ++k) {
          count += search(str, i, j, rows, cols, dirs[k]);
        }
      }
    }
  }

  return count;
}

void day4(signal act, str8 line, arena *a, arena scratch) {
  static i32 rows = 0;
  static i32 cols = 0;
  static str8_builder builder;

  if (act == BEGIN_SIGNAL) {
    rows = 0;
    cols = 0;
    builder = str8_builder_init(a, 1024);
    return;
  } else if (act == END_SIGNAL) {
    i64 count = count_occurrences(builder.str, rows, cols);
    printf("Day 4, Part 1: count = %ld\n", count);
    return;
  }

  assert(line.len != 0);
  if (cols == 0) {
    cols = line.len;
  }
  str8_builder_append(&builder, line, a);
  rows++;
}
