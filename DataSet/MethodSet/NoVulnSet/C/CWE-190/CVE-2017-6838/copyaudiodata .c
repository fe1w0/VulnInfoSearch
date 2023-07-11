bool copyaudiodata (AFfilehandle infile, AFfilehandle outfile, int trackid)
{
	int frameSize = afGetVirtualFrameSize(infile, trackid, 1);

	int kBufferFrameCount = 65536;
	int bufferSize;
	while (multiplyCheckOverflow(kBufferFrameCount, frameSize, &bufferSize))
		kBufferFrameCount /= 2;
	void *buffer = malloc(bufferSize);

	AFframecount totalFrames = afGetFrameCount(infile, AF_DEFAULT_TRACK);
	AFframecount totalFramesWritten = 0;

	bool success = true;

	while (totalFramesWritten < totalFrames)
	{
		AFframecount framesToRead = totalFrames - totalFramesWritten;
		if (framesToRead > kBufferFrameCount)
			framesToRead = kBufferFrameCount;

		AFframecount framesRead = afReadFrames(infile, trackid, buffer,
			framesToRead);

		if (framesRead < framesToRead)
		{
			fprintf(stderr, "Bad read of audio track data.\n");
			success = false;
			break;
		}

		AFframecount framesWritten = afWriteFrames(outfile, trackid, buffer,
			framesRead);

		if (framesWritten < framesRead)
		{
			fprintf(stderr, "Bad write of audio track data.\n");
			success = false;
			break;
		}

		totalFramesWritten += framesWritten;
	}

	free(buffer);

	return success;
}