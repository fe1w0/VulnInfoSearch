static void clear_evtchn_to_irq_row(unsigned row)
{
	unsigned col;

	for (col = 0; col < EVTCHN_PER_ROW; col++)
		evtchn_to_irq[row][col] = -1;
}