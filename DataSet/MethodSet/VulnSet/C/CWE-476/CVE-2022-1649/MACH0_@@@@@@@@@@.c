void MACH0_(iterate_chained_fixups)(struct MACH0_(obj_t) *bin, ut64 limit_start, ut64 limit_end, ut32 event_mask, RFixupCallback callback, void * context) {
	int i = 0;
	for (; i < bin->nsegs && i < bin->segs_count; i++) {
		if (!bin->chained_starts[i]) {
			continue;
		}
		int page_size = bin->chained_starts[i]->page_size;
		if (page_size < 1) {
			page_size = 4096;
		}
		ut64 start = bin->segs[i].fileoff;
		ut64 end = start + bin->segs[i].filesize;
		if (end >= limit_start && start <= limit_end) {
			ut64 page_idx = (R_MAX (start, limit_start) - start) / page_size;
			ut64 page_end_idx = (R_MIN (limit_end, end) - start) / page_size;
			for (; page_idx <= page_end_idx; page_idx++) {
				if (page_idx >= bin->chained_starts[i]->page_count) {
					break;
				}
				ut16 page_start = bin->chained_starts[i]->page_start[page_idx];
				if (page_start == DYLD_CHAINED_PTR_START_NONE) {
					continue;
				}
				ut64 cursor = start + page_idx * page_size + page_start;
				while (cursor < limit_end && cursor < end) {
					ut8 tmp[8];
					bool previous_rebasing = bin->rebasing_buffer;
					bin->rebasing_buffer = true;
					if (r_buf_read_at (bin->b, cursor, tmp, 8) != 8) {
						bin->rebasing_buffer = previous_rebasing;
						break;
					}
					bin->rebasing_buffer = previous_rebasing;
					ut64 raw_ptr = r_read_le64 (tmp);
					ut64 ptr_value = raw_ptr;
					ut64 delta, stride, addend;
					ut16 pointer_format = bin->chained_starts[i]->pointer_format;
					RFixupEvent event = R_FIXUP_EVENT_NONE;
					ut8 key = 0, addr_div = 0;
					ut16 diversity = 0;
					ut32 ordinal = UT32_MAX;
					switch (pointer_format) {
					case DYLD_CHAINED_PTR_ARM64E:
						{
						stride = 8;
						bool is_auth = IS_PTR_AUTH (raw_ptr);
						bool is_bind = IS_PTR_BIND (raw_ptr);
						if (is_auth && is_bind) {
							struct dyld_chained_ptr_arm64e_auth_bind *p =
									(struct dyld_chained_ptr_arm64e_auth_bind *) &raw_ptr;
							event = R_FIXUP_EVENT_BIND_AUTH;
							delta = p->next;
							ordinal = p->ordinal;
							key = p->key;
							addr_div = p->addrDiv;
							diversity = p->diversity;
						} else if (!is_auth && is_bind) {
							struct dyld_chained_ptr_arm64e_bind *p =
									(struct dyld_chained_ptr_arm64e_bind *) &raw_ptr;
							event = R_FIXUP_EVENT_BIND;
							delta = p->next;
							ordinal = p->ordinal;
							addend = p->addend;
						} else if (is_auth && !is_bind) {
							struct dyld_chained_ptr_arm64e_auth_rebase *p =
									(struct dyld_chained_ptr_arm64e_auth_rebase *) &raw_ptr;
							event = R_FIXUP_EVENT_REBASE_AUTH;
							delta = p->next;
							ptr_value = p->target + bin->baddr;
							key = p->key;
							addr_div = p->addrDiv;
							diversity = p->diversity;
						} else {
							struct dyld_chained_ptr_arm64e_rebase *p =
									(struct dyld_chained_ptr_arm64e_rebase *) &raw_ptr;
							event = R_FIXUP_EVENT_REBASE;
							delta = p->next;
							ptr_value = ((ut64)p->high8 << 56) | p->target;
						}
						}
						break;
					case DYLD_CHAINED_PTR_ARM64E_USERLAND24:
						{
						stride = 8;
						struct dyld_chained_ptr_arm64e_bind24 *bind =
								(struct dyld_chained_ptr_arm64e_bind24 *) &raw_ptr;
						if (bind->bind) {
							delta = bind->next;
							if (bind->auth) {
								struct dyld_chained_ptr_arm64e_auth_bind24 *p =
										(struct dyld_chained_ptr_arm64e_auth_bind24 *) &raw_ptr;
								event = R_FIXUP_EVENT_BIND_AUTH;
								ordinal = p->ordinal;
								key = p->key;
								addr_div = p->addrDiv;
								diversity = p->diversity;
							} else {
								event = R_FIXUP_EVENT_BIND;
								ordinal = bind->ordinal;
								addend = bind->addend;
							}
						} else {
							if (bind->auth) {
								struct dyld_chained_ptr_arm64e_auth_rebase *p =
										(struct dyld_chained_ptr_arm64e_auth_rebase *) &raw_ptr;
								event = R_FIXUP_EVENT_REBASE_AUTH;
								delta = p->next;
								ptr_value = p->target + bin->baddr;
								key = p->key;
								addr_div = p->addrDiv;
								diversity = p->diversity;
							} else {
								struct dyld_chained_ptr_arm64e_rebase *p =
									(struct dyld_chained_ptr_arm64e_rebase *) &raw_ptr;
								event = R_FIXUP_EVENT_REBASE;
								delta = p->next;
								ptr_value = bin->baddr + (((ut64)p->high8 << 56) | p->target);
							}
						}
						}
						break;
					case DYLD_CHAINED_PTR_64:
					case DYLD_CHAINED_PTR_64_OFFSET:
						{
						stride = 4;
						struct dyld_chained_ptr_64_bind *bind =
								(struct dyld_chained_ptr_64_bind *) &raw_ptr;
						if (bind->bind) {
							event = R_FIXUP_EVENT_BIND;
							delta = bind->next;
							ordinal = bind->ordinal;
							addend = bind->addend;
						} else {
							struct dyld_chained_ptr_64_rebase *p =
								(struct dyld_chained_ptr_64_rebase *) &raw_ptr;
							event = R_FIXUP_EVENT_REBASE;
							delta = p->next;
							ptr_value = ((ut64)p->high8 << 56) | p->target;
							if (pointer_format == DYLD_CHAINED_PTR_64_OFFSET) {
								ptr_value += bin->baddr;
							}
						}
						}
						break;
					default:
						eprintf ("Unsupported chained pointer format %d\n", pointer_format);
						return;
					}
					if (cursor >= limit_start && cursor <= limit_end - 8 && (event & event_mask) != 0) {
						bool carry_on;
						switch (event) {
						case R_FIXUP_EVENT_BIND: {
							RFixupBindEventDetails event_details;

							event_details.type = event;
							event_details.bin = bin;
							event_details.offset = cursor;
							event_details.raw_ptr = raw_ptr;
							event_details.ordinal = ordinal;
							event_details.addend = addend;

							carry_on = callback (context, (RFixupEventDetails *) &event_details);
							break;
						}
						case R_FIXUP_EVENT_BIND_AUTH: {
							RFixupBindAuthEventDetails event_details;

							event_details.type = event;
							event_details.bin = bin;
							event_details.offset = cursor;
							event_details.raw_ptr = raw_ptr;
							event_details.ordinal = ordinal;
							event_details.key = key;
							event_details.addr_div = addr_div;
							event_details.diversity = diversity;

							carry_on = callback (context, (RFixupEventDetails *) &event_details);
							break;
						}
						case R_FIXUP_EVENT_REBASE: {
							RFixupRebaseEventDetails event_details;

							event_details.type = event;
							event_details.bin = bin;
							event_details.offset = cursor;
							event_details.raw_ptr = raw_ptr;
							event_details.ptr_value = ptr_value;

							carry_on = callback (context, (RFixupEventDetails *) &event_details);
							break;
						}
						case R_FIXUP_EVENT_REBASE_AUTH: {
							RFixupRebaseAuthEventDetails event_details;

							event_details.type = event;
							event_details.bin = bin;
							event_details.offset = cursor;
							event_details.raw_ptr = raw_ptr;
							event_details.ptr_value = ptr_value;
							event_details.key = key;
							event_details.addr_div = addr_div;
							event_details.diversity = diversity;

							carry_on = callback (context, (RFixupEventDetails *) &event_details);
							break;
						}
						default:
							eprintf ("Unexpected event while iterating chained fixups\n");
							carry_on = false;
							break;
						}
						if (!carry_on) {
							return;
						}
					}
					cursor += delta * stride;
					if (!delta) {
						break;
					}
				}
			}
		}
	}
}