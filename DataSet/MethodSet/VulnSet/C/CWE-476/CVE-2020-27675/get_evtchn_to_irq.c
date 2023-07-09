int get_evtchn_to_irq(evtchn_port_t evtchn)
{
	if (evtchn >= xen_evtchn_max_channels())
		return -1;
	if (evtchn_to_irq[EVTCHN_ROW(evtchn)] == NULL)
		return -1;
	return evtchn_to_irq[EVTCHN_ROW(evtchn)][EVTCHN_COL(evtchn)];
}