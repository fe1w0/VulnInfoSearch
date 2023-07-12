TEST(CheckpointVersionTest, BadConsumer) {
  VersionDef versions;
  versions.set_producer(TF_CHECKPOINT_VERSION + 1);
  versions.add_bad_consumers(TF_CHECKPOINT_VERSION);
  VersionTest(
      versions,
      strings::StrCat(
          "Checkpoint disallows consumer version ", TF_CHECKPOINT_VERSION,
          ".  Please upgrade TensorFlow: this version is likely buggy."));
}