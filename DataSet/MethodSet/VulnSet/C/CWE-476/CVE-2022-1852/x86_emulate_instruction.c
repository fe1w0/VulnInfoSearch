int x86_emulate_instruction(struct kvm_vcpu *vcpu, gpa_t cr2_or_gpa,
			    int emulation_type, void *insn, int insn_len)
{
	int r;
	struct x86_emulate_ctxt *ctxt = vcpu->arch.emulate_ctxt;
	bool writeback = true;
	bool write_fault_to_spt;

	if (unlikely(!kvm_can_emulate_insn(vcpu, emulation_type, insn, insn_len)))
		return 1;

	vcpu->arch.l1tf_flush_l1d = true;

	/*
	 * Clear write_fault_to_shadow_pgtable here to ensure it is
	 * never reused.
	 */
	write_fault_to_spt = vcpu->arch.write_fault_to_shadow_pgtable;
	vcpu->arch.write_fault_to_shadow_pgtable = false;

	if (!(emulation_type & EMULTYPE_NO_DECODE)) {
		kvm_clear_exception_queue(vcpu);

		r = x86_decode_emulated_instruction(vcpu, emulation_type,
						    insn, insn_len);
		if (r != EMULATION_OK)  {
			if ((emulation_type & EMULTYPE_TRAP_UD) ||
			    (emulation_type & EMULTYPE_TRAP_UD_FORCED)) {
				kvm_queue_exception(vcpu, UD_VECTOR);
				return 1;
			}
			if (reexecute_instruction(vcpu, cr2_or_gpa,
						  write_fault_to_spt,
						  emulation_type))
				return 1;
			if (ctxt->have_exception) {
				/*
				 * #UD should result in just EMULATION_FAILED, and trap-like
				 * exception should not be encountered during decode.
				 */
				WARN_ON_ONCE(ctxt->exception.vector == UD_VECTOR ||
					     exception_type(ctxt->exception.vector) == EXCPT_TRAP);
				inject_emulated_exception(vcpu);
				return 1;
			}
			return handle_emulation_failure(vcpu, emulation_type);
		}
	}

	if ((emulation_type & EMULTYPE_VMWARE_GP) &&
	    !is_vmware_backdoor_opcode(ctxt)) {
		kvm_queue_exception_e(vcpu, GP_VECTOR, 0);
		return 1;
	}

	/*
	 * EMULTYPE_SKIP without EMULTYPE_COMPLETE_USER_EXIT is intended for
	 * use *only* by vendor callbacks for kvm_skip_emulated_instruction().
	 * The caller is responsible for updating interruptibility state and
	 * injecting single-step #DBs.
	 */
	if (emulation_type & EMULTYPE_SKIP) {
		if (ctxt->mode != X86EMUL_MODE_PROT64)
			ctxt->eip = (u32)ctxt->_eip;
		else
			ctxt->eip = ctxt->_eip;

		if (emulation_type & EMULTYPE_COMPLETE_USER_EXIT) {
			r = 1;
			goto writeback;
		}

		kvm_rip_write(vcpu, ctxt->eip);
		if (ctxt->eflags & X86_EFLAGS_RF)
			kvm_set_rflags(vcpu, ctxt->eflags & ~X86_EFLAGS_RF);
		return 1;
	}

	if (retry_instruction(ctxt, cr2_or_gpa, emulation_type))
		return 1;

	/* this is needed for vmware backdoor interface to work since it
	   changes registers values  during IO operation */
	if (vcpu->arch.emulate_regs_need_sync_from_vcpu) {
		vcpu->arch.emulate_regs_need_sync_from_vcpu = false;
		emulator_invalidate_register_cache(ctxt);
	}

restart:
	if (emulation_type & EMULTYPE_PF) {
		/* Save the faulting GPA (cr2) in the address field */
		ctxt->exception.address = cr2_or_gpa;

		/* With shadow page tables, cr2 contains a GVA or nGPA. */
		if (vcpu->arch.mmu->root_role.direct) {
			ctxt->gpa_available = true;
			ctxt->gpa_val = cr2_or_gpa;
		}
	} else {
		/* Sanitize the address out of an abundance of paranoia. */
		ctxt->exception.address = 0;
	}

	r = x86_emulate_insn(ctxt);

	if (r == EMULATION_INTERCEPTED)
		return 1;

	if (r == EMULATION_FAILED) {
		if (reexecute_instruction(vcpu, cr2_or_gpa, write_fault_to_spt,
					emulation_type))
			return 1;

		return handle_emulation_failure(vcpu, emulation_type);
	}

	if (ctxt->have_exception) {
		r = 1;
		if (inject_emulated_exception(vcpu))
			return r;
	} else if (vcpu->arch.pio.count) {
		if (!vcpu->arch.pio.in) {
			/* FIXME: return into emulator if single-stepping.  */
			vcpu->arch.pio.count = 0;
		} else {
			writeback = false;
			vcpu->arch.complete_userspace_io = complete_emulated_pio;
		}
		r = 0;
	} else if (vcpu->mmio_needed) {
		++vcpu->stat.mmio_exits;

		if (!vcpu->mmio_is_write)
			writeback = false;
		r = 0;
		vcpu->arch.complete_userspace_io = complete_emulated_mmio;
	} else if (vcpu->arch.complete_userspace_io) {
		writeback = false;
		r = 0;
	} else if (r == EMULATION_RESTART)
		goto restart;
	else
		r = 1;

writeback:
	if (writeback) {
		unsigned long rflags = static_call(kvm_x86_get_rflags)(vcpu);
		toggle_interruptibility(vcpu, ctxt->interruptibility);
		vcpu->arch.emulate_regs_need_sync_to_vcpu = false;
		if (!ctxt->have_exception ||
		    exception_type(ctxt->exception.vector) == EXCPT_TRAP) {
			kvm_pmu_trigger_event(vcpu, PERF_COUNT_HW_INSTRUCTIONS);
			if (ctxt->is_branch)
				kvm_pmu_trigger_event(vcpu, PERF_COUNT_HW_BRANCH_INSTRUCTIONS);
			kvm_rip_write(vcpu, ctxt->eip);
			if (r && (ctxt->tf || (vcpu->guest_debug & KVM_GUESTDBG_SINGLESTEP)))
				r = kvm_vcpu_do_singlestep(vcpu);
			static_call_cond(kvm_x86_update_emulated_instruction)(vcpu);
			__kvm_set_rflags(vcpu, ctxt->eflags);
		}

		/*
		 * For STI, interrupts are shadowed; so KVM_REQ_EVENT will
		 * do nothing, and it will be requested again as soon as
		 * the shadow expires.  But we still need to check here,
		 * because POPF has no interrupt shadow.
		 */
		if (unlikely((ctxt->eflags & ~rflags) & X86_EFLAGS_IF))
			kvm_make_request(KVM_REQ_EVENT, vcpu);
	} else
		vcpu->arch.emulate_regs_need_sync_to_vcpu = true;

	return r;
}