static int stimer_set_count(struct kvm_vcpu_hv_stimer *stimer, u64 count,
			    bool host)
{
	struct kvm_vcpu *vcpu = hv_stimer_to_vcpu(stimer);
	struct kvm_vcpu_hv_synic *synic = to_hv_synic(vcpu);

	if (!synic->active && !host)
		return 1;

	trace_kvm_hv_stimer_set_count(hv_stimer_to_vcpu(stimer)->vcpu_id,
				      stimer->index, count, host);

	stimer_cleanup(stimer);
	stimer->count = count;
	if (stimer->count == 0)
		stimer->config.enable = 0;
	else if (stimer->config.auto_enable)
		stimer->config.enable = 1;

	if (stimer->config.enable)
		stimer_mark_pending(stimer, false);

	return 0;
}