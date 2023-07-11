void PngImg::InitStorage_() {
    rowPtrs_.resize(info_.height, nullptr);
    data_ = new png_byte[info_.height * info_.rowbytes];

    for(size_t i = 0; i < info_.height; ++i) {
        rowPtrs_[i] = data_ + i * info_.rowbytes;
    }
}