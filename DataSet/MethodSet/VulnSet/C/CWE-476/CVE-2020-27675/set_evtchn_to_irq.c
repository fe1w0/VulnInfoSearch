static int set_evtchn_to_irq(evtchn_port_t evtchn, unsigned int irq)
{
	unsigned row;
	unsigned col;

	if (evtchn >= xen_evtchn_max_channels())
		return -EINVAL;

	row = EVTCHN_ROW(evtchn);
	col = EVTCHN_COL(evtchn);

	if (evtchn_to_irq[row] == NULL) {
		/* Unallocated irq entries return -1 anyway */
		if (irq == -1)
			return 0;

		evtchn_to_irq[row] = (int *)get_zeroed_page(GFP_KERNEL);
		if (evtchn_to_irq[row] == NULL)
			return -ENOMEM;

		clear_evtchn_to_irq_row(row);
	}

	evtchn_to_irq[row][col] = irq;
	return 0;
}