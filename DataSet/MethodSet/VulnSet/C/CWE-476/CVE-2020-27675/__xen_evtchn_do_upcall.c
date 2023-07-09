static void __xen_evtchn_do_upcall(void)
{
	struct vcpu_info *vcpu_info = __this_cpu_read(xen_vcpu);
	int cpu = smp_processor_id();

	do {
		vcpu_info->evtchn_upcall_pending = 0;

		xen_evtchn_handle_events(cpu);

		BUG_ON(!irqs_disabled());

		virt_rmb(); /* Hypervisor can set upcall pending. */

	} while (vcpu_info->evtchn_upcall_pending);
}