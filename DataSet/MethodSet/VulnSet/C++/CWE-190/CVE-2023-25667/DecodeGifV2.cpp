  void DecodeGifV2(OpKernelContext* context, StringPiece input) {
    // GIF has 3 channels.
    OP_REQUIRES(context, channels_ == 0 || channels_ == 3,
                errors::InvalidArgument("channels must be 0 or 3 for GIF, got ",
                                        channels_));

    if (op_type_ == "DecodeBmp") {
      // TODO(b/171060723): Only DecodeBmp as op_type_ is not acceptable here
      // because currently `decode_(jpeg|png|gif)` ops can decode any one of
      // jpeg, png or gif but not bmp. Similarly, `decode_bmp` cannot decode
      // anything but bmp formats. This behavior needs to be revisited. For more
      // details, please refer to the bug.
      OP_REQUIRES(context, false,
                  errors::InvalidArgument(
                      "Trying to decode GIF format using DecodeBmp op. Use "
                      "`decode_gif` or `decode_image` instead."));
    } else if (op_type_ == "DecodeAndCropJpeg") {
      OP_REQUIRES(context, false,
                  errors::InvalidArgument(
                      "DecodeAndCropJpeg operation can run on JPEG only, but "
                      "detected GIF."));
    }

    // Decode GIF, allocating tensor if dtype is uint8, otherwise defer tensor
    // allocation til after dtype conversion is done. `gif`::Decode` supports
    // uint8 only.
    Tensor* output = nullptr;
    int buffer_size = 0;
    string error_string;
    uint8* buffer = gif::Decode(
        input.data(), input.size(),
        [&](int num_frames, int width, int height, int channels) -> uint8* {
          buffer_size = num_frames * height * width * channels;

          Status status;
          // By the existing API, we support decoding GIF with `decode_jpeg` or
          // with `decode_png` if the GIF is a single-frame GIF (non-animated).
          // We need to make sure to return 3-D shapes when using in this case.
          if (op_type_ == "DecodePng" || op_type_ == "DecodeJpeg") {
            if (num_frames == 1) {
              status = context->allocate_output(
                  0, TensorShape({height, width, channels}), &output);
            } else {
              status = errors::InvalidArgument(
                  "Got ", num_frames, " frames, but animated gifs ",
                  "can only be decoded by tf.io.decode_gif or ",
                  "tf.io.decode_image");
            }
          } else if (op_type_ == "DecodeGif" ||
                     (op_type_ == "DecodeImage" && expand_animations_)) {
            status = context->allocate_output(
                0, TensorShape({num_frames, height, width, channels}), &output);
          } else if (op_type_ == "DecodeImage" && !expand_animations_) {
            status = context->allocate_output(
                0, TensorShape({height, width, channels}), &output);
          } else {
            status = errors::InvalidArgument("Bad op type ", op_type_);
          }
          if (!status.ok()) {
            VLOG(1) << status;
            context->SetStatus(status);
            return nullptr;
          }

          if (data_type_ == DataType::DT_UINT8) {
            return output->flat<uint8>().data();
          } else {
            return new uint8[buffer_size];
          }
        },
        &error_string, expand_animations_);

    OP_REQUIRES(context, buffer,
                errors::InvalidArgument("Invalid GIF data (size ", input.size(),
                                        "), ", error_string));

    // For when desired data type is uint8, the output buffer is already
    // allocated during the `gif::Decode` call above; return.
    if (data_type_ == DataType::DT_UINT8) {
      return;
    }
    // Make sure we don't forget to deallocate `buffer`.
    std::unique_ptr<uint8[]> buffer_unique_ptr(buffer);

    // Convert the raw uint8 buffer to desired dtype.
    // Use eigen threadpooling to speed up the copy operation.
    TTypes<uint8>::UnalignedConstFlat buffer_view(buffer, buffer_size);
    const auto& device = context->eigen_device<Eigen::ThreadPoolDevice>();
    if (data_type_ == DataType::DT_UINT16) {
      uint16 scale = floor((std::numeric_limits<uint16>::max() + 1) /
                           (std::numeric_limits<uint8>::max() + 1));
      // Fill output tensor with desired dtype.
      output->flat<uint16>().device(device) =
          buffer_view.cast<uint16>() * scale;
    } else if (data_type_ == DataType::DT_FLOAT) {
      float scale = 1. / std::numeric_limits<uint8>::max();
      // Fill output tensor with desired dtype.
      output->flat<float>().device(device) = buffer_view.cast<float>() * scale;
    }
  }