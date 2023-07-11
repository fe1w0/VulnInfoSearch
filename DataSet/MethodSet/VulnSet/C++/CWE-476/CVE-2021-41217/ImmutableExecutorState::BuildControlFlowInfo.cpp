Status ImmutableExecutorState::BuildControlFlowInfo(const Graph* g,
                                                    ControlFlowInfo* cf_info) {
  const int num_nodes = g->num_node_ids();
  cf_info->frame_names.resize(num_nodes);
  std::vector<Node*> parent_nodes;
  parent_nodes.resize(num_nodes);
  std::vector<bool> visited;
  visited.resize(num_nodes);

  string frame_name;
  std::deque<Node*> ready;

  // Initialize with the root nodes.
  for (Node* n : g->nodes()) {
    if (n->in_edges().empty()) {
      visited[n->id()] = true;
      cf_info->unique_frame_names.insert(frame_name);
      ready.push_back(n);
    }
  }

  while (!ready.empty()) {
    Node* curr_node = ready.front();
    int curr_id = curr_node->id();
    ready.pop_front();

    Node* parent = nullptr;
    if (IsEnter(curr_node)) {
      // Enter a child frame.
      TF_RETURN_IF_ERROR(
          GetNodeAttr(curr_node->attrs(), "frame_name", &frame_name));
      parent = curr_node;
    } else if (IsExit(curr_node)) {
      // Exit to the parent frame.
      parent = parent_nodes[curr_id];
      frame_name = cf_info->frame_names[parent->id()];
      parent = parent_nodes[parent->id()];
    } else {
      parent = parent_nodes[curr_id];
      frame_name = cf_info->frame_names[curr_id];
    }

    for (const Edge* out_edge : curr_node->out_edges()) {
      Node* out = out_edge->dst();
      if (IsSink(out)) continue;
      const int out_id = out->id();

      // Add to ready queue if not visited.
      bool is_visited = visited[out_id];
      if (!is_visited) {
        ready.push_back(out);
        visited[out_id] = true;

        // Process the node 'out'.
        cf_info->frame_names[out_id] = frame_name;
        parent_nodes[out_id] = parent;
        cf_info->unique_frame_names.insert(frame_name);
      }
    }
  }

  return Status::OK();
}