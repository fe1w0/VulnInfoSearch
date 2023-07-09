static bool bin_pe_init_metadata_hdr(RBinPEObj* pe) {
	PE_(image_metadata_header) * metadata = R_NEW0 (PE_(image_metadata_header));
	if (!metadata) {
		return false;
	}
	PE_DWord metadata_directory = pe->clr_hdr? PE_(va2pa) (pe, pe->clr_hdr->MetaDataDirectoryAddress): 0;
	if (!metadata_directory) {
		free (metadata);
		return false;
	}
	int rr = r_buf_fread_at (pe->b, metadata_directory,
		(ut8*) metadata, pe->big_endian? "1I2S": "1i2s", 1);
	if (rr < 1) {
		goto fail;
	}

	rr = r_buf_fread_at (pe->b, metadata_directory + 8,
		(ut8*) (&metadata->Reserved), pe->big_endian? "1I": "1i", 1);
	if (rr < 1) {
		goto fail;
	}

	rr = r_buf_fread_at (pe->b, metadata_directory + 12,
		(ut8*) (&metadata->VersionStringLength), pe->big_endian? "1I": "1i", 1);
	if (rr < 1) {
		goto fail;
	}

	eprintf ("Metadata Signature: 0x%"PFMT64x" 0x%"PFMT64x" %d\n",
		(ut64)metadata_directory, (ut64)metadata->Signature, (int)metadata->VersionStringLength);

	// read the version string
	int len = metadata->VersionStringLength; // XXX: dont trust this length
	if (len > 0) {
		metadata->VersionString = calloc (1, len + 1);
		if (!metadata->VersionString) {
			goto fail;
		}

		rr = r_buf_read_at (pe->b, metadata_directory + 16, (ut8*)(metadata->VersionString), len);
		if (rr != len) {
			eprintf ("Warning: read (metadata header) - cannot parse version string\n");
			free (metadata->VersionString);
			free (metadata);
			return 0;
		}
		eprintf (".NET Version: %s\n", metadata->VersionString);
	}

	// read the header after the string
	rr = r_buf_fread_at (pe->b, metadata_directory + 16 + metadata->VersionStringLength,
		(ut8*) (&metadata->Flags), pe->big_endian? "2S": "2s", 1);

	if (rr < 1) {
		goto fail;
	}

	eprintf ("Number of Metadata Streams: %d\n", metadata->NumberOfStreams);
	pe->metadata_header = metadata;


	// read metadata streams
	int stream_addr = metadata_directory + 20 + metadata->VersionStringLength;
	PE_(image_metadata_stream) * stream;
	PE_(image_metadata_stream) **streams = calloc (sizeof (PE_(image_metadata_stream)*), metadata->NumberOfStreams);
	if (!streams) {
		goto fail;
	}
	int count;
	for (count = 0; count < metadata->NumberOfStreams; count++) {
		stream = R_NEW0 (PE_(image_metadata_stream));
		if (!stream) {
			free (streams);
			goto fail;
		}
		if (r_buf_size (pe->b) < (stream_addr + 8 + MAX_METADATA_STRING_LENGTH)) {
			eprintf ("Truncated\n");
			free (stream);
			free (streams);
			goto fail;
		}
		if (r_buf_fread_at (pe->b, stream_addr, (ut8*) stream, pe->big_endian? "2I": "2i", 1) < 1) {
			free (stream);
			free (streams);
			goto fail;
		}
		eprintf ("DirectoryAddress: %x Size: %x\n", stream->Offset, stream->Size);
		char* stream_name = calloc (1, MAX_METADATA_STRING_LENGTH + 1);

		if (!stream_name) {
			free (stream);
			free (streams);
			goto fail;
		}

		int c = bin_pe_read_metadata_string (stream_name, pe->b, stream_addr + 8);
		if (c == 0) {
			free (stream_name);
			free (stream);
			free (streams);
			goto fail;
		}
		eprintf ("Stream name: %s %d\n", stream_name, c);
		stream->Name = stream_name;
		streams[count] = stream;
		stream_addr += 8 + c;
	}
	pe->streams = streams;
	return true;
fail:
	eprintf ("Warning: read (metadata header)\n");
	free (metadata);
	return false;
}