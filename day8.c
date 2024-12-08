#include "common.h"

typedef struct {
  i16 col;
  i16 freq;
} antenna_type;

typedef union {
  antenna_type antenna;
  i32 packed;
} antenna_union;

static void process_line(str8 line, i16 row, i32s antennas[], arena *a) {
  assert(row < 50);
  i32s *this_row_antennas = antennas + row;
  for (i32 j = 0; j < line.len; ++j) {
    if (line.data[j] == '.') {
      continue;
    }
    antenna_union next = {.antenna = {.col = j, .freq = line.data[j]}};
    i32s_push(this_row_antennas, next.packed, a);
  }
}

static bool sort_compare(i32 a, i32 b) {
  antenna_union a_ = {.packed = a};
  antenna_union b_ = {.packed = b};
  assert(a_.antenna.col != b_.antenna.col);
  return a_.antenna.col < b_.antenna.col;
}

static void bubble_sort(i32 *begin, i32 len) {
  while (true) {
    bool modified = false;
    for (i32 i = 0; i < len - 1; ++i) {
      if (sort_compare(begin[i + 1], begin[i])) {
        i32 temp = begin[i];
        begin[i] = begin[i + 1];
        begin[i + 1] = temp;
        modified = true;
      }
    }
    if (!modified) {
      break;
    }
  }
}

static void finalize_antennas(i32s antennas[], i32 rows) {
  for (i32 i = 0; i < rows; ++i) {
    bubble_sort(antennas[i].data, antennas[i].len);
  }
}

static bool linear_search(const i32s *row_antennas, i32 col, i16 freq) {
  for (i32 j = 0; j < row_antennas->len; ++j) {
    antenna_union a = {.packed = row_antennas->data[j]};
    if (a.antenna.col == col && a.antenna.freq == freq) {
      return true;
    }
    if (a.antenna.col > col) {
      break;
    }
  }
  return false;
}

static i32 lower_bound(i32 i, i32 rows) { return i / 2 + i % 2; }
static i32 upper_bound(i32 i, i32 rows) { return (rows + i + 1) / 2; }

static i32 antinode_check(const i32s antennas[], i32 rows, i32 cols, i32 i,
                          i32 j, bool *antinodes) {
  // printf("Checking for (%d, %d)\n", i, j);
  i32 i_lower = lower_bound(i, rows);
  i32 i_upper = upper_bound(i, rows);
  i32 j_lower = lower_bound(j, cols);
  i32 j_upper = upper_bound(j, cols);

  for (i32 i1 = i_lower; i1 < i_upper; ++i1) {
    for (i32 ai = 0; ai < antennas[i1].len; ++ai) {
      antenna_union a = {.packed = antennas[i1].data[ai]};
      i32 j1 = a.antenna.col;
      if (j1 >= j_upper) {
        break;
      } else if (j1 != j && j1 >= j_lower) {
        // printf("  candidate antenna at (%d, %d) with freq %d\n", i1, j1,
        //       (int)a.antenna.freq);
        // (i1, j1) are the coordinates of one of a _possible_ antenna pair.
        i16 freq = a.antenna.freq;
        // (i2, j2) are the coordinates to check for a matching antenna
        i32 i2 = 2 * i1 - i;
        i32 j2 = 2 * j1 - j;
        assert(i2 >= 0 && i2 < rows && j2 >= 0 && j2 < cols);
        if (linear_search(antennas + i2, j2, freq)) {
          // We have a matching antenna - mark this antinode and check for an
          // opposing one.
          // printf("  found matching antenna at (%d, %d)\n", i2, j2);
          antinodes[i * cols + j] = true;
          i32 i3 = i2 + i1 - i;
          i32 j3 = j2 + j1 - j;
          if (i3 >= 0 && i3 < rows && j3 >= 0 && j3 < cols) {
            // printf("  checking for opposing antinode at (%d, %d)\n", i3, j3);
            if (antinodes[i3 * cols + j3]) {
              // printf("    opposing antinode already processed\n");
              return 1;
            } else {
              // printf("    found opposing antinode, return 2\n");
              antinodes[i3 * cols + j3] = true;
              return 2;
            }
          }
          return 1;
        }
      }
    }
  }

  // printf("Returning 0\n");
  return 0;
}

static i32 count_antinodes(const i32s antennas[], i32 rows, i32 cols, arena a) {
  bool *antinodes = new (&a, bool, rows *cols);
  memset(antinodes, 0, sizeof(bool) * rows * cols);

  i32 count = 0;
  for (i32 i = 0; i < rows; ++i) {
    for (i32 j = 0; j < cols; ++j) {
      if (antinodes[i * cols + j]) {
        continue;
      }
      count += antinode_check(antennas, rows, cols, i, j, antinodes);
    }
  }

  return count;
}

void day8(input_pipe *pipe, arena a) {
  i16 rows = 0;
  i16 cols = 0;
  i32s antennas[50] = {0};

  arena scratch = {.begin = new (&a, byte, 128), .end = scratch.begin + 128};

  while (!pipe->eof) {
    arena scratch_ = scratch;
    str8 line = input_pipe_getline(pipe, &scratch_).str;
    if (!line.len) {
      break;
    }
    if (cols == 0) {
      cols = line.len;
    }

    process_line(line, rows++, antennas, &a);
  }
  finalize_antennas(antennas, rows);

  i32 part1_count = count_antinodes(antennas, rows, cols, a);
  printf("Day 8, Part 1: count = %d\n", part1_count);
}
