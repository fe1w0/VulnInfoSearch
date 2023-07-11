void PngImg::InitStorage_() {
    rowPtrs_.resize(info_.height, nullptr);
    // Extend height and rowbytes from uint32_t to size_t to avoid multiplication overflow when size_t is larger
    size_t h = info_.height;
    size_t rb = info_.rowbytes;
    // We need to make sure that info_.height * info_.rowbytes will not overflow size_t
    // Unfotunately, there's no simple and portable way to do this in C++
    // For integer division of positive numbers a * b > c <==> a > c / b holds
    if (h > std::numeric_limits<size_t>::max() / rb) {
        // TODO Propagate this exception to JS, and test it
        throw std::runtime_error("Image is too large to allocate single buffer");
    }
    data_ = new png_byte[h * rb];

    for(size_t i = 0; i < info_.height; ++i) {
        rowPtrs_[i] = data_ + i * rb;
    }
}