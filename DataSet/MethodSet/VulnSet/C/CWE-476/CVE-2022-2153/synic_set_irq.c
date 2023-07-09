static int synic_set_irq(struct kvm_vcpu_hv_synic *synic, u32 sint)
{
	struct kvm_vcpu *vcpu = hv_synic_to_vcpu(synic);
	struct kvm_lapic_irq irq;
	int ret, vector;

	if (sint >= ARRAY_SIZE(synic->sint))
		return -EINVAL;

	vector = synic_get_sint_vector(synic_read_sint(synic, sint));
	if (vector < 0)
		return -ENOENT;

	memset(&irq, 0, sizeof(irq));
	irq.shorthand = APIC_DEST_SELF;
	irq.dest_mode = APIC_DEST_PHYSICAL;
	irq.delivery_mode = APIC_DM_FIXED;
	irq.vector = vector;
	irq.level = 1;

	ret = kvm_irq_delivery_to_apic(vcpu->kvm, vcpu->arch.apic, &irq, NULL);
	trace_kvm_hv_synic_set_irq(vcpu->vcpu_id, sint, irq.vector, ret);
	return ret;
}