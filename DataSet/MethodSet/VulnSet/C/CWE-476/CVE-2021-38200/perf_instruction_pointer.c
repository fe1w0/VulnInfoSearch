unsigned long perf_instruction_pointer(struct pt_regs *regs)
{
	bool use_siar = regs_use_siar(regs);
	unsigned long siar = mfspr(SPRN_SIAR);

	if (ppmu->flags & PPMU_P10_DD1) {
		if (siar)
			return siar;
		else
			return regs->nip;
	} else if (use_siar && siar_valid(regs))
		return mfspr(SPRN_SIAR) + perf_ip_adjust(regs);
	else if (use_siar)
		return 0;		// no valid instruction pointer
	else
		return regs->nip;
}