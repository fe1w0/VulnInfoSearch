int main(int argc, char * argv[])
{
    gr_face * face = 0;
    try
	{
		if (argc != 2)	throw std::length_error("not enough arguments: need a backing font");

		dummyFace = face_handle(argv[1]);
		testFeatTable<FeatTableTestA>(testDataA, "A\n");
		testFeatTable<FeatTableTestB>(testDataB, "B\n");
		testFeatTable<FeatTableTestB>(testDataBunsorted, "Bu\n");
		testFeatTable<FeatTableTestC>(testDataCunsorted, "C\n");
		testFeatTable<FeatTableTestD>(testDataDunsorted, "D\n");
		testFeatTable<FeatTableTestE>(testDataE, "E\n");

		// test a bad settings offset stradling the end of the table
		FeatureMap testFeatureMap;
		dummyFace.replace_table(TtfUtil::Tag::Feat, &testBadOffset, sizeof testBadOffset);
		face = gr_make_face_with_ops(&dummyFace, &face_handle::ops, gr_face_dumbRendering);
		bool readStatus = testFeatureMap.readFeats(*face);
		testAssert("fail gracefully on bad table", !readStatus);
	}
	catch (std::exception & e)
	{
		fprintf(stderr, "%s: %s\n", argv[0], e.what());
		gr_face_destroy(face);
		return 1;
	}

    gr_face_destroy(face);
    return 0;
}