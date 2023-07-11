Status OpLevelCostEstimator::PredictCropAndResize(const OpContext& op_context,
                                                  NodeCosts* node_costs) const {
  bool found_unknown_shapes = false;

  const auto method = op_context.op_info.attr().find("method");
  bool use_bilinear_interp;
  if (method == op_context.op_info.attr().end() ||
      method->second.s() == "bilinear") {
    use_bilinear_interp = true;
  } else if (method->second.s() == "nearest") {
    use_bilinear_interp = false;
  } else {
    LOG(WARNING) << "method attr in CropAndResize invalid; expected bilinear "
                    "or nearest.";
    return PredictCostOfAnUnknownOp(op_context, node_costs);
  }

  const int64_t num_boxes = op_context.op_info.inputs(1).shape().dim(0).size();
  const auto crop_shape = MaybeGetMinimumShape(
      op_context.op_info.outputs(0).shape(), 4, &found_unknown_shapes);
  const int64_t crop_height = crop_shape.dim(1).size();
  const int64_t crop_width = crop_shape.dim(2).size();
  const int64_t output_elements = CalculateTensorElementCount(
      op_context.op_info.outputs(0), &found_unknown_shapes);

#define EIGEN_COST(X) Eigen::internal::functor_traits<Eigen::internal::X>::Cost
  const auto sub_cost = EIGEN_COST(scalar_difference_op<float>);
  const auto add_cost = EIGEN_COST(scalar_sum_op<float>);
  const auto mul_cost = EIGEN_COST(scalar_product_op<float>);
  auto div_cost = EIGEN_COST(scalar_div_cost<float>);
  const auto floor_cost = EIGEN_COST(scalar_floor_op<float>);
  const auto ceil_cost = EIGEN_COST(scalar_ceil_op<float>);
  auto round_cost = EIGEN_COST(scalar_round_op<float>);
  const auto cast_to_float_cost = Eigen::internal::functor_traits<
      Eigen::internal::scalar_cast_op<int64_t, float>>::Cost;
#undef EIGEN_COST

  // Computing ops following
  // tensorflow/core/kernels/image/crop_and_resize_op.cc at 08/25/2020. Op
  // calculation differs from rough estimate in implementation, as it separates
  // out cost per box from cost per pixel and cost per element.

  // Ops for variables height_scale and width_scale.
  int64_t ops = (sub_cost * 6 + mul_cost * 2 + div_cost * 2) * num_boxes;
  // Ops for variable in_y.
  ops += (mul_cost * 2 + sub_cost + add_cost) * crop_height * num_boxes;
  // Ops for variable in_x (same computation across both branches).
  ops += (mul_cost * 2 + sub_cost + add_cost) * crop_height * crop_width *
         num_boxes;
  // Specify op_cost based on the method.
  if (use_bilinear_interp) {
    // Ops for variables top_y_index, bottom_y_index, y_lerp.
    ops += (floor_cost + ceil_cost + sub_cost) * crop_height * num_boxes;
    // Ops for variables left_x, right_x, x_lerp;
    ops += (floor_cost + ceil_cost + sub_cost) * crop_height * crop_width *
           num_boxes;
    // Ops for innermost loop across depth.
    ops +=
        (cast_to_float_cost * 4 + add_cost * 3 + sub_cost * 3 + mul_cost * 3) *
        output_elements;
  } else /* method == "nearest" */ {
    // Ops for variables closest_x_index and closest_y_index.
    ops += round_cost * 2 * crop_height * crop_width * num_boxes;
    // Ops for innermost loop across depth.
    ops += cast_to_float_cost * output_elements;
  }
  return PredictDefaultNodeCosts(ops, op_context, &found_unknown_shapes,
                                 node_costs);
}