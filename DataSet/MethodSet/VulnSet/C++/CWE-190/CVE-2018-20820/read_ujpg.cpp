bool read_ujpg( void )
{
    using namespace IOUtil;
    using namespace Sirikata;
//    colldata.start_decoder_worker_thread(std::bind(&simple_decoder, &colldata, str_in));
    unsigned char ujpg_mrk[ 64 ];
    // this is where we will enable seccomp, before reading user data
    write_byte_bill(Billing::HEADER, true, 24); // for the fixed header

    str_out->call_size_callback(max_file_size);
    uint32_t compressed_header_size = 0;
    if (ReadFull(str_in, ujpg_mrk, 4) != 4) {
        custom_exit(ExitCode::SHORT_READ);
    }
    write_byte_bill(Billing::HEADER, true, 4);

    compressed_header_size = LEtoUint32(ujpg_mrk);
    if (compressed_header_size > 128 * 1024 * 1024 || max_file_size > 128 * 1024 * 1024) {
        always_assert(false && "Only support images < 128 megs");
        return false; // bool too big
    }
    bool pending_header_reads = false;
    if (header_reader == NULL) {
        std::vector<uint8_t, JpegAllocator<uint8_t> > compressed_header_buffer(compressed_header_size);
        IOUtil::ReadFull(str_in, compressed_header_buffer.data(), compressed_header_buffer.size());
        header_reader = new MemReadWriter((JpegAllocator<uint8_t>()));
        {
            if (ujgversion == 1) {
                JpegAllocator<uint8_t> no_free_allocator;
#if !defined(USE_STANDARD_MEMORY_ALLOCATORS) && !defined(_WIN32) && !defined(EMSCRIPTEN)
                no_free_allocator.setup_memory_subsystem(32 * 1024 * 1024,
                                                         16,
                                                         &mem_init_nop,
                                                         &MemMgrAllocatorMalloc,
                                                         &mem_nop,
                                                         &mem_realloc_nop,
                                                         &MemMgrAllocatorMsize);
#endif
                std::pair<std::vector<uint8_t,
                                      Sirikata::JpegAllocator<uint8_t> >,
                          JpegError> uncompressed_header_buffer(
                              ZlibDecoderDecompressionReader::Decompress(compressed_header_buffer.data(),
                                                                         compressed_header_buffer.size(),
                                                                         no_free_allocator,
                                                                         max_file_size + 2048));
                if (uncompressed_header_buffer.second) {
                    always_assert(false && "Data not properly zlib coded");
                    return false;
                }
                zlib_hdrs = compressed_header_buffer.size();
                header_reader->SwapIn(uncompressed_header_buffer.first, 0);
            } else {
                std::pair<std::vector<uint8_t,
                                      Sirikata::JpegAllocator<uint8_t> >,
                          JpegError> uncompressed_header_buffer(
                              Sirikata::BrotliCodec::Decompress(compressed_header_buffer.data(),
                                                                compressed_header_buffer.size(),
                                                                JpegAllocator<uint8_t>(),
                                                                max_file_size * 2 + 128 * 1024 * 1024));
                if (uncompressed_header_buffer.second) {
                    always_assert(false && "Data not properly zlib coded");
                    return false;
                }
                zlib_hdrs = compressed_header_buffer.size();
                header_reader->SwapIn(uncompressed_header_buffer.first, 0);            
            }
        }
        write_byte_bill(Billing::HEADER,
                        true,
                        compressed_header_buffer.size());
    } else {
        always_assert(compressed_header_size == 0 && "Special concatenation requires 0 size header");
    }
    grbs = sizeof(EOI);
    grbgdata = EOI; // if we don't have any garbage, assume FFD9 EOI
    // read header from file
    ReadFull(header_reader, ujpg_mrk, 3 ) ;
    // check marker
    if ( memcmp( ujpg_mrk, "HDR", 3 ) == 0 ) {
        // read size of header, alloc memory
        ReadFull(header_reader, ujpg_mrk, 4 );
        hdrs = LEtoUint32(ujpg_mrk);
        hdrdata = (unsigned char*) aligned_alloc(hdrs);
        memset(hdrdata, 0, hdrs);
        if ( hdrdata == NULL ) {
            fprintf( stderr, MEM_ERRMSG );
            errorlevel.store(2);
            return false;
        }
        // read hdrdata
        ReadFull(header_reader, hdrdata, hdrs );
    }
    else {
        fprintf( stderr, "HDR marker not found" );
        errorlevel.store(2);
        return false;
    }
    bool memory_optimized_image = (filetype != UJG) && !g_allow_progressive;
    // parse header for image-info
    if ( !setup_imginfo_jpg(memory_optimized_image) )
        return false;

    // beginning here: recovery information (needed for exact JPEG recovery)

    // read padbit information from file
    ReadFull(header_reader, ujpg_mrk, 3 );
    // check marker
    if ( memcmp( ujpg_mrk, "P0D", 3 ) == 0 ) {
        // This is a more nuanced pad byte that can have different values per bit
        header_reader->Read( reinterpret_cast<unsigned char*>(&padbit), 1 );
    }
    else if ( memcmp( ujpg_mrk, "PAD", 3 ) == 0 ) {
        // this is a single pad bit that is implied to have all the same values
        header_reader->Read( reinterpret_cast<unsigned char*>(&padbit), 1 );
        if (!(padbit == 0 || padbit == 1 ||padbit == -1)) {
            while (write(2,
                        "Legacy Padbit must be 0, 1 or -1\n",
                         strlen("Legacy Padbit must be 0, 1 or -1\n")) < 0
                   && errno == EINTR) {
            }
            custom_exit(ExitCode::STREAM_INCONSISTENT);
        }
        if (padbit == 1) {
            padbit = 0x7f; // all 6 bits set
        }
    }
    else {
        fprintf( stderr, "PAD marker not found" );
        errorlevel.store(2);
        return false;
    }
    std::vector<ThreadHandoff> thread_handoff;
    // read further recovery information if any
    while ( ReadFull(header_reader, ujpg_mrk, 3 ) == 3 ) {
        // check marker
        if ( memcmp( ujpg_mrk, "CRS", 3 ) == 0 ) {
            rst_cnt_set = true;
            ReadFull(header_reader, ujpg_mrk, 4);
            rst_cnt.resize(LEtoUint32(ujpg_mrk));
            for (size_t i = 0; i < rst_cnt.size(); ++i) {
                ReadFull(header_reader, ujpg_mrk, 4);
                rst_cnt.at(i) = LEtoUint32(ujpg_mrk);
            }
        } else if ( memcmp( ujpg_mrk, "HHX", 2 ) == 0 ) { // only look at first two bytes
            size_t to_alloc = ThreadHandoff::get_remaining_data_size_from_two_bytes(ujpg_mrk + 1) + 2;
            if(to_alloc) {
                std::vector<unsigned char> data(to_alloc);
                data[0] = ujpg_mrk[1];
                data[1] = ujpg_mrk[2];
                ReadFull(header_reader, &data[2], to_alloc - 2);
                thread_handoff = ThreadHandoff::deserialize(&data[0], to_alloc);
            }
        } else if ( memcmp( ujpg_mrk, "FRS", 3 ) == 0 ) {
            // read number of false set RST markers per scan from file
            ReadFull(header_reader, ujpg_mrk, 4);
            scnc = LEtoUint32(ujpg_mrk);
            
            rst_err.insert(rst_err.end(), scnc - rst_err.size(), 0);
            // read data
            ReadFull(header_reader, rst_err.data(), scnc );
        }
        else if ( memcmp( ujpg_mrk, "GRB", 3 ) == 0 ) {
            // read garbage (data after end of JPG) from file
            ReadFull(header_reader, ujpg_mrk, 4);
            grbs = LEtoUint32(ujpg_mrk);
            grbgdata = aligned_alloc(grbs);
            memset(grbgdata, 0, sizeof(grbs));
            if ( grbgdata == NULL ) {
                fprintf( stderr, MEM_ERRMSG );
                errorlevel.store(2);
                return false;
            }
            // read garbage data
            ReadFull(header_reader, grbgdata, grbs );
        }
        else if ( memcmp( ujpg_mrk, "PGR", 3 ) == 0 || memcmp( ujpg_mrk, "PGE", 3 ) == 0 ) {
            // read prefix garbage (data before beginning of JPG) from file
            if (ujpg_mrk[2] == 'E') {
                // embedded jpeg: full header required
                embedded_jpeg = true;
            }
            ReadFull(header_reader, ujpg_mrk, 4);
            prefix_grbs = LEtoUint32(ujpg_mrk);
            prefix_grbgdata = aligned_alloc(prefix_grbs);
            memset(prefix_grbgdata, 0, sizeof(prefix_grbs));
            if ( prefix_grbgdata == NULL ) {
                fprintf( stderr, MEM_ERRMSG );
                errorlevel.store(2);
                return false;
            }
            // read garbage data
            ReadFull(header_reader, prefix_grbgdata, prefix_grbs );
        }
        else if ( memcmp( ujpg_mrk, "SIZ", 3 ) == 0 ) {
            // full size of the original file
            ReadFull(header_reader, ujpg_mrk, 4);
            max_file_size = LEtoUint32(ujpg_mrk);
        }
        else if ( memcmp( ujpg_mrk, "EEE", 3) == 0) {
            ReadFull(header_reader, ujpg_mrk, 28);
            max_cmp = LEtoUint32(ujpg_mrk);
            max_bpos = LEtoUint32(ujpg_mrk + 4);
            max_sah = LEtoUint32(ujpg_mrk + 8);
            max_dpos[0] = LEtoUint32(ujpg_mrk + 12);
            max_dpos[1] = LEtoUint32(ujpg_mrk + 16);
            max_dpos[2] = LEtoUint32(ujpg_mrk + 20);
            max_dpos[3] = LEtoUint32(ujpg_mrk + 24);
            early_eof_encountered = true;
            colldata.set_truncation_bounds(max_cmp, max_bpos, max_dpos, max_sah);
        }
        else {
            if (memcmp(ujpg_mrk, "CNT", 3) == 0 ) {
                pending_header_reads = true;
                break;
            } else if (memcmp(ujpg_mrk, "CMP", 3) == 0 ) {
                break;
            } else {
                fprintf( stderr, "unknown data found" );
                errorlevel.store(2);
            }
            return false;
        }
    }
    if (!pending_header_reads) {
        delete header_reader;
        header_reader = NULL;
    }
    write_byte_bill(Billing::HEADER,
                    false,
                    2 + hdrs + prefix_grbs + grbs);

    ReadFull(str_in, ujpg_mrk, 3 ) ;
    write_byte_bill(Billing::HEADER, true, 3);

    write_byte_bill(Billing::DELIMITERS, true, 4 * NUM_THREADS); // trailing vpx_encode bits
    write_byte_bill(Billing::HEADER, true, 4); //trailing size

    if (memcmp(ujpg_mrk, "CMP", 3) != 0) {
        always_assert(false && "CMP must be present (uncompressed) in the file or CNT continue marker");
        return false; // not a JPG
    }
    colldata.signal_worker_should_begin();
    g_decoder->initialize(str_in, thread_handoff);
    colldata.start_decoder(g_decoder);
    return true;
}