#include "common.h"

typedef struct {
  i32 num_chars;
  i32 a;
  i32 b;
} parsed_instr;

typedef struct {
  i32 num;
  i32 consumed;
} parse_int_result;

static parse_int_result try_parse_int(str8 str) {
  str8 num_str = {str.data, 0};
  for (i32 i = 0; i < str.len && i < 3; ++i) {
    if (str.data[i] >= '0' && str.data[i] <= '9') {
      num_str.len++;
    } else {
      break;
    }
  }

  if (num_str.len == 0) {
    return (parse_int_result){-1, 0};
  }

  return (parse_int_result){str8_to_i64(num_str), num_str.len};
}

static parsed_instr try_parse(str8 line) {
  assert(line.data[0] == 'm');
  if (line.len < 8) {
    return (parsed_instr){line.len, 0, 0};
  }

  if (line.data[1] != 'u') {
    return (parsed_instr){1, 0, 0};
  } else if (line.data[2] != 'l') {
    return (parsed_instr){2, 0, 0};
  } else if (line.data[3] != '(') {
    return (parsed_instr){3, 0, 0};
  }

  i32 i = 4;
  parse_int_result first_result =
      try_parse_int((str8){line.data + i, line.len - i});
  if (first_result.num == -1) {
    return (parsed_instr){4 + first_result.consumed, 0, 0};
  }

  // fprintf(stderr, "successfully parsed first number: %d\n",
  // first_result.num);
  i += first_result.consumed;
  if (i == line.len || line.data[i] != ',') {
    return (parsed_instr){i, 0, 0};
  }
  i++;

  parse_int_result second_result =
      try_parse_int((str8){line.data + i, line.len - i});
  if (second_result.num == -1) {
    return (parsed_instr){i + second_result.consumed, 0, 0};
  }

  // fprintf(stderr, "successfully parsed second number: %d\n",
  // second_result.num);
  i += second_result.consumed;
  if (i == line.len || line.data[i] != ')') {
    return (parsed_instr){i, 0, 0};
  }

  return (parsed_instr){i, first_result.num, second_result.num};
}

typedef enum { DO, DONT, NEITHER } conditional_type;

typedef struct {
  conditional_type cond;
  i32 consumed;
} conditional_parse_result;

conditional_parse_result parse_conditional(str8 in) {
  assert(in.data[0] == 'd');
  if (in.len < 4) {
    return (conditional_parse_result){NEITHER, in.len};
  }

  if (in.data[1] != 'o') {
    return (conditional_parse_result){NEITHER, 2};
  }

  if (in.data[2] == '(' && in.data[3] == ')') {
    return (conditional_parse_result){DO, 4};
  }

  if (in.len < 7) {
    return (conditional_parse_result){NEITHER, in.len};
  }

  if (in.data[2] != 'n') {
    return (conditional_parse_result){NEITHER, 3};
  }

  if (in.data[3] != '\'') {
    return (conditional_parse_result){NEITHER, 4};
  }

  if (in.data[4] != 't') {
    return (conditional_parse_result){NEITHER, 5};
  }

  if (in.data[5] != '(') {
    return (conditional_parse_result){NEITHER, 6};
  }

  if (in.data[6] != ')') {
    return (conditional_parse_result){NEITHER, 7};
  }

  return (conditional_parse_result){DONT, 7};
}

typedef struct {
  i64 part1;
  i64 part2;
  bool enabled;
} line_state;

static line_state line_sum(str8 line, line_state state_in) {
  line_state out = state_in;
  for (i32 i = 0; i < line.len; ++i) {
    if (line.data[i] == 'm') {
      parsed_instr this_instr = try_parse((str8){line.data + i, line.len - i});
      i64 inc = (i64)this_instr.a * this_instr.b;
      out.part1 += inc;
      if (out.enabled) {
        out.part2 += inc;
      }
      i += this_instr.num_chars - 1;
    } else if (line.data[i] == 'd') {
      conditional_parse_result result =
          parse_conditional((str8){line.data + i, line.len - i});
      if (result.cond == DO) {
        out.enabled = true;
      } else if (result.cond == DONT) {
        out.enabled = false;
      }
      i += result.consumed - 1;
    }
  }

  return out;
}

void day3(signal act, str8 next_input, arena *a, arena scratch) {
  static line_state state;
  if (act == BEGIN_SIGNAL) {
    state.part1 = 0;
    state.part2 = 0;
    state.enabled = true;
    return;
  } else if (act == END_SIGNAL) {
    printf("Day 3, Part 1: sum = %ld\n", state.part1);
    printf("Day 3, Part 2: sum = %ld\n", state.part2);
    return;
  }

  state = line_sum(next_input, state);
}
