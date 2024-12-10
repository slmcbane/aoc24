#include "common.h"

typedef struct {
  i16 row;
  i16 col;
} coordinate_type;

typedef union {
  coordinate_type coord;
  i32 packed;
} coord_union;

static void process_line(str8 line, i16 row, i32s antennas[], arena *a) {
  assert(row < 50);
  for (i32 j = 0; j < line.len; ++j) {
    if (line.data[j] == '.') {
      continue;
    }
    coord_union next = {.coord = {.row = row, .col = j}};
    i32s_push(antennas + (line.data[j] - '0'), next.packed, a);
  }
}

static bool in_bounds(i16 i, i16 j, i16 rows, i16 cols) {
  return i >= 0 && i < rows && j >= 0 && j < cols;
}

static i32 nodes_for_pair(i16 i1, i16 j1, i16 i2, i16 j2, i16 rows, i16 cols,
                          byte antinodes[]) {
  coord_union out = {0};
  i16 di = i2 - i1;
  i16 dj = j2 - j1;

  // Check part1 antinodes
  i16 i3 = i1 - di;
  i16 j3 = j1 - dj;
  if (in_bounds(i3, j3, rows, cols) && !(antinodes[i3 * cols + j3] & 1)) {
    antinodes[i3 * cols + j3] |= 1;
    out.coord.row += 1;
  }

  i16 i4 = i2 + di;
  i16 j4 = j2 + dj;
  if (in_bounds(i4, j4, rows, cols) && !(antinodes[i4 * cols + j4] & 1)) {
    antinodes[i4 * cols + j4] |= 1;
    out.coord.row += 1;
  }

  // Now check all antinodes for part 2.
  while (in_bounds(i1, j1, rows, cols)) {
    i1 -= di;
    j1 -= dj;
  }
  i1 += di;
  j1 += dj;

  while (in_bounds(i2, j2, rows, cols)) {
    i2 += di;
    j2 += dj;
  }

  while (i1 != i2) {
    if (!(antinodes[i1 * cols + j1] & 2)) {
      antinodes[i1 * cols + j1] |= 2;
      out.coord.col += 1;
    }
    i1 += di;
    j1 += dj;
  }
  assert(j1 == j2);

  return out.packed;
}

static i32 count_antinodes(const i32s antennas[], i16 rows, i16 cols) {
  assert(rows <= 50 && cols <= 50);
  byte antinodes[2500] = {0};
  i32 out = 0;

  for (i32 ai = 0; ai < 75; ++ai) {
    for (i32 i = 0; i < antennas[ai].len; ++i) {
      for (i32 j = i + 1; j < antennas[ai].len; ++j) {
        coord_union coord1 = {.packed = antennas[ai].data[i]};
        coord_union coord2 = {.packed = antennas[ai].data[j]};
        out +=
            nodes_for_pair(coord1.coord.row, coord1.coord.col, coord2.coord.row,
                           coord2.coord.col, rows, cols, antinodes);
      }
    }
  }

  return out;
}

void day8(input_pipe *pipe, arena a) {
  i16 rows = 0;
  i16 cols = 0;
  i32s antennas[75] = {0};

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

  coord_union packed_counts = {.packed = count_antinodes(antennas, rows, cols)};
  i32 part1_count = packed_counts.coord.row;
  i32 part2_count = packed_counts.coord.col;
  printf("Day 8, Part 1: count = %d\n", part1_count);
  assert(part1_count == 301);
  printf("Day 8, Part 2: count = %d\n", part2_count);
  assert(part2_count == 1019);
}
