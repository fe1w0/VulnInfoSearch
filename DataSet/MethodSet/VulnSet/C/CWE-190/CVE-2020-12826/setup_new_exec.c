void setup_new_exec(struct linux_binprm * bprm)
{
	/*
	 * Once here, prepare_binrpm() will not be called any more, so
	 * the final state of setuid/setgid/fscaps can be merged into the
	 * secureexec flag.
	 */
	bprm->secureexec |= bprm->cap_elevated;

	if (bprm->secureexec) {
		/* Make sure parent cannot signal privileged process. */
		current->pdeath_signal = 0;

		/*
		 * For secureexec, reset the stack limit to sane default to
		 * avoid bad behavior from the prior rlimits. This has to
		 * happen before arch_pick_mmap_layout(), which examines
		 * RLIMIT_STACK, but after the point of no return to avoid
		 * needing to clean up the change on failure.
		 */
		if (bprm->rlim_stack.rlim_cur > _STK_LIM)
			bprm->rlim_stack.rlim_cur = _STK_LIM;
	}

	arch_pick_mmap_layout(current->mm, &bprm->rlim_stack);

	current->sas_ss_sp = current->sas_ss_size = 0;

	/*
	 * Figure out dumpability. Note that this checking only of current
	 * is wrong, but userspace depends on it. This should be testing
	 * bprm->secureexec instead.
	 */
	if (bprm->interp_flags & BINPRM_FLAGS_ENFORCE_NONDUMP ||
	    !(uid_eq(current_euid(), current_uid()) &&
	      gid_eq(current_egid(), current_gid())))
		set_dumpable(current->mm, suid_dumpable);
	else
		set_dumpable(current->mm, SUID_DUMP_USER);

	arch_setup_new_exec();
	perf_event_exec();
	__set_task_comm(current, kbasename(bprm->filename), true);

	/* Set the new mm task size. We have to do that late because it may
	 * depend on TIF_32BIT which is only updated in flush_thread() on
	 * some architectures like powerpc
	 */
	current->mm->task_size = TASK_SIZE;

	/* An exec changes our domain. We are no longer part of the thread
	   group */
	current->self_exec_id++;
	flush_signal_handlers(current, 0);
}