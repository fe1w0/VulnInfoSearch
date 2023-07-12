TEST(CheckpointVersionTest, MinProducer) {
  VersionDef versions;
  versions.set_producer(TF_CHECKPOINT_VERSION_MIN_PRODUCER - 1);
  VersionTest(versions, strings::StrCat("Checkpoint producer version ",
                                        TF_CHECKPOINT_VERSION_MIN_PRODUCER - 1,
                                        " below min producer ",
                                        TF_CHECKPOINT_VERSION_MIN_PRODUCER,
                                        " supported by TensorFlow"));
}