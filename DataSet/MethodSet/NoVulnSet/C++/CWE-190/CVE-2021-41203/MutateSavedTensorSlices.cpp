void MutateSavedTensorSlices(
    const std::string& fname,
    const std::function<std::string(SavedTensorSlices)>& mutator) {
  table::Options options;
  options.compression = table::kNoCompression;

  // Read all entres from the table.
  std::vector<std::pair<std::string, std::string>> entries;
  {
    std::unique_ptr<RandomAccessFile> file;
    TF_CHECK_OK(Env::Default()->NewRandomAccessFile(fname, &file));
    uint64 file_size;
    TF_CHECK_OK(Env::Default()->GetFileSize(fname, &file_size));
    table::Table* t;
    TF_CHECK_OK(table::Table::Open(options, file.get(), file_size, &t));
    std::unique_ptr<table::Table> table(t);
    std::unique_ptr<table::Iterator> it(table->NewIterator());
    for (it->Seek(""); it->Valid(); it->Next()) {
      entries.emplace_back(it->key(), it->value());
    }
    TF_CHECK_OK(it->status());
  }

  // Rewrite the table, mutating each value.
  {
    std::unique_ptr<WritableFile> file;
    TF_CHECK_OK(Env::Default()->NewWritableFile(fname, &file));
    table::TableBuilder builder(options, file.get());
    for (const auto& entry : entries) {
      SavedTensorSlices sts;
      CHECK(sts.ParseFromString(entry.second));
      builder.Add(entry.first, mutator(std::move(sts)));
    }
    TF_CHECK_OK(builder.Finish());
    TF_CHECK_OK(file->Close());
  }
}