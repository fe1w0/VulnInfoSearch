TEST(CheckpointVersionTest, MinConsumer) {
  VersionDef versions;
  versions.set_producer(TF_CHECKPOINT_VERSION + 1);
  versions.set_min_consumer(TF_CHECKPOINT_VERSION + 1);
  VersionTest(
      versions,
      strings::StrCat("Checkpoint min consumer version ",
                      TF_CHECKPOINT_VERSION + 1, " above current version ",
                      TF_CHECKPOINT_VERSION, " for TensorFlow"));
}