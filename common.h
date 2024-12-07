#ifndef AOC_COMMON_H
#define AOC_COMMON_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef ptrdiff_t size;
typedef uint32_t u32;
typedef uint64_t u64;
typedef unsigned char byte;

typedef struct {
  byte *begin;
  byte *end;
} arena;

// with credit to Chris Wellons' excellent article on arena allocation
// https://nullprogram.com/blog/2023/09/27/
void *arena_allocate(arena *a, size sz, size align, size count);

#define new(a, T, n) ((T *)arena_allocate(a, sizeof(T), _Alignof(T), n))

typedef struct {
  byte *data;
  i32 len;
} str8;

i64 str8_to_i64(str8 s);
void fprint_str8(FILE *f, str8 s);
str8 str8_concat(str8 a, str8 b, arena *arena);

typedef struct {
  byte input_page[4096];
  int fd;
  i32 input_start;
  i32 input_end;
  bool eof;
} input_pipe;

void input_pipe_init(input_pipe *pipe, const char *file_name);

typedef struct {
  str8 str;
  bool in_arena;
} getline_result;

// Returns a view of the next line if it is entirely in the pipe's input buffer,
// otherwise constructs the line in the arena.
getline_result input_pipe_getline(input_pipe *pipe, arena *a);
int input_pipe_next(input_pipe *pipe);
void input_pipe_deinit(input_pipe *pipe);

typedef struct {
  str8 str;
  i32 cap;
} str8_builder;

str8_builder str8_builder_init(arena *a, i32 count);
void str8_builder_push(str8_builder *b, char c, arena *a);
void str8_builder_append(str8_builder *b, str8 s, arena *a);

typedef struct {
  str8 *data;
  i32 len;
  i32 cap;
} str8s;

str8 *str8s_push(str8s *strs, arena *a);
str8s str8_split(str8 s, arena *a);

typedef struct {
  i32 *data;
  i32 len;
  i32 cap;
} i32s;

void i32s_push(i32s *is, i32 i, arena *a);

#endif // AOC_COMMON_H
