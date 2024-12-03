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
void *arena_allocate(arena *a, size sz, size align, size count);

#define new(a, T, n) ((T *)arena_allocate(a, sizeof(T), _Alignof(T), n))

typedef struct {
  char *data;
  i32 len;
} str8;

i64 str8_to_i64(str8 s);
void fprint_str8(FILE *f, str8 s);

typedef struct {
  str8 str;
  i32 cap;
} str8_builder;

str8_builder str8_builder_init(arena *a, i32 count);
void str8_builder_push(str8_builder *b, char c, arena *a);

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

typedef struct hashtable_node hashtable_node;

struct hashtable_node {
  hashtable_node *children[4];
  intptr_t key;
  intptr_t value;
};

typedef struct {
  hashtable_node *root;
} hashtable;

typedef struct {
  hashtable_node *node;
  bool inserted;
} hashtable_result;

typedef bool (*ht_predicate)(intptr_t, intptr_t);
typedef u64 (*ht_hash)(intptr_t);

hashtable_result find_or_insert(hashtable *ht, intptr_t key, intptr_t value,
                                ht_hash hash, ht_predicate comp, arena *a);

#endif // AOC_COMMON_H
