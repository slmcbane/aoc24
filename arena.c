#include "common.h"

void *arena_allocate(arena *a, size sz, size align, size count) {
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
