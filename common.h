#ifndef AOC_COMMON_H
#define AOC_COMMON_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int32_t i32;
typedef int64_t i64;
typedef ptrdiff_t size;
typedef uint32_t u32;
typedef uint64_t u64;

typedef enum { BEGIN_SIGNAL, DATA_SIGNAL, END_SIGNAL } signal;

typedef struct {
  char *begin;
  char *end;
} arena;

// with credit to Chris Wellons' excellent article on arena allocation
// https://nullprogram.com/blog/2023/09/27/
inline void *arena_allocate(arena *a, size sz, size align, size count) {
  size pad = -(uintptr_t)a->begin & (align - 1);
  size avail = a->end - a->begin - pad;
  if (__builtin_expect(avail < 0 || count > avail / sz, 0)) {
    fprintf(stderr, "exhausted arena, aborting\n");
    exit(255);
  }
  void *out = a->begin + pad;
  a->begin += pad + count * sz;
  return out;
}

#define new(a, T, n) ((T *)arena_allocate(a, sizeof(T), _Alignof(T), n))

typedef struct {
  char *data;
  i32 len;
} str8;

typedef struct {
  str8 str;
  i32 cap;
} str8_builder;

inline str8_builder str8_builder_init(arena *a, i32 count) {
  return (str8_builder){.str = {.data = new (a, char, count), .len = 0},
                        .cap = count};
}

inline void str8_builder_push(str8_builder *b, char c, arena *a) {
  if (b->str.len == b->cap) {
    char *old_data = b->str.data;
    b->cap *= 2;
    b->str.data = new (a, char, b->cap);
    memcpy(b->str.data, old_data, b->str.len);
  }

  b->str.data[b->str.len++] = c;
}

typedef struct {
  str8 *data;
  i32 len;
  i32 cap;
} str8s;

inline str8s str8s_new() { return (str8s){0}; }

inline str8 *str8s_push(str8s *s, arena *a) {
  if (s->len == s->cap) {
    s->cap = s->cap ? s->cap * 2 : 8;
    str8 *old_data = s->data;
    s->data = new (a, str8, s->cap);
    for (i32 i = 0; i < s->len; i++) {
      s->data[i] = old_data[i];
    }
  }

  return &s->data[s->len++];
}

str8s split(str8 s, arena *a);

#endif // AOC_COMMON_H
