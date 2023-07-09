bool kvm_irq_delivery_to_apic_fast(struct kvm *kvm, struct kvm_lapic *src,
		struct kvm_lapic_irq *irq, int *r, struct dest_map *dest_map)
{
	struct kvm_apic_map *map;
	unsigned long bitmap;
	struct kvm_lapic **dst = NULL;
	int i;
	bool ret;

	*r = -1;

	if (irq->shorthand == APIC_DEST_SELF) {
		*r = kvm_apic_set_irq(src->vcpu, irq, dest_map);
		return true;
	}

	rcu_read_lock();
	map = rcu_dereference(kvm->arch.apic_map);

	ret = kvm_apic_map_get_dest_lapic(kvm, &src, irq, map, &dst, &bitmap);
	if (ret) {
		*r = 0;
		for_each_set_bit(i, &bitmap, 16) {
			if (!dst[i])
				continue;
			*r += kvm_apic_set_irq(dst[i]->vcpu, irq, dest_map);
		}
	}

	rcu_read_unlock();
	return ret;
}