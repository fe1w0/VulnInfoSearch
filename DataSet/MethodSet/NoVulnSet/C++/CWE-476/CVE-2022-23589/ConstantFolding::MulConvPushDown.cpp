bool ConstantFolding::MulConvPushDown(GraphDef* optimized_graph, NodeDef* node,
                                      const GraphProperties& properties) {
  // Push down multiplication on ConvND.
  //                       *                  ConvND
  //                     /   \                /    \
  //                 ConvND  C2    -- >      X      *
  //                  / \                          / \
  //                 X  C1                       C1  C2
  //
  // where C1 and C2 are constants and X is non-constant.
  //
  // TODO(rmlarsen): Use PrepareConstantPushDown() to simplify this code.

  if (!IsAnyMul(*node) || NumNonControlInputs(*node) != 2) return false;

  NodeDef* mul_left_child = node_map_->GetNode(node->input(0));
  NodeDef* mul_right_child = node_map_->GetNode(node->input(1));
  if (mul_left_child == nullptr || mul_right_child == nullptr) {
    return false;
  }
  // One child must be constant, and the second must be Conv op.
  const bool left_child_is_constant = IsReallyConstant(*mul_left_child);
  const bool right_child_is_constant = IsReallyConstant(*mul_right_child);
  if (!left_child_is_constant && !right_child_is_constant) {
    return false;
  }
  NodeDef* conv_node =
      left_child_is_constant ? mul_right_child : mul_left_child;
  if (!IsConv2D(*conv_node) && !IsConv3D(*conv_node)) {
    return false;
  }
  if (node->device() != mul_left_child->device() ||
      node->device() != mul_right_child->device()) {
    return false;
  }

  // Make sure that it is safe to change the value of the convolution
  // output.
  if (conv_node->input_size() < 2 ||
      NumNonControlOutputs(*conv_node, *node_map_) > 1 ||
      nodes_to_preserve_.find(conv_node->name()) != nodes_to_preserve_.end()) {
    return false;
  }

  // Identify the nodes to swap.
  NodeDef* conv_left_child = node_map_->GetNode(conv_node->input(0));
  NodeDef* conv_right_child = node_map_->GetNode(conv_node->input(1));
  const bool conv_left_is_constant = IsReallyConstant(*conv_left_child);
  const bool conv_right_is_constant = IsReallyConstant(*conv_right_child);
  if (!conv_left_is_constant && !conv_right_is_constant) {
    // At least one of the convolution inputs should be constant.
    return false;
  }
  if (conv_left_is_constant && conv_right_is_constant) {
    // Leverage regular constant folding to handle this.
    return false;
  }
  const auto& mul_props = properties.GetOutputProperties(node->name());
  const auto& conv_props = properties.GetOutputProperties(conv_node->name());
  if (mul_props.empty() || conv_props.empty()) {
    return false;
  }
  const auto& mul_shape = mul_props[0].shape();
  const auto& conv_shape = conv_props[0].shape();
  if (!ShapesSymbolicallyEqual(mul_shape, conv_shape)) {
    return false;
  }

  const auto& input_props = properties.GetInputProperties(conv_node->name());
  if (input_props.size() < 2) {
    return false;
  }
  const auto& filter_shape = input_props[1].shape();

  NodeDef* const_node =
      left_child_is_constant ? mul_left_child : mul_right_child;
  const auto& const_props = properties.GetOutputProperties(const_node->name());
  if (const_props.empty()) {
    return false;
  }
  const auto& const_shape = const_props[0].shape();
  if (!IsValidConstShapeForMulConvPushDown(
          conv_node->attr().at("data_format").s(), filter_shape, const_shape)) {
    return false;
  }

  string mul_new_name = AddPrefixToNodeName("merged_input", conv_node->name());
  if (node_map_->NodeExists(mul_new_name)) {
    return false;
  }
  // Make sure we don't introduce loops in the graph by removing control
  // dependencies from the conv2d node to c2.
  string conv_const_input =
      conv_left_is_constant ? conv_node->input(0) : conv_node->input(1);
  if (MaybeRemoveControlInput(conv_node->name(), const_node, optimized_graph,
                              node_map_.get())) {
    // Add a control dep from c1 to c2 to ensure c2 is in the right frame
    MaybeAddControlInput(conv_const_input, const_node, optimized_graph,
                         node_map_.get());
  }

  conv_node->set_name(node->name());
  node->set_name(mul_new_name);
  if (conv_left_is_constant) {
    node_map_->UpdateInput(conv_node->name(), node->input(0), mul_new_name);
    conv_node->set_input(0, mul_new_name);
  } else {
    node_map_->UpdateInput(conv_node->name(), node->input(1), mul_new_name);
    conv_node->set_input(1, mul_new_name);
  }
  NodeDef* conv_const_node =
      conv_left_is_constant ? conv_left_child : conv_right_child;
  if (left_child_is_constant) {
    node->set_input(1, conv_const_node->name());
  } else {
    node->set_input(0, conv_const_node->name());
  }
  node_map_->AddNode(mul_new_name, node);

  return true;
}