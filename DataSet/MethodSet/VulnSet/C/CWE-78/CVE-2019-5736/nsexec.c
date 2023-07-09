void nsexec(void)
{
	int pipenum;
	jmp_buf env;
	int sync_child_pipe[2], sync_grandchild_pipe[2];
	struct nlconfig_t config = { 0 };

	/*
	 * If we don't have an init pipe, just return to the go routine.
	 * We'll only get an init pipe for start or exec.
	 */
	pipenum = initpipe();
	if (pipenum == -1)
		return;

	/* Parse all of the netlink configuration. */
	nl_parse(pipenum, &config);

	/* Set oom_score_adj. This has to be done before !dumpable because
	 * /proc/self/oom_score_adj is not writeable unless you're an privileged
	 * user (if !dumpable is set). All children inherit their parent's
	 * oom_score_adj value on fork(2) so this will always be propagated
	 * properly.
	 */
	update_oom_score_adj(config.oom_score_adj, config.oom_score_adj_len);

	/*
	 * Make the process non-dumpable, to avoid various race conditions that
	 * could cause processes in namespaces we're joining to access host
	 * resources (or potentially execute code).
	 *
	 * However, if the number of namespaces we are joining is 0, we are not
	 * going to be switching to a different security context. Thus setting
	 * ourselves to be non-dumpable only breaks things (like rootless
	 * containers), which is the recommendation from the kernel folks.
	 */
	if (config.namespaces) {
		if (prctl(PR_SET_DUMPABLE, 0, 0, 0, 0) < 0)
			bail("failed to set process as non-dumpable");
	}

	/* Pipe so we can tell the child when we've finished setting up. */
	if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sync_child_pipe) < 0)
		bail("failed to setup sync pipe between parent and child");

	/*
	 * We need a new socketpair to sync with grandchild so we don't have
	 * race condition with child.
	 */
	if (socketpair(AF_LOCAL, SOCK_STREAM, 0, sync_grandchild_pipe) < 0)
		bail("failed to setup sync pipe between parent and grandchild");

	/* TODO: Currently we aren't dealing with child deaths properly. */

	/*
	 * Okay, so this is quite annoying.
	 *
	 * In order for this unsharing code to be more extensible we need to split
	 * up unshare(CLONE_NEWUSER) and clone() in various ways. The ideal case
	 * would be if we did clone(CLONE_NEWUSER) and the other namespaces
	 * separately, but because of SELinux issues we cannot really do that. But
	 * we cannot just dump the namespace flags into clone(...) because several
	 * usecases (such as rootless containers) require more granularity around
	 * the namespace setup. In addition, some older kernels had issues where
	 * CLONE_NEWUSER wasn't handled before other namespaces (but we cannot
	 * handle this while also dealing with SELinux so we choose SELinux support
	 * over broken kernel support).
	 *
	 * However, if we unshare(2) the user namespace *before* we clone(2), then
	 * all hell breaks loose.
	 *
	 * The parent no longer has permissions to do many things (unshare(2) drops
	 * all capabilities in your old namespace), and the container cannot be set
	 * up to have more than one {uid,gid} mapping. This is obviously less than
	 * ideal. In order to fix this, we have to first clone(2) and then unshare.
	 *
	 * Unfortunately, it's not as simple as that. We have to fork to enter the
	 * PID namespace (the PID namespace only applies to children). Since we'll
	 * have to double-fork, this clone_parent() call won't be able to get the
	 * PID of the _actual_ init process (without doing more synchronisation than
	 * I can deal with at the moment). So we'll just get the parent to send it
	 * for us, the only job of this process is to update
	 * /proc/pid/{setgroups,uid_map,gid_map}.
	 *
	 * And as a result of the above, we also need to setns(2) in the first child
	 * because if we join a PID namespace in the topmost parent then our child
	 * will be in that namespace (and it will not be able to give us a PID value
	 * that makes sense without resorting to sending things with cmsg).
	 *
	 * This also deals with an older issue caused by dumping cloneflags into
	 * clone(2): On old kernels, CLONE_PARENT didn't work with CLONE_NEWPID, so
	 * we have to unshare(2) before clone(2) in order to do this. This was fixed
	 * in upstream commit 1f7f4dde5c945f41a7abc2285be43d918029ecc5, and was
	 * introduced by 40a0d32d1eaffe6aac7324ca92604b6b3977eb0e. As far as we're
	 * aware, the last mainline kernel which had this bug was Linux 3.12.
	 * However, we cannot comment on which kernels the broken patch was
	 * backported to.
	 *
	 * -- Aleksa "what has my life come to?" Sarai
	 */

	switch (setjmp(env)) {
		/*
		 * Stage 0: We're in the parent. Our job is just to create a new child
		 *          (stage 1: JUMP_CHILD) process and write its uid_map and
		 *          gid_map. That process will go on to create a new process, then
		 *          it will send us its PID which we will send to the bootstrap
		 *          process.
		 */
	case JUMP_PARENT:{
			int len;
			pid_t child, first_child = -1;
			bool ready = false;

			/* For debugging. */
			prctl(PR_SET_NAME, (unsigned long)"runc:[0:PARENT]", 0, 0, 0);

			/* Start the process of getting a container. */
			child = clone_parent(&env, JUMP_CHILD);
			if (child < 0)
				bail("unable to fork: child_func");

			/*
			 * State machine for synchronisation with the children.
			 *
			 * Father only return when both child and grandchild are
			 * ready, so we can receive all possible error codes
			 * generated by children.
			 */
			while (!ready) {
				enum sync_t s;
				int ret;

				syncfd = sync_child_pipe[1];
				close(sync_child_pipe[0]);

				if (read(syncfd, &s, sizeof(s)) != sizeof(s))
					bail("failed to sync with child: next state");

				switch (s) {
				case SYNC_ERR:
					/* We have to mirror the error code of the child. */
					if (read(syncfd, &ret, sizeof(ret)) != sizeof(ret))
						bail("failed to sync with child: read(error code)");

					exit(ret);
				case SYNC_USERMAP_PLS:
					/*
					 * Enable setgroups(2) if we've been asked to. But we also
					 * have to explicitly disable setgroups(2) if we're
					 * creating a rootless container for single-entry mapping.
					 * i.e. config.is_setgroup == false.
					 * (this is required since Linux 3.19).
					 *
					 * For rootless multi-entry mapping, config.is_setgroup shall be true and
					 * newuidmap/newgidmap shall be used.
					 */

					if (config.is_rootless_euid && !config.is_setgroup)
						update_setgroups(child, SETGROUPS_DENY);

					/* Set up mappings. */
					update_uidmap(config.uidmappath, child, config.uidmap, config.uidmap_len);
					update_gidmap(config.gidmappath, child, config.gidmap, config.gidmap_len);

					s = SYNC_USERMAP_ACK;
					if (write(syncfd, &s, sizeof(s)) != sizeof(s)) {
						kill(child, SIGKILL);
						bail("failed to sync with child: write(SYNC_USERMAP_ACK)");
					}
					break;
				case SYNC_RECVPID_PLS:{
						first_child = child;

						/* Get the init_func pid. */
						if (read(syncfd, &child, sizeof(child)) != sizeof(child)) {
							kill(first_child, SIGKILL);
							bail("failed to sync with child: read(childpid)");
						}

						/* Send ACK. */
						s = SYNC_RECVPID_ACK;
						if (write(syncfd, &s, sizeof(s)) != sizeof(s)) {
							kill(first_child, SIGKILL);
							kill(child, SIGKILL);
							bail("failed to sync with child: write(SYNC_RECVPID_ACK)");
						}

						/* Send the init_func pid back to our parent.
						 *
						 * Send the init_func pid and the pid of the first child back to our parent.
						 * We need to send both back because we can't reap the first child we created (CLONE_PARENT).
						 * It becomes the responsibility of our parent to reap the first child.
						 */
						len = dprintf(pipenum, "{\"pid\": %d, \"pid_first\": %d}\n", child, first_child);
						if (len < 0) {
							kill(child, SIGKILL);
							bail("unable to generate JSON for child pid");
						}
					}
					break;
				case SYNC_CHILD_READY:
					ready = true;
					break;
				default:
					bail("unexpected sync value: %u", s);
				}
			}

			/* Now sync with grandchild. */

			ready = false;
			while (!ready) {
				enum sync_t s;
				int ret;

				syncfd = sync_grandchild_pipe[1];
				close(sync_grandchild_pipe[0]);

				s = SYNC_GRANDCHILD;
				if (write(syncfd, &s, sizeof(s)) != sizeof(s)) {
					kill(child, SIGKILL);
					bail("failed to sync with child: write(SYNC_GRANDCHILD)");
				}

				if (read(syncfd, &s, sizeof(s)) != sizeof(s))
					bail("failed to sync with child: next state");

				switch (s) {
				case SYNC_ERR:
					/* We have to mirror the error code of the child. */
					if (read(syncfd, &ret, sizeof(ret)) != sizeof(ret))
						bail("failed to sync with child: read(error code)");

					exit(ret);
				case SYNC_CHILD_READY:
					ready = true;
					break;
				default:
					bail("unexpected sync value: %u", s);
				}
			}
			exit(0);
		}

		/*
		 * Stage 1: We're in the first child process. Our job is to join any
		 *          provided namespaces in the netlink payload and unshare all
		 *          of the requested namespaces. If we've been asked to
		 *          CLONE_NEWUSER, we will ask our parent (stage 0) to set up
		 *          our user mappings for us. Then, we create a new child
		 *          (stage 2: JUMP_INIT) for PID namespace. We then send the
		 *          child's PID to our parent (stage 0).
		 */
	case JUMP_CHILD:{
			pid_t child;
			enum sync_t s;

			/* We're in a child and thus need to tell the parent if we die. */
			syncfd = sync_child_pipe[0];
			close(sync_child_pipe[1]);

			/* For debugging. */
			prctl(PR_SET_NAME, (unsigned long)"runc:[1:CHILD]", 0, 0, 0);

			/*
			 * We need to setns first. We cannot do this earlier (in stage 0)
			 * because of the fact that we forked to get here (the PID of
			 * [stage 2: JUMP_INIT]) would be meaningless). We could send it
			 * using cmsg(3) but that's just annoying.
			 */
			if (config.namespaces)
				join_namespaces(config.namespaces);

			/*
			 * Deal with user namespaces first. They are quite special, as they
			 * affect our ability to unshare other namespaces and are used as
			 * context for privilege checks.
			 *
			 * We don't unshare all namespaces in one go. The reason for this
			 * is that, while the kernel documentation may claim otherwise,
			 * there are certain cases where unsharing all namespaces at once
			 * will result in namespace objects being owned incorrectly.
			 * Ideally we should just fix these kernel bugs, but it's better to
			 * be safe than sorry, and fix them separately.
			 *
			 * A specific case of this is that the SELinux label of the
			 * internal kern-mount that mqueue uses will be incorrect if the
			 * UTS namespace is cloned before the USER namespace is mapped.
			 * I've also heard of similar problems with the network namespace
			 * in some scenarios. This also mirrors how LXC deals with this
			 * problem.
			 */
			if (config.cloneflags & CLONE_NEWUSER) {
				if (unshare(CLONE_NEWUSER) < 0)
					bail("failed to unshare user namespace");
				config.cloneflags &= ~CLONE_NEWUSER;

				/*
				 * We don't have the privileges to do any mapping here (see the
				 * clone_parent rant). So signal our parent to hook us up.
				 */

				/* Switching is only necessary if we joined namespaces. */
				if (config.namespaces) {
					if (prctl(PR_SET_DUMPABLE, 1, 0, 0, 0) < 0)
						bail("failed to set process as dumpable");
				}
				s = SYNC_USERMAP_PLS;
				if (write(syncfd, &s, sizeof(s)) != sizeof(s))
					bail("failed to sync with parent: write(SYNC_USERMAP_PLS)");

				/* ... wait for mapping ... */

				if (read(syncfd, &s, sizeof(s)) != sizeof(s))
					bail("failed to sync with parent: read(SYNC_USERMAP_ACK)");
				if (s != SYNC_USERMAP_ACK)
					bail("failed to sync with parent: SYNC_USERMAP_ACK: got %u", s);
				/* Switching is only necessary if we joined namespaces. */
				if (config.namespaces) {
					if (prctl(PR_SET_DUMPABLE, 0, 0, 0, 0) < 0)
						bail("failed to set process as dumpable");
				}

				/* Become root in the namespace proper. */
				if (setresuid(0, 0, 0) < 0)
					bail("failed to become root in user namespace");
			}
			/*
			 * Unshare all of the namespaces. Now, it should be noted that this
			 * ordering might break in the future (especially with rootless
			 * containers). But for now, it's not possible to split this into
			 * CLONE_NEWUSER + [the rest] because of some RHEL SELinux issues.
			 *
			 * Note that we don't merge this with clone() because there were
			 * some old kernel versions where clone(CLONE_PARENT | CLONE_NEWPID)
			 * was broken, so we'll just do it the long way anyway.
			 */
			if (unshare(config.cloneflags & ~CLONE_NEWCGROUP) < 0)
				bail("failed to unshare namespaces");

			/*
			 * TODO: What about non-namespace clone flags that we're dropping here?
			 *
			 * We fork again because of PID namespace, setns(2) or unshare(2) don't
			 * change the PID namespace of the calling process, because doing so
			 * would change the caller's idea of its own PID (as reported by getpid()),
			 * which would break many applications and libraries, so we must fork
			 * to actually enter the new PID namespace.
			 */
			child = clone_parent(&env, JUMP_INIT);
			if (child < 0)
				bail("unable to fork: init_func");

			/* Send the child to our parent, which knows what it's doing. */
			s = SYNC_RECVPID_PLS;
			if (write(syncfd, &s, sizeof(s)) != sizeof(s)) {
				kill(child, SIGKILL);
				bail("failed to sync with parent: write(SYNC_RECVPID_PLS)");
			}
			if (write(syncfd, &child, sizeof(child)) != sizeof(child)) {
				kill(child, SIGKILL);
				bail("failed to sync with parent: write(childpid)");
			}

			/* ... wait for parent to get the pid ... */

			if (read(syncfd, &s, sizeof(s)) != sizeof(s)) {
				kill(child, SIGKILL);
				bail("failed to sync with parent: read(SYNC_RECVPID_ACK)");
			}
			if (s != SYNC_RECVPID_ACK) {
				kill(child, SIGKILL);
				bail("failed to sync with parent: SYNC_RECVPID_ACK: got %u", s);
			}

			s = SYNC_CHILD_READY;
			if (write(syncfd, &s, sizeof(s)) != sizeof(s)) {
				kill(child, SIGKILL);
				bail("failed to sync with parent: write(SYNC_CHILD_READY)");
			}

			/* Our work is done. [Stage 2: JUMP_INIT] is doing the rest of the work. */
			exit(0);
		}

		/*
		 * Stage 2: We're the final child process, and the only process that will
		 *          actually return to the Go runtime. Our job is to just do the
		 *          final cleanup steps and then return to the Go runtime to allow
		 *          init_linux.go to run.
		 */
	case JUMP_INIT:{
			/*
			 * We're inside the child now, having jumped from the
			 * start_child() code after forking in the parent.
			 */
			enum sync_t s;

			/* We're in a child and thus need to tell the parent if we die. */
			syncfd = sync_grandchild_pipe[0];
			close(sync_grandchild_pipe[1]);
			close(sync_child_pipe[0]);
			close(sync_child_pipe[1]);

			/* For debugging. */
			prctl(PR_SET_NAME, (unsigned long)"runc:[2:INIT]", 0, 0, 0);

			if (read(syncfd, &s, sizeof(s)) != sizeof(s))
				bail("failed to sync with parent: read(SYNC_GRANDCHILD)");
			if (s != SYNC_GRANDCHILD)
				bail("failed to sync with parent: SYNC_GRANDCHILD: got %u", s);

			if (setsid() < 0)
				bail("setsid failed");

			if (setuid(0) < 0)
				bail("setuid failed");

			if (setgid(0) < 0)
				bail("setgid failed");

			if (!config.is_rootless_euid && config.is_setgroup) {
				if (setgroups(0, NULL) < 0)
					bail("setgroups failed");
			}

			/* ... wait until our topmost parent has finished cgroup setup in p.manager.Apply() ... */
			if (config.cloneflags & CLONE_NEWCGROUP) {
				uint8_t value;
				if (read(pipenum, &value, sizeof(value)) != sizeof(value))
					bail("read synchronisation value failed");
				if (value == CREATECGROUPNS) {
					if (unshare(CLONE_NEWCGROUP) < 0)
						bail("failed to unshare cgroup namespace");
				} else
					bail("received unknown synchronisation value");
			}

			s = SYNC_CHILD_READY;
			if (write(syncfd, &s, sizeof(s)) != sizeof(s))
				bail("failed to sync with patent: write(SYNC_CHILD_READY)");

			/* Close sync pipes. */
			close(sync_grandchild_pipe[0]);

			/* Free netlink data. */
			nl_free(&config);

			/* Finish executing, let the Go runtime take over. */
			return;
		}
	default:
		bail("unexpected jump value");
	}

	/* Should never be reached. */
	bail("should never be reached");
}