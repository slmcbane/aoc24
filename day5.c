#include "common.h"

typedef struct {
  i32s out_edges;
  i32 payload;
} graph_node;

typedef struct {
  graph_node nodes[100];
  i32 topo_order[100];
} graph_type;

/*static i32 topo_sort_node(graph_type *graph, graph_node *node, i32 next_index)
{ fprintf(stderr, "topo_sort_node for %ld\n", node - graph->nodes);
  node->payload = 2;
  for (i32 i = 0; i < node->out_edges.len; ++i) {
    graph_node *child = graph->nodes + node->out_edges.data[i];
    assert(child->payload != 2 && "cycle detected");
    if (child->payload == 0) {
      next_index = topo_sort_node(graph, child, next_index);
      assert(child->payload == 1);
    }
  }
  fprintf(stderr, "node %d in topo order is %ld\n", next_index,
          node - graph->nodes);
  graph->topo_order[node - graph->nodes] = next_index--;
  node->payload = 1;
  return next_index;
}

static void do_topo_sort(graph_type *graph) {
  i32 next_index = 99;
  for (i32 i = 0; i < 100; ++i) {
    if (graph->nodes[i].payload == 1) {
      continue;
    }

    next_index = topo_sort_node(graph, graph->nodes + i, next_index);
  }

  assert(next_index == -1);

  for (i32 i = 0; i < 100; ++i) {
    graph->nodes[i].payload = 0;
  }
} */

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
  i32 node_pos = node->payload;
  assert(node_pos != 0);

  for (i32 i = 0; i < node->out_edges.len; ++i) {
    i32 child = node->out_edges.data[i];
    i32 child_pos = graph->nodes[child].payload;
    if (child_pos == 0) {
      continue;
    } else if (child_pos < node_pos) {
      return false;
    }
  }

  return true;
}

static i32 check_line(graph_type *graph, str8 next_line, arena a) {
  i32s nums = get_numbers(next_line, &a);
  for (i32 i = 0; i < nums.len; ++i) {
    assert(nums.data[i] < 100);
    graph->nodes[nums.data[i]].payload = i + 1;
  }

  bool correct = true;
  for (i32 i = 0; correct && i < nums.len; ++i) {
    correct &= check_ordering(graph, nums.data[i]);
  }

  for (i32 i = 0; i < nums.len; ++i) {
    graph->nodes[nums.data[i]].payload = 0;
  }

  if (correct) {
    i32 middle_value = nums.data[nums.len / 2];
    return middle_value;
  }

  return 0;
}

static i32 check_lines(graph_type *graph, input_pipe *pipe, arena a,
                       arena scratch) {
  i32 page_number_sum = 0;
  arena local = scratch;
  str8 next_line = input_pipe_getline(pipe, &local).str;
  assert(next_line.len != 0);
  while (next_line.len != 0) {
    page_number_sum += check_line(graph, next_line, a);
    local = scratch;
    next_line = input_pipe_getline(pipe, &local).str;
  }

  return page_number_sum;
}

void day5(input_pipe *pipe, arena a) {
  graph_type graph;
  arena scratch = {.begin = new (&a, byte, 512), .end = scratch.begin + 512};
  build_graph(&graph, pipe, &a, scratch);

  i32 page_number_sum = check_lines(&graph, pipe, a, scratch);
  printf("Day 5, Part 1: page number sum = %d\n", page_number_sum);
}
