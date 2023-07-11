TfLiteStatus ComputeDepthMultiplier(TfLiteContext* context,
                                    const TfLiteTensor* input,
                                    const TfLiteTensor* filter,
                                    int16* depth_multiplier) {
  int num_filter_channels = SizeOfDimension(filter, 3);
  int num_input_channels = SizeOfDimension(input, 3);
  TF_LITE_ENSURE_EQ(context, num_filter_channels % num_input_channels, 0);

  *depth_multiplier = num_filter_channels / num_input_channels;
  return kTfLiteOk;
}