int x86_decode_emulated_instruction(struct kvm_vcpu *vcpu, int emulation_type,
				    void *insn, int insn_len)
{
	int r = EMULATION_OK;
	struct x86_emulate_ctxt *ctxt = vcpu->arch.emulate_ctxt;

	init_emulate_ctxt(vcpu);

	/*
	 * We will reenter on the same instruction since we do not set
	 * complete_userspace_io. This does not handle watchpoints yet,
	 * those would be handled in the emulate_ops.
	 */
	if (!(emulation_type & EMULTYPE_SKIP) &&
	    kvm_vcpu_check_breakpoint(vcpu, &r))
		return r;

	r = x86_decode_insn(ctxt, insn, insn_len, emulation_type);

	trace_kvm_emulate_insn_start(vcpu);
	++vcpu->stat.insn_emulation;

	return r;
}