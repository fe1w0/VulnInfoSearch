static void kvm_vcpu_destroy(struct kvm_vcpu *vcpu)
{
	kvm_dirty_ring_free(&vcpu->dirty_ring);
	kvm_arch_vcpu_destroy(vcpu);

	/*
	 * No need for rcu_read_lock as VCPU_RUN is the only place that changes
	 * the vcpu->pid pointer, and at destruction time all file descriptors
	 * are already gone.
	 */
	put_pid(rcu_dereference_protected(vcpu->pid, 1));

	free_page((unsigned long)vcpu->run);
	kmem_cache_free(kvm_vcpu_cache, vcpu);
}