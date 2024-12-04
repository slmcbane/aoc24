#include "common.h"

str8 *str8s_push(str8s *s, arena *a) {
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

str8_builder str8_builder_init(arena *a, i32 count) {
  return (str8_builder){.str = {.data = new (a, char, count), .len = 0},
                        .cap = count};
}

void str8_builder_push(str8_builder *b, char c, arena *a) {
  if (b->str.len == b->cap) {
    char *old_data = b->str.data;
    b->cap *= 2;
    b->str.data = new (a, char, b->cap);
    memcpy(b->str.data, old_data, b->str.len);
  }

  b->str.data[b->str.len++] = c;
}

void str8_builder_append(str8_builder *b, str8 s, arena *a) {
  i32 new_len = b->str.len + s.len;
  if (b->cap < new_len) {
    while (b->cap < new_len) {
      b->cap *= 2;
    }
    char *old_data = b->str.data;
    b->str.data = new (a, char, b->cap);
    memcpy(b->str.data, old_data, b->str.len);
  }
  memcpy(b->str.data + b->str.len, s.data, s.len);
  b->str.len = new_len;
}

str8s str8_split(str8 s, arena *a) {
  const char spaces[] = " \t\n\r";
  str8s tokens = {0};
  i32 token_start = 0;
  i32 token_len = 0;

  for (i32 i = 0; i < s.len; ++i, ++token_len) {
    for (i32 j = 0; j < (i32)sizeof(spaces); ++j) {
      if (s.data[i] == spaces[j]) {
        if (token_len) {
          str8 *token = str8s_push(&tokens, a);
          token->data = s.data + token_start;
          token->len = token_len;
        }
        token_start = i + 1;
        token_len = -1;
        break;
      }
    }
  }

  if (token_len) {
    str8 *token = str8s_push(&tokens, a);
    token->data = s.data + token_start;
    token->len = token_len;
  }

  return tokens;
}

i64 str8_to_i64(str8 s) {
  i64 result = 0;
  i64 power = 1;

  for (i32 i = s.len - 1; i >= 0; --i, power *= 10) {
    assert(s.data[i] >= '0' && s.data[i] <= '9' && "non-digit character");
    result += (s.data[i] - '0') * power;
  }

  return result;
}

void fprint_str8(FILE *f, str8 s) {
  for (i32 i = 0; i < s.len; ++i) {
    fputc(s.data[i], f);
  }
}
