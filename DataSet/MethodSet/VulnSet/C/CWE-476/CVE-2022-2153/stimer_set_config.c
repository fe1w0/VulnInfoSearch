static int stimer_set_config(struct kvm_vcpu_hv_stimer *stimer, u64 config,
			     bool host)
{
	union hv_stimer_config new_config = {.as_uint64 = config},
		old_config = {.as_uint64 = stimer->config.as_uint64};
	struct kvm_vcpu *vcpu = hv_stimer_to_vcpu(stimer);
	struct kvm_vcpu_hv *hv_vcpu = to_hv_vcpu(vcpu);
	struct kvm_vcpu_hv_synic *synic = to_hv_synic(vcpu);

	if (!synic->active && !host)
		return 1;

	if (unlikely(!host && hv_vcpu->enforce_cpuid && new_config.direct_mode &&
		     !(hv_vcpu->cpuid_cache.features_edx &
		       HV_STIMER_DIRECT_MODE_AVAILABLE)))
		return 1;

	trace_kvm_hv_stimer_set_config(hv_stimer_to_vcpu(stimer)->vcpu_id,
				       stimer->index, config, host);

	stimer_cleanup(stimer);
	if (old_config.enable &&
	    !new_config.direct_mode && new_config.sintx == 0)
		new_config.enable = 0;
	stimer->config.as_uint64 = new_config.as_uint64;

	if (stimer->config.enable)
		stimer_mark_pending(stimer, false);

	return 0;
}