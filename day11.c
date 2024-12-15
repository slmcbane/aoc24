#include "common.h"

typedef struct {
  i64 value;
  i64 num_steps;
} cache_key;

typedef struct hash_node hash_node;

struct hash_node {
  cache_key key;
  i64 stone_count;
  hash_node *children[4];
};

typedef struct {
  hash_node *root;
} cache_type;

static uint64_t hash_key(cache_key key) {
  uint64_t x = key.value;
  x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9;
  uint64_t y = key.num_steps;
  y = (y ^ (y << 28)) * 0x94d049bb133111eb;
  return x ^ y;
}

static i64 *cache_lookup(cache_type *cache, cache_key key, arena *a) {
  uint64_t hash = hash_key(key);
  hash_node **dst = &cache->root;
  while (*dst) {
    if ((*dst)->key.value == key.value &&
        (*dst)->key.num_steps == key.num_steps) {
      return &(*dst)->stone_count;
    }
    dst = &(*dst)->children[hash & 3];
    hash >>= 2;
  }
  *dst = new (a, hash_node, 1);
  memset(*dst, 0, sizeof(hash_node));
  (*dst)->key = key;
  return &(*dst)->stone_count;
}

static const u64 powers_of_10[] = {
    1,           10,           100,           1000,           10000,
    100000,      1000000,      10000000,      100000000,      1000000000,
    10000000000, 100000000000, 1000000000000, 10000000000000, 100000000000000};

static int intlog2(u64 n) {
  _Static_assert(sizeof(u64) == sizeof(unsigned long),
                 "assumes long is 64 bits");
  assert(n != 0);
  int leading_zeros = __builtin_clzl(n);
  return 63 - leading_zeros;
}

static int intlog10(u64 n) {
  assert(n != 0);
  int ceil = ((intlog2(n) + 1) * 1233) >> 12;
  return ceil - (n < powers_of_10[ceil]);
}

bool get_even_digits_halves(i64 value, i64 *part1, i64 *part2) {
  int log10 = intlog10(value);
  if (log10 % 2 == 0) {
    return false;
  }
  int ndigits = (log10 + 1) / 2;
  *part1 = value / powers_of_10[ndigits];
  *part2 = value % powers_of_10[ndigits];
  return true;
}

static i64 count_stones(i64 value, i64 num_steps, cache_type *cache, arena *a) {
  if (num_steps == 0) {
    return 1;
  }

  if (value == 0) {
    return count_stones(1, num_steps - 1, cache, a);
  }

  i64 part1, part2;
  if (!get_even_digits_halves(value, &part1, &part2)) {
    return count_stones(value * 2024, num_steps - 1, cache, a);
  }

  i64 *cached = NULL;
  if (num_steps >= 4) {
    cached = cache_lookup(cache, (cache_key){value, num_steps}, a);
    if (*cached != 0) {
      // printf("cache hit for (%ld, %ld): %ld\n", value, num_steps, *cached);
      return *cached;
    }
  }

  i64 result = count_stones(part1, num_steps - 1, cache, a) +
               count_stones(part2, num_steps - 1, cache, a);
  if (cached) {
    *cached = result;
  }

  return result;
}

void day11(input_pipe *pipe, arena a) {
  cache_type cache = {0};
  str8 line = input_pipe_getline(pipe, &a).str;
  str8s nums_strs = str8_split(line, &a);
  i64 part1_count = 0;
  for (i32 i = 0; i < nums_strs.len; ++i) {
    i64 value = str8_to_i64(nums_strs.data[i]);
    part1_count += count_stones(value, 25, &cache, &a);
  }
  printf("Day 11, Part 1: count = %ld\n", part1_count);

  i64 part2_count = 0;
  for (i32 i = 0; i < nums_strs.len; ++i) {
    i64 value = str8_to_i64(nums_strs.data[i]);
    part2_count += count_stones(value, 75, &cache, &a);
  }
  printf("Day 11, Part 2: count = %ld\n", part2_count);
}
