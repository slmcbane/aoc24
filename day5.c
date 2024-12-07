#include "common.h"

typedef struct {
  i32s out_edges;
  i16 index;
  i16 in_degree;
} graph_node;

typedef struct {
  graph_node nodes[100];
} graph_type;

static void build_graph(graph_type *graph, input_pipe *pipe, arena *a,
                        arena scratch) {
  *graph = (graph_type){0};

  while (1) {
    arena for_line = scratch;
    str8 line = input_pipe_getline(pipe, &for_line).str;
    if (line.len == 0) {
      break;
    }
    assert(line.data[2] == '|' && line.len == 5);
    str8 token1 = {line.data, 2};
    str8 token2 = {line.data + 3, 2};
    i32 parent = str8_to_i64(token1);
    i32 child = str8_to_i64(token2);
    i32s_push(&graph->nodes[parent].out_edges, child, a);
  }
}

typedef struct queue_node queue_node;

struct queue_node {
  graph_node *n;
  queue_node *next;
};

typedef struct {
  queue_node *head;
  queue_node *tail;
} queue;

static void queue_push(queue *queue, graph_node *n, arena a) {
  queue_node *qn = new (&a, queue_node, 1);
  qn->n = n;
  qn->next = NULL;
  if (queue->head == NULL) {
    assert(queue->tail == NULL);
    queue->head = qn;
    queue->tail = qn;
  } else {
    queue->tail->next = qn;
    queue->tail = qn;
  }
}

static graph_node *queue_pop(queue *queue) {
  queue_node *head = queue->head;
  queue->head = head->next;
  if (queue->head == NULL) {
    queue->tail = NULL;
  }
  return head->n;
}

static void topo_sort_line(i32s *nums, graph_type *graph, arena a) {
  for (i32 i = 0; i < nums->len; ++i) {
    i32 node_number = nums->data[i];
    graph->nodes[node_number].in_degree = 1;
  }

  for (i32 i = 0; i < nums->len; ++i) {
    graph_node *node = graph->nodes + nums->data[i];
    for (i32 j = 0; j < node->out_edges.len; ++j) {
      graph_node *child = graph->nodes + node->out_edges.data[j];
      if (child->in_degree) {
        child->in_degree++;
      }
    }
  }

  queue q = {0};
  i32 in_q = 0;

  for (i32 i = 0; i < nums->len; ++i) {
    graph_node *node = graph->nodes + nums->data[i];
    if (node->in_degree == 1) {
      queue_push(&q, node, a);
      in_q++;
    }
  }

  i16 index = 0;
  while (in_q) {
    i32 new_in_q = 0;
    for (i32 i = 0; i < in_q; ++i) {
      graph_node *node = queue_pop(&q);
      assert(node->in_degree == 1);
      node->index = index++;
      node->in_degree = 0;
      for (i32 i = 0; i < node->out_edges.len; ++i) {
        graph_node *child = graph->nodes + node->out_edges.data[i];
        child->in_degree--;
        if (child->in_degree == 1) {
          queue_push(&q, child, a);
          new_in_q++;
        }
      }
    }

    in_q = new_in_q;
  }

  for (i32 i = 0; i < nums->len;) {
    graph_node *node = graph->nodes + nums->data[i];
    assert(node->in_degree == 0);
    if (node->index == i) {
      node->index = 0;
      i++;
    } else {
      i32 tmp = nums->data[i];
      nums->data[i] = nums->data[node->index];
      nums->data[node->index] = tmp;
    }
  }
}

static i32s get_numbers(str8 next_line, arena *a) {
  i32s nums = {0};
  for (i32 i = 0; i < next_line.len; i += 3) {
    str8 token = {next_line.data + i, 2};
    i32s_push(&nums, str8_to_i64(token), a);
  }
  assert(nums.len % 2 != 0);
  return nums;
}

static bool check_ordering(const graph_type *graph, i32 n) {
  const graph_node *node = graph->nodes + n;
  i16 node_pos = node->index;
  assert(node_pos != 0);

  for (i32 i = 0; i < node->out_edges.len; ++i) {
    i32 child = node->out_edges.data[i];
    i16 child_pos = graph->nodes[child].index;
    if (child_pos == 0) {
      continue;
    } else if (child_pos < node_pos) {
      return false;
    }
  }

  return true;
}

typedef struct {
  i32 middle_value;
  bool correct_line;
} line_result;

static line_result check_line(graph_type *graph, str8 next_line, arena a) {
  i32s nums = get_numbers(next_line, &a);
  for (i32 i = 0; i < nums.len; ++i) {
    assert(nums.data[i] < 100);
    graph->nodes[nums.data[i]].index = i + 1;
  }

  bool correct = true;
  for (i32 i = 0; correct && i < nums.len; ++i) {
    correct &= check_ordering(graph, nums.data[i]);
  }

  for (i32 i = 0; i < nums.len; ++i) {
    graph->nodes[nums.data[i]].index = 0;
  }

  if (correct) {
    i32 middle_value = nums.data[nums.len / 2];
    return (line_result){middle_value, true};
  }

  topo_sort_line(&nums, graph, a);
  i32 middle_value = nums.data[nums.len / 2];
  return (line_result){middle_value, false};
}

typedef struct {
  i32 part1_sum;
  i32 part2_sum;
} sums;

static sums check_lines(graph_type *graph, input_pipe *pipe, arena a,
                        arena scratch) {
  sums out = {0};
  arena local = scratch;
  str8 next_line = input_pipe_getline(pipe, &local).str;
  assert(next_line.len != 0);
  while (next_line.len != 0) {
    line_result this_line_result = check_line(graph, next_line, a);
    if (this_line_result.correct_line) {
      out.part1_sum += this_line_result.middle_value;
    } else {
      out.part2_sum += this_line_result.middle_value;
    }
    local = scratch;
    next_line = input_pipe_getline(pipe, &local).str;
  }

  return out;
}

void day5(input_pipe *pipe, arena a) {
  graph_type graph;
  arena scratch = {.begin = new (&a, byte, 512), .end = scratch.begin + 512};
  build_graph(&graph, pipe, &a, scratch);

  sums result = check_lines(&graph, pipe, a, scratch);
  printf("Day 5, Part 1: page number sum = %d\n", result.part1_sum);
  printf("Day 5, Part 2: page number sum = %d\n", result.part2_sum);
}
