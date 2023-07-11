static void xen_free_irq(unsigned irq)
{
	struct irq_info *info = info_for_irq(irq);

	if (WARN_ON(!info))
		return;

	list_del(&info->list);

	set_info_for_irq(irq, NULL);

	WARN_ON(info->refcnt > 0);

	kfree(info);

	/* Legacy IRQ descriptors are managed by the arch. */
	if (irq < nr_legacy_irqs())
		return;

	irq_free_desc(irq);
}