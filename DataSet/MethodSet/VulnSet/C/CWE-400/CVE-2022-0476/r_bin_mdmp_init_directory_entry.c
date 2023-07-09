static bool r_bin_mdmp_init_directory_entry(struct r_bin_mdmp_obj *obj, struct minidump_directory *entry) {
	r_strf_buffer (128);
	struct minidump_handle_operation_list handle_operation_list;
	struct minidump_memory_list memory_list;
	struct minidump_memory64_list memory64_list;
	struct minidump_memory_info_list memory_info_list;
	struct minidump_module_list module_list;
	struct minidump_thread_list thread_list;
	struct minidump_thread_ex_list thread_ex_list;
	struct minidump_thread_info_list thread_info_list;
	struct minidump_token_info_list token_info_list;
	struct minidump_unloaded_module_list unloaded_module_list;
	ut64 offset;
	int i, r;

	/* We could confirm data sizes but a malcious MDMP will always get around
	** this! But we can ensure that the data is not outside of the file */
	if ((ut64)entry->location.rva + entry->location.data_size > r_buf_size (obj->b)) {
		eprintf ("[ERROR] Size Mismatch - Stream data is larger than file size!\n");
		return false;
	}

	switch (entry->stream_type) {
	case THREAD_LIST_STREAM:
		r = r_buf_read_at (obj->b, entry->location.rva, (ut8 *)&thread_list, sizeof (thread_list));
		if (r != sizeof (thread_list)) {
			break;
		}

		sdb_set (obj->kv, "mdmp_thread.format", "ddddq?? "
			"ThreadId SuspendCount PriorityClass Priority "
			"Teb (mdmp_memory_descriptor)Stack "
			"(mdmp_location_descriptor)ThreadContext", 0);
		sdb_num_set (obj->kv, "mdmp_thread_list.offset",
			entry->location.rva, 0);
		sdb_set (obj->kv, "mdmp_thread_list.format",
			r_strf ("d[%d]? "
				"NumberOfThreads (mdmp_thread)Threads",
				thread_list.number_of_threads),
			0);

		/* TODO: Not yet fully parsed or utilised */
		break;
	case MODULE_LIST_STREAM:
		module_list.number_of_modules = r_buf_read_le32_at (obj->b, entry->location.rva);

		sdb_set (obj->kv, "mdmp_module.format", "qddtd???qq "
			"BaseOfImage SizeOfImage CheckSum "
			"TimeDateStamp ModuleNameRVA "
			"(mdmp_vs_fixedfileinfo)VersionInfo "
			"(mdmp_location_descriptor)CvRecord "
			"(mdmp_location_descriptor)MiscRecord "
			"Reserved0 Reserved1", 0);
		sdb_num_set (obj->kv, "mdmp_module_list.offset",
			entry->location.rva, 0);
		sdb_set (obj->kv, "mdmp_module_list.format",
			r_strf ("d[%d]? "
				"NumberOfModule (mdmp_module)Modules",
				module_list.number_of_modules),
			0);

		offset = entry->location.rva + sizeof (module_list);
		for (i = 0; i < module_list.number_of_modules; i++) {
			struct minidump_module *module = R_NEW (struct minidump_module);
			if (!module) {
				break;
			}
			read_module (obj->b, offset, module);
			r_list_append (obj->streams.modules, module);
			offset += sizeof (*module);
		}
		break;
	case MEMORY_LIST_STREAM:
		r = r_buf_read_at (obj->b, entry->location.rva, (ut8 *)&memory_list, sizeof (memory_list));
		if (r != sizeof (memory_list)) {
			break;
		}

		sdb_num_set (obj->kv, "mdmp_memory_list.offset",
			entry->location.rva, 0);
		sdb_set (obj->kv, "mdmp_memory_list.format",
			r_strf ("d[%d]? "
				"NumberOfMemoryRanges "
				"(mdmp_memory_descriptor)MemoryRanges ",
				memory_list.number_of_memory_ranges),
			0);

		offset = entry->location.rva + sizeof (memory_list);
		for (i = 0; i < memory_list.number_of_memory_ranges; i++) {
			struct minidump_memory_descriptor *desc = R_NEW (struct minidump_memory_descriptor);
			if (!desc) {
				break;
			}
			r = r_buf_read_at (obj->b, offset, (ut8 *)desc, sizeof (*desc));
			if (r != sizeof (*desc)) {
				break;
			}
			r_list_append (obj->streams.memories, desc);
			offset += sizeof (*desc);
		}
		break;
	case EXCEPTION_STREAM:
		/* TODO: Not yet fully parsed or utilised */
		obj->streams.exception = R_NEW (struct minidump_exception_stream);
		if (!obj->streams.exception) {
			break;
		}

		r = r_buf_read_at (obj->b, entry->location.rva, (ut8 *)obj->streams.exception, sizeof (*obj->streams.exception));
		if (r != sizeof (*obj->streams.exception)) {
			break;
		}

		sdb_set (obj->kv, "mdmp_exception.format", "[4]E[4]Eqqdd[15]q "
							   "(mdmp_exception_code)ExceptionCode "
							   "(mdmp_exception_flags)ExceptionFlags "
							   "ExceptionRecord ExceptionAddress "
							   "NumberParameters __UnusedAlignment "
							   "ExceptionInformation",
			0);
		sdb_num_set (obj->kv, "mdmp_exception_stream.offset",
			entry->location.rva, 0);
		sdb_set (obj->kv, "mdmp_exception_stream.format", "dd?? "
								  "ThreadId __Alignment "
								  "(mdmp_exception)ExceptionRecord "
								  "(mdmp_location_descriptor)ThreadContext",
			0);

		break;
	case SYSTEM_INFO_STREAM:
		obj->streams.system_info = R_NEW (struct minidump_system_info);
		if (!obj->streams.system_info) {
			break;
		}
		r = r_buf_read_at (obj->b, entry->location.rva, (ut8 *)obj->streams.system_info, sizeof (*obj->streams.system_info));
		if (r != sizeof (*obj->streams.system_info)) {
			break;
		}

		sdb_num_set (obj->kv, "mdmp_system_info.offset",
			entry->location.rva, 0);
		/* TODO: We need E as a byte! */
		sdb_set (obj->kv, "mdmp_system_info.format", "[2]EwwbBddd[4]Ed[2]Ew[2]q "
			"(mdmp_processor_architecture)ProcessorArchitecture "
			"ProcessorLevel ProcessorRevision NumberOfProcessors "
			"(mdmp_product_type)ProductType "
			"MajorVersion MinorVersion BuildNumber (mdmp_platform_id)PlatformId "
			"CsdVersionRva (mdmp_suite_mask)SuiteMask Reserved2 ProcessorFeatures", 0);

		break;
	case THREAD_EX_LIST_STREAM:
		/* TODO: Not yet fully parsed or utilised */
		r = r_buf_read_at (obj->b, entry->location.rva, (ut8 *)&thread_ex_list, sizeof (thread_ex_list));
		if (r != sizeof (thread_ex_list)) {
			break;
		}

		sdb_set (obj->kv, "mdmp_thread_ex.format", "ddddq??? "
			"ThreadId SuspendCount PriorityClass Priority "
			"Teb (mdmp_memory_descriptor)Stack "
			"(mdmp_location_descriptor)ThreadContext "
			"(mdmp_memory_descriptor)BackingStore", 0);
		sdb_num_set (obj->kv, "mdmp_thread_ex_list.offset",
			entry->location.rva, 0);
		sdb_set (obj->kv, "mdmp_thread_ex_list.format",
			r_strf ("d[%d]? NumberOfThreads "
				"(mdmp_thread_ex)Threads",
				thread_ex_list.number_of_threads),
			0);

		offset = entry->location.rva + sizeof (thread_ex_list);
		for (i = 0; i < thread_ex_list.number_of_threads; i++) {
			struct minidump_thread_ex *thread = R_NEW (struct minidump_thread_ex);
			if (!thread) {
				break;
			}
			r = r_buf_read_at (obj->b, offset, (ut8 *)thread, sizeof (*thread));
			if (r != sizeof (*thread)) {
				break;
			}
			r_list_append (obj->streams.ex_threads, thread);
			offset += sizeof (*thread);
		}
		break;
	case MEMORY_64_LIST_STREAM:
		read_memory64_list (obj->b, entry->location.rva, &memory64_list);

		sdb_num_set (obj->kv, "mdmp_memory64_list.offset",
			entry->location.rva, 0);
		sdb_set (obj->kv, "mdmp_memory64_list.format",
			r_strf ("qq[%"PFMT64d"]? NumberOfMemoryRanges "
				"BaseRva "
				"(mdmp_memory_descriptor64)MemoryRanges",
				memory64_list.number_of_memory_ranges),
			0);

		obj->streams.memories64.base_rva = memory64_list.base_rva;
		offset = entry->location.rva + sizeof (memory64_list);
		for (i = 0; i < memory64_list.number_of_memory_ranges; i++) {
			struct minidump_memory_descriptor64 *desc = R_NEW (struct minidump_memory_descriptor64);
			if (!desc) {
				break;
			}
			read_desc (obj->b, offset, desc);
			r_list_append (obj->streams.memories64.memories, desc);
			offset += sizeof (*desc);
		}
		break;
	case COMMENT_STREAM_A:
		/* TODO: Not yet fully parsed or utilised */
		obj->streams.comments_a = R_NEWS (ut8, COMMENTS_SIZE);
		if (!obj->streams.comments_a) {
			break;
		}
		r = r_buf_read_at (obj->b, entry->location.rva, obj->streams.comments_a, COMMENTS_SIZE);
		if (r != COMMENTS_SIZE) {
			break;
		}

		sdb_num_set (obj->kv, "mdmp_comment_stream_a.offset",
			entry->location.rva, 0);
		sdb_set (obj->kv, "mdmp_comment_stream_a.format",
			"s CommentA", 0);

		break;
	case COMMENT_STREAM_W:
		/* TODO: Not yet fully parsed or utilised */
		obj->streams.comments_w = R_NEWS (ut8, COMMENTS_SIZE);
		if (!obj->streams.comments_w) {
			break;
		}
		r = r_buf_read_at (obj->b, entry->location.rva, obj->streams.comments_w, COMMENTS_SIZE);
		if (r != COMMENTS_SIZE) {
			break;
		}

		sdb_num_set (obj->kv, "mdmp_comment_stream_w.offset",
			entry->location.rva, 0);
		sdb_set (obj->kv, "mdmp_comment_stream_w.format",
				"s CommentW", 0);

		break;
	case HANDLE_DATA_STREAM:
		/* TODO: Not yet fully parsed or utilised */
		obj->streams.handle_data = R_NEW (struct minidump_handle_data_stream);
		if (!obj->streams.handle_data) {
			break;
		}
		r = r_buf_read_at (obj->b, entry->location.rva, (ut8 *)obj->streams.handle_data, sizeof (*obj->streams.handle_data));
		if (r != sizeof (*obj->streams.handle_data)) {
			break;
		}

		sdb_num_set (obj->kv, "mdmp_handle_data_stream.offset",
				entry->location.rva, 0);
		sdb_set (obj->kv, "mdmp_handle_data_stream.format", "dddd "
				"SizeOfHeader SizeOfDescriptor "
				"NumberOfDescriptors Reserved", 0);
		break;
	case FUNCTION_TABLE_STREAM:
		/* TODO: Not yet fully parsed or utilised */
		obj->streams.function_table = R_NEW (struct minidump_function_table_stream);
		if (!obj->streams.function_table) {
			break;
		}
		r = r_buf_read_at (obj->b, entry->location.rva, (ut8 *)obj->streams.function_table, sizeof (*obj->streams.function_table));
		if (r != sizeof (*obj->streams.function_table)) {
			break;
		}

		sdb_num_set (obj->kv, "mdmp_function_table_stream.offset",
			entry->location.rva, 0);
		sdb_set (obj->kv, "mdmp_function_table_stream.format", "dddddd "
			"SizeOfHeader SizeOfDescriptor SizeOfNativeDescriptor "
			"SizeOfFunctionEntry NumberOfDescriptors SizeOfAlignPad",
			0);
		break;
	case UNLOADED_MODULE_LIST_STREAM:
		/* TODO: Not yet fully parsed or utilised */
		r = r_buf_read_at (obj->b, entry->location.rva, (ut8 *)&unloaded_module_list, sizeof (unloaded_module_list));
		if (r != sizeof (unloaded_module_list)) {
			break;
		}

		sdb_set (obj->kv, "mdmp_unloaded_module.format", "qddtd "
			"BaseOfImage SizeOfImage CheckSum TimeDateStamp "
			"ModuleNameRva", 0);
		sdb_num_set (obj->kv, "mdmp_unloaded_module_list.offset",
			entry->location.rva, 0);
		sdb_set (obj->kv, "mdmp_unloaded_module_list.format", "ddd "
			"SizeOfHeader SizeOfEntry NumberOfEntries", 0);

		offset = entry->location.rva + sizeof (unloaded_module_list);
		for (i = 0; i < unloaded_module_list.number_of_entries; i++) {
			struct minidump_unloaded_module *module = R_NEW (struct minidump_unloaded_module);
			if (!module) {
				break;
			}
			r = r_buf_read_at (obj->b, offset, (ut8 *)module, sizeof (*module));
			if (r != sizeof (*module)) {
				break;
			}
			r_list_append (obj->streams.unloaded_modules, module);
			offset += sizeof (*module);
		}
		break;
	case MISC_INFO_STREAM:
		/* TODO: Not yet fully parsed or utilised */
		obj->streams.misc_info.misc_info_1 = R_NEW (struct minidump_misc_info);
		if (!obj->streams.misc_info.misc_info_1) {
			break;
		}
		r = r_buf_read_at (obj->b, entry->location.rva, (ut8 *)obj->streams.misc_info.misc_info_1, sizeof (*obj->streams.misc_info.misc_info_1));
		if (r != sizeof (*obj->streams.misc_info.misc_info_1)) {
			break;
		}

		/* TODO: Handle different sizes */
		sdb_num_set (obj->kv, "mdmp_misc_info.offset",
			entry->location.rva, 0);
		sdb_set (obj->kv, "mdmp_misc_info.format", "d[4]Bdtttddddd "
			"SizeOfInfo (mdmp_misc1_flags)Flags1 ProcessId "
			"ProcessCreateTime ProcessUserTime ProcessKernelTime "
			"ProcessorMaxMhz ProcessorCurrentMhz "
			"ProcessorMhzLimit ProcessorMaxIdleState "
			"ProcessorCurrentIdleState", 0);

		break;
	case MEMORY_INFO_LIST_STREAM:
		r = r_buf_read_at (obj->b, entry->location.rva, (ut8 *)&memory_info_list, sizeof (memory_info_list));
		if (r != sizeof (memory_info_list)) {
			break;
		}

		sdb_set (obj->kv, "mdmp_memory_info.format",
			"qq[4]Edq[4]E[4]E[4]Ed BaseAddress AllocationBase "
			"(mdmp_page_protect)AllocationProtect __Alignment1 RegionSize "
			"(mdmp_mem_state)State (mdmp_page_protect)Protect "
			"(mdmp_mem_type)Type __Alignment2", 0);
		sdb_num_set (obj->kv, "mdmp_memory_info_list.offset",
			entry->location.rva, 0);
		sdb_set (obj->kv, "mdmp_memory_info_list.format",
			r_strf ("ddq[%"PFMT64d"]? SizeOfHeader SizeOfEntry "
				"NumberOfEntries (mdmp_memory_info)MemoryInfo",
				memory_info_list.number_of_entries),
			0);

		offset = entry->location.rva + sizeof (memory_info_list);
		for (i = 0; i < memory_info_list.number_of_entries; i++) {
			struct minidump_memory_info *info = R_NEW (struct minidump_memory_info);
			if (!info) {
				break;
			}
			r = r_buf_read_at (obj->b, offset, (ut8 *)info, sizeof (*info));
			if (r != sizeof (*info)) {
				break;
			}
			r_list_append (obj->streams.memory_infos, info);
			offset += sizeof (*info);
		}
		break;
	case THREAD_INFO_LIST_STREAM:
		/* TODO: Not yet fully parsed or utilised */
		r = r_buf_read_at (obj->b, entry->location.rva, (ut8 *)&thread_info_list, sizeof (thread_info_list));
		if (r != sizeof (thread_info_list)) {
			break;
		}

		sdb_set (obj->kv, "mdmp_thread_info.format", "ddddttttqq "
			"ThreadId DumpFlags DumpError ExitStatus CreateTime "
			"ExitTime KernelTime UserTime StartAddress Affinity",
			0);
		sdb_num_set (obj->kv, "mdmp_thread_info_list.offset",
				entry->location.rva, 0);
		sdb_set (obj->kv, "mdmp_thread_info_list.format", "ddd "
			"SizeOfHeader SizeOfEntry NumberOfEntries", 0);

		offset = entry->location.rva + sizeof (thread_info_list);
		for (i = 0; i < thread_info_list.number_of_entries; i++) {
			struct minidump_thread_info *info = R_NEW (struct minidump_thread_info);
			if (!info) {
				break;
			}
			r = r_buf_read_at (obj->b, offset, (ut8 *)info, sizeof (*info));
			if (r != sizeof (*info)) {
				break;
			}
			r_list_append (obj->streams.thread_infos, info);
			offset += sizeof (*info);
		}
		break;
	case HANDLE_OPERATION_LIST_STREAM:
		/* TODO: Not yet fully parsed or utilised */
		r = r_buf_read_at (obj->b, entry->location.rva, (ut8 *)&handle_operation_list, sizeof (handle_operation_list));
		if (r != sizeof (handle_operation_list)) {
			break;
		}

		sdb_num_set (obj->kv, "mdmp_handle_operation_list.offset",
			entry->location.rva, 0);
		sdb_set (obj->kv, "mdmp_handle_operation_list.format", "dddd "
			"SizeOfHeader SizeOfEntry NumberOfEntries Reserved", 0);

		offset = entry->location.rva + sizeof (handle_operation_list);
		for (i = 0; i < handle_operation_list.number_of_entries; i++) {
			struct avrf_handle_operation *op = R_NEW (struct avrf_handle_operation);
			if (!op) {
				break;
			}
			r = r_buf_read_at (obj->b, offset, (ut8 *)op, sizeof (*op));
			if (r != sizeof (*op)) {
				break;
			}
			r_list_append (obj->streams.operations, op);
			offset += sizeof (*op);
		}

		break;
	case TOKEN_STREAM:
		/* TODO: Not fully parsed or utilised */
		r = r_buf_read_at (obj->b, entry->location.rva, (ut8 *)&token_info_list, sizeof (token_info_list));
		if (r != sizeof (token_info_list)) {
			break;
		}

		sdb_set (obj->kv, "mdmp_token_info.format", "ddq "
			"TokenSize TokenId TokenHandle", 0);

		sdb_num_set (obj->kv, "mdmp_token_info_list.offset",
			entry->location.rva, 0);
		sdb_set (obj->kv, "mdmp_token_info_list.format", "dddd "
			"TokenListSize TokenListEntries ListHeaderSize ElementHeaderSize", 0);

		offset = entry->location.rva + sizeof (token_info_list);
		for (i = 0; i < token_info_list.number_of_entries; i++) {
			struct minidump_token_info *info = R_NEW (struct minidump_token_info);
			if (!info) {
				break;
			}
			r = r_buf_read_at (obj->b, offset, (ut8 *)info, sizeof (*info));
			if (r != sizeof (*info)) {
				break;
			}
			r_list_append (obj->streams.token_infos, info);
			offset += sizeof (*info);
		}
		break;

	case LAST_RESERVED_STREAM:
		/* TODO: Not yet fully parsed or utilised */
		break;
	case UNUSED_STREAM:
	case RESERVED_STREAM_0:
	case RESERVED_STREAM_1:
		/* Silently ignore reserved streams */
		break;
	default:
		eprintf ("[WARN] Invalid or unsupported enumeration encountered %d\n", entry->stream_type);
		break;
	}
	return true;
}