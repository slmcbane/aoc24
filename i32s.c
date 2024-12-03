#include "common.h"

void i32s_push(i32s *is, i32 i, arena *a) {
  if (is->len == is->cap) {
    is->cap = is->cap ? is->cap * 2 : 8;
    i32 *old_data = is->data;
    is->data = new (a, i32, is->cap);
    for (i32 j = 0; j < is->len; j++) {
      is->data[j] = old_data[j];
    }
  }

  is->data[is->len++] = i;
}
