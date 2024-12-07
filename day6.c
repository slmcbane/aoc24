#include "common.h"

typedef struct {
  int16_t x;
  int16_t y;
} coordinate;

typedef struct {
  coordinate *data;
  i32 exp;
} obstruction_table;

static u32 coord_hash(coordinate in) {
  u32 x;
  memcpy(&x, &in, sizeof(u32));
  x ^= x << 13;
  x ^= x >> 17;
  return x ^ (x << 5);
}

typedef struct hashtable_node hashtable_node;

struct hashtable_node {
  hashtable_node *children[4];
  intptr_t key;
  intptr_t value;
};

static coordinate *insert(obstruction_table *obstructions, coordinate coord) {
  u32 hash = coord_hash(coord);
  u32 mask = ((u32)1 << obstructions->exp) - 1;
  u32 index = hash & mask;
  u32 init_index = index;
  u32 step = (hash >> obstructions->exp) | 1;

  coordinate *this_coord = obstructions->data + index;
  while (this_coord->x != 0 || this_coord->y != 0) {
    index = (index + step) & mask;
    assert(index != init_index && "table full");
    this_coord = obstructions->data + index;
  }
  this_coord->x = coord.x + 1;
  this_coord->y = coord.y + 1;
  return this_coord;
}

static bool find(const obstruction_table *obstructions, coordinate coord) {
  u32 hash = coord_hash(coord);
  u32 mask = ((u32)1 << obstructions->exp) - 1;
  u32 index = hash & mask;
  u32 step = (hash >> obstructions->exp) | 1;

  coordinate *this_coord = obstructions->data + index;
  while (this_coord->x != 0 || this_coord->y != 0) {
    if (this_coord->x == coord.x + 1 && this_coord->y == coord.y + 1) {
      return true;
    }
    index = (index + step) & mask;
    this_coord = obstructions->data + index;
  }

  return false;
}

static void process_line(str8 line, i16 curr_y, coordinate *start_coord,
                         obstruction_table *obstructions) {
  byte *line_begin = line.data;
  byte *curr_pos = line_begin;
  i32 remaining = line.len;
  while (remaining) {
    byte *next_obstruction = memchr(curr_pos, '#', remaining);
    if (!next_obstruction) {
      break;
    }
    insert(obstructions,
           (coordinate){(i16)(next_obstruction - line_begin), curr_y});
    remaining -= next_obstruction - curr_pos - 1;
    curr_pos = next_obstruction + 1;
  }

  if (start_coord->x == -1) {
    byte *pos = memchr(line_begin, '^', line.len);
    if (pos) {
      *start_coord = (coordinate){(i16)(pos - line_begin), curr_y};
    }
  }
}

static bool in_bounds(coordinate current, i16 rows, i16 columns) {
  return current.x >= 0 && current.x < columns && current.y >= 0 &&
         current.y < rows;
}

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

static int advance(coordinate *current, int dir,
                   obstruction_table obstructions) {
  static const i8 x_inc[4] = {0, 1, 0, -1};
  static const i8 y_inc[4] = {-1, 0, 1, 0};
  coordinate next = {current->x + x_inc[dir], current->y + y_inc[dir]};
  if (find(&obstructions, next)) {
    dir = (dir + 1) & 3;
  } else {
    *current = next;
  }
  return dir;
}

static i32 do_walk(coordinate start_pos, i16 rows, i16 cols,
                   obstruction_table obstructions, byte *visited) {
  i32 visited_count = 0;
  int dir = UP;
  while (in_bounds(start_pos, rows, cols)) {
    int index = start_pos.y * cols + start_pos.x;
    if (__builtin_expect(visited[index] & (1 << dir), 0)) {
      return 0;
    }
    visited_count += visited[index] == 0;
    visited[index] = visited[index] | (1 << dir);
    dir = advance(&start_pos, dir, obstructions);
  }

  return visited_count;
}

// Return the number of visited squares, mark them with a true byte in 'visited'
static i32 solve_part1(coordinate start_pos, i16 rows, i16 cols,
                       obstruction_table obstructions, byte *visited) {
  memset(visited, 0, rows * cols);
  return do_walk(start_pos, rows, cols, obstructions, visited);
}

static i32 solve_part2(coordinate start_pos, i16 rows, i16 cols,
                       obstruction_table obstructions, const byte *to_test,
                       byte *visited) {
  i32 count = 0;
  for (i16 i = 0; i < rows; ++i) {
    for (i16 j = 0; j < cols; ++j) {
      if (!to_test[i * cols + j]) {
        continue;
      }
      coordinate *inserted = insert(&obstructions, (coordinate){j, i});
      memset(visited, 0, rows * cols);
      i32 visited_count = do_walk(start_pos, rows, cols, obstructions, visited);
      count += visited_count == 0;
      *inserted = (coordinate){0, 0};
    }
  }

  return count;
}

void day6(input_pipe *pipe, arena a) {
  obstruction_table obstructions = {.data = new (&a, coordinate, 1 << 11),
                                    .exp = 11};
  memset(obstructions.data, 0, sizeof(coordinate) << 11);
  coordinate start_pos = {-1, -1};

  i16 columns = 0;
  i16 curr_y = 0;
  while (!pipe->eof) {
    arena scratch = a;
    str8 line = input_pipe_getline(pipe, &scratch).str;
    if (line.len == 0) {
      break;
    }
    columns = line.len;
    process_line(line, curr_y, &start_pos, &obstructions);
    curr_y++;
  }
  i16 rows = curr_y;

  byte *visited = new (&a, byte, rows * columns);
  i32 visited_count =
      solve_part1(start_pos, rows, columns, obstructions, visited);
  printf("Day 6, Part 1: visited count = %d\n", visited_count);
  assert(visited_count == 4967);

  byte *part2_visited = new (&a, byte, rows * columns);
  i32 part2_count = solve_part2(start_pos, rows, columns, obstructions, visited,
                                part2_visited);
  printf("Day 6, Part 2: count = %d\n", part2_count);
  assert(part2_count == 1789);
}
