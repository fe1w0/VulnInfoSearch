bool IsIdentityConsumingSwitch(const MutableGraphView& graph,
                               const NodeDef& node) {
  if ((IsIdentity(node) || IsIdentityNSingleInput(node)) &&
      node.input_size() > 0) {
    TensorId tensor_id = ParseTensorName(node.input(0));
    if (IsTensorIdControlling(tensor_id)) {
      return false;
    }

    NodeDef* input_node = graph.GetNode(tensor_id.node());
    if (input_node == nullptr) {
      return false;
    }
    return IsSwitch(*input_node);
  }
  return false;
}