inline void BinaryBroadcastFiveFold(const ArithmeticParams& unswitched_params,
                                    const RuntimeShape& unswitched_input1_shape,
                                    const T* unswitched_input1_data,
                                    const RuntimeShape& unswitched_input2_shape,
                                    const T* unswitched_input2_data,
                                    const RuntimeShape& output_shape,
                                    T* output_data, ElementwiseF elementwise_f,
                                    ScalarBroadcastF scalar_broadcast_f) {
  ArithmeticParams switched_params = unswitched_params;
  switched_params.input1_offset = unswitched_params.input2_offset;
  switched_params.input1_multiplier = unswitched_params.input2_multiplier;
  switched_params.input1_shift = unswitched_params.input2_shift;
  switched_params.input2_offset = unswitched_params.input1_offset;
  switched_params.input2_multiplier = unswitched_params.input1_multiplier;
  switched_params.input2_shift = unswitched_params.input1_shift;

  const bool use_unswitched =
      unswitched_params.broadcast_category ==
      tflite::BroadcastableOpCategory::kFirstInputBroadcastsFast;

  const ArithmeticParams& params =
      use_unswitched ? unswitched_params : switched_params;
  const T* input1_data =
      use_unswitched ? unswitched_input1_data : unswitched_input2_data;
  const T* input2_data =
      use_unswitched ? unswitched_input2_data : unswitched_input1_data;

  // Fivefold nested loops. The second input resets its position for each
  // iteration of the second loop. The first input resets its position at the
  // beginning of the fourth loop. The innermost loop is an elementwise add of
  // sections of the arrays.
  T* output_data_ptr = output_data;
  const T* input1_data_ptr = input1_data;
  const T* input2_data_reset = input2_data;
  // In the fivefold pattern, y0, y2 and y4 are not broadcast, and so shared
  // between input shapes. y3 for input 1 is always broadcast, and so the
  // dimension there is 1, whereas optionally y1 might be broadcast for
  // input 2. Put another way, input1.shape.FlatSize = y0 * y1 * y2 * y4,
  // input2.shape.FlatSize = y0 * y2 * y3 * y4.
  int y0 = params.broadcast_shape[0];
  int y1 = params.broadcast_shape[1];
  int y2 = params.broadcast_shape[2];
  int y3 = params.broadcast_shape[3];
  int y4 = params.broadcast_shape[4];
  if (y4 > 1) {
    // General fivefold pattern, with y4 > 1 so there is a non-broadcast inner
    // dimension.
    for (int i0 = 0; i0 < y0; ++i0) {
      const T* input2_data_ptr = nullptr;
      for (int i1 = 0; i1 < y1; ++i1) {
        input2_data_ptr = input2_data_reset;
        for (int i2 = 0; i2 < y2; ++i2) {
          for (int i3 = 0; i3 < y3; ++i3) {
            elementwise_f(y4, params, input1_data_ptr, input2_data_ptr,
                          output_data_ptr);
            input2_data_ptr += y4;
            output_data_ptr += y4;
          }
          // We have broadcast y4 of input1 data y3 times, and now move on.
          input1_data_ptr += y4;
        }
      }
      // We have broadcast y2*y3*y4 of input2 data y1 times, and now move on.
      input2_data_reset = input2_data_ptr;
    }
  } else {
    // Special case of y4 == 1, in which the innermost loop is a single
    // element and can be combined with the next (y3) as an inner broadcast.
    //
    // Note that this handles the case of pure scalar broadcast when
    // y0 == y1 == y2 == 1. With low overhead it handles cases such as scalar
    // broadcast with batch (as y2 > 1).
    //
    // NOTE The process is the same as the above general case except
    // simplified for y4 == 1 and the loop over y3 is contained within the
    // AddScalarBroadcast function.
    for (int i0 = 0; i0 < y0; ++i0) {
      const T* input2_data_ptr = nullptr;
      for (int i1 = 0; i1 < y1; ++i1) {
        input2_data_ptr = input2_data_reset;
        for (int i2 = 0; i2 < y2; ++i2) {
          scalar_broadcast_f(y3, params, *input1_data_ptr, input2_data_ptr,
                             output_data_ptr);
          input2_data_ptr += y3;
          output_data_ptr += y3;
          input1_data_ptr += 1;
        }
      }
      input2_data_reset = input2_data_ptr;
    }
  }
}