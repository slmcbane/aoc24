#include "common.h"

#include <fcntl.h>
#include <unistd.h>

static void fill_pipe(input_pipe *pipe) {
  assert(pipe->input_start == pipe->input_end);
  if (pipe->eof) {
    return;
  }

  ssize_t read_count =
      read(pipe->fd, pipe->input_page, sizeof(pipe->input_page));
  assert(read_count >= 0 && "error in read call");
  pipe->input_start = 0;
  pipe->input_end = read_count;
  if (read_count == 0) {
    pipe->eof = true;
  }
}

void input_pipe_init(input_pipe *pipe, const char *file_name) {
  int fd = open(file_name, O_RDONLY);
  if (fd == -1) {
    fprintf(stderr, "Failed to open file %s\n", file_name);
    exit(1);
  }

  pipe->fd = fd;
  pipe->input_start = 0;
  pipe->input_end = 0;
  pipe->eof = false;

  fill_pipe(pipe);
}

void input_pipe_deinit(input_pipe *pipe) { close(pipe->fd); }

int input_pipe_next(input_pipe *pipe) {
  if (__builtin_expect(pipe->input_start == pipe->input_end, 0)) {
    fill_pipe(pipe);
  }

  if (__builtin_expect(pipe->eof, 0)) {
    return -1;
  }

  return pipe->input_page[pipe->input_start++];
}

getline_result input_pipe_getline(input_pipe *pipe, arena *a) {
  void *endline = memchr(pipe->input_page + pipe->input_start, '\n',
                         pipe->input_end - pipe->input_start);

  if (endline) {
    byte *begin = pipe->input_page + pipe->input_start;
    int off = (byte *)endline - begin;
    pipe->input_start += off + 1;
    return (getline_result){
        .str =
            (str8){
                .data = begin,
                .len = off,
            },
        .in_arena = false,
    };
  }

  str8 first_part = {.data = new (a, byte, pipe->input_end - pipe->input_start),
                     .len = pipe->input_end - pipe->input_start};
  memcpy(first_part.data, pipe->input_page + pipe->input_start,
         pipe->input_end - pipe->input_start);
  do {
    pipe->input_start = pipe->input_end = 0;
    fill_pipe(pipe);
    endline =
        memchr(pipe->input_page, '\n', pipe->input_end - pipe->input_start);
    if (!endline) {
      str8 next_part = {.data = pipe->input_page, .len = pipe->input_end};
      first_part = str8_concat(first_part, next_part, a);
    }
  } while (endline == NULL && !pipe->eof);

  if (endline) {
    assert(!pipe->eof);
    int off = (byte *)endline - pipe->input_page;
    pipe->input_start = off + 1;
    str8 next_part = {.data = pipe->input_page, .len = off};
    return (getline_result){.str = str8_concat(first_part, next_part, a),
                            .in_arena = true};
  } else {
    assert(pipe->eof);
    assert(pipe->input_start == pipe->input_end);
    return (getline_result){.str = first_part, .in_arena = true};
  }
}
