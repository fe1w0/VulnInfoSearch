static void xen_free_irq(unsigned irq)
{
	struct irq_info *info = info_for_irq(irq);
	unsigned long flags;

	if (WARN_ON(!info))
		return;

	write_lock_irqsave(&evtchn_rwlock, flags);

	list_del(&info->list);

	set_info_for_irq(irq, NULL);

	WARN_ON(info->refcnt > 0);

	write_unlock_irqrestore(&evtchn_rwlock, flags);

	kfree(info);

	/* Legacy IRQ descriptors are managed by the arch. */
	if (irq < nr_legacy_irqs())
		return;

	irq_free_desc(irq);
}