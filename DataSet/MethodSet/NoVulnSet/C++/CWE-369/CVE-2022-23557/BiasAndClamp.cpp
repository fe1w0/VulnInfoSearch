inline void BiasAndClamp(float clamp_min, float clamp_max, int bias_size,
                         const float* bias_data, int array_size,
                         float* array_data) {
  if (bias_size == 0) return;
  // Note: see b/132215220: in May 2019 we thought it would be OK to replace
  // this with the Eigen one-liner:
  //   return (array.colwise() + bias).cwiseMin(clamp_max).cwiseMin(clamp_max).
  // This turned out to severely regress performance: +4ms (i.e. 8%) on
  // MobileNet v2 / 1.0 / 224. So we keep custom NEON code for now.
  TFLITE_DCHECK_EQ((array_size % bias_size), 0);
#ifdef USE_NEON
  float* array_ptr = array_data;
  float* array_end_ptr = array_ptr + array_size;
  const auto clamp_min_vec = vdupq_n_f32(clamp_min);
  const auto clamp_max_vec = vdupq_n_f32(clamp_max);
  for (; array_ptr != array_end_ptr; array_ptr += bias_size) {
    int i = 0;
    for (; i <= bias_size - 16; i += 16) {
      auto b0 = vld1q_f32(bias_data + i);
      auto b1 = vld1q_f32(bias_data + i + 4);
      auto b2 = vld1q_f32(bias_data + i + 8);
      auto b3 = vld1q_f32(bias_data + i + 12);
      auto a0 = vld1q_f32(array_ptr + i);
      auto a1 = vld1q_f32(array_ptr + i + 4);
      auto a2 = vld1q_f32(array_ptr + i + 8);
      auto a3 = vld1q_f32(array_ptr + i + 12);
      auto x0 = vaddq_f32(a0, b0);
      auto x1 = vaddq_f32(a1, b1);
      auto x2 = vaddq_f32(a2, b2);
      auto x3 = vaddq_f32(a3, b3);
      x0 = vmaxq_f32(clamp_min_vec, x0);
      x1 = vmaxq_f32(clamp_min_vec, x1);
      x2 = vmaxq_f32(clamp_min_vec, x2);
      x3 = vmaxq_f32(clamp_min_vec, x3);
      x0 = vminq_f32(clamp_max_vec, x0);
      x1 = vminq_f32(clamp_max_vec, x1);
      x2 = vminq_f32(clamp_max_vec, x2);
      x3 = vminq_f32(clamp_max_vec, x3);
      vst1q_f32(array_ptr + i, x0);
      vst1q_f32(array_ptr + i + 4, x1);
      vst1q_f32(array_ptr + i + 8, x2);
      vst1q_f32(array_ptr + i + 12, x3);
    }
    for (; i <= bias_size - 4; i += 4) {
      auto b = vld1q_f32(bias_data + i);
      auto a = vld1q_f32(array_ptr + i);
      auto x = vaddq_f32(a, b);
      x = vmaxq_f32(clamp_min_vec, x);
      x = vminq_f32(clamp_max_vec, x);
      vst1q_f32(array_ptr + i, x);
    }
    for (; i < bias_size; i++) {
      array_ptr[i] = ActivationFunctionWithMinMax(array_ptr[i] + bias_data[i],
                                                  clamp_min, clamp_max);
    }
  }
#else  // not NEON
  for (int array_offset = 0; array_offset < array_size;
       array_offset += bias_size) {
    for (int i = 0; i < bias_size; i++) {
      array_data[array_offset + i] = ActivationFunctionWithMinMax(
          array_data[array_offset + i] + bias_data[i], clamp_min, clamp_max);
    }
  }
#endif
}