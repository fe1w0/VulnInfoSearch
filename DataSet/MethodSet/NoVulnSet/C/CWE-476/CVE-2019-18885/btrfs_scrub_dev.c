int btrfs_scrub_dev(struct btrfs_fs_info *fs_info, u64 devid, u64 start,
		    u64 end, struct btrfs_scrub_progress *progress,
		    int readonly, int is_dev_replace)
{
	struct scrub_ctx *sctx;
	int ret;
	struct btrfs_device *dev;
	unsigned int nofs_flag;

	if (btrfs_fs_closing(fs_info))
		return -EINVAL;

	if (fs_info->nodesize > BTRFS_STRIPE_LEN) {
		/*
		 * in this case scrub is unable to calculate the checksum
		 * the way scrub is implemented. Do not handle this
		 * situation at all because it won't ever happen.
		 */
		btrfs_err(fs_info,
			   "scrub: size assumption nodesize <= BTRFS_STRIPE_LEN (%d <= %d) fails",
		       fs_info->nodesize,
		       BTRFS_STRIPE_LEN);
		return -EINVAL;
	}

	if (fs_info->sectorsize != PAGE_SIZE) {
		/* not supported for data w/o checksums */
		btrfs_err_rl(fs_info,
			   "scrub: size assumption sectorsize != PAGE_SIZE (%d != %lu) fails",
		       fs_info->sectorsize, PAGE_SIZE);
		return -EINVAL;
	}

	if (fs_info->nodesize >
	    PAGE_SIZE * SCRUB_MAX_PAGES_PER_BLOCK ||
	    fs_info->sectorsize > PAGE_SIZE * SCRUB_MAX_PAGES_PER_BLOCK) {
		/*
		 * would exhaust the array bounds of pagev member in
		 * struct scrub_block
		 */
		btrfs_err(fs_info,
			  "scrub: size assumption nodesize and sectorsize <= SCRUB_MAX_PAGES_PER_BLOCK (%d <= %d && %d <= %d) fails",
		       fs_info->nodesize,
		       SCRUB_MAX_PAGES_PER_BLOCK,
		       fs_info->sectorsize,
		       SCRUB_MAX_PAGES_PER_BLOCK);
		return -EINVAL;
	}

	/* Allocate outside of device_list_mutex */
	sctx = scrub_setup_ctx(fs_info, is_dev_replace);
	if (IS_ERR(sctx))
		return PTR_ERR(sctx);

	mutex_lock(&fs_info->fs_devices->device_list_mutex);
	dev = btrfs_find_device(fs_info->fs_devices, devid, NULL, NULL, true);
	if (!dev || (test_bit(BTRFS_DEV_STATE_MISSING, &dev->dev_state) &&
		     !is_dev_replace)) {
		mutex_unlock(&fs_info->fs_devices->device_list_mutex);
		ret = -ENODEV;
		goto out_free_ctx;
	}

	if (!is_dev_replace && !readonly &&
	    !test_bit(BTRFS_DEV_STATE_WRITEABLE, &dev->dev_state)) {
		mutex_unlock(&fs_info->fs_devices->device_list_mutex);
		btrfs_err_in_rcu(fs_info, "scrub: device %s is not writable",
				rcu_str_deref(dev->name));
		ret = -EROFS;
		goto out_free_ctx;
	}

	mutex_lock(&fs_info->scrub_lock);
	if (!test_bit(BTRFS_DEV_STATE_IN_FS_METADATA, &dev->dev_state) ||
	    test_bit(BTRFS_DEV_STATE_REPLACE_TGT, &dev->dev_state)) {
		mutex_unlock(&fs_info->scrub_lock);
		mutex_unlock(&fs_info->fs_devices->device_list_mutex);
		ret = -EIO;
		goto out_free_ctx;
	}

	down_read(&fs_info->dev_replace.rwsem);
	if (dev->scrub_ctx ||
	    (!is_dev_replace &&
	     btrfs_dev_replace_is_ongoing(&fs_info->dev_replace))) {
		up_read(&fs_info->dev_replace.rwsem);
		mutex_unlock(&fs_info->scrub_lock);
		mutex_unlock(&fs_info->fs_devices->device_list_mutex);
		ret = -EINPROGRESS;
		goto out_free_ctx;
	}
	up_read(&fs_info->dev_replace.rwsem);

	ret = scrub_workers_get(fs_info, is_dev_replace);
	if (ret) {
		mutex_unlock(&fs_info->scrub_lock);
		mutex_unlock(&fs_info->fs_devices->device_list_mutex);
		goto out_free_ctx;
	}

	sctx->readonly = readonly;
	dev->scrub_ctx = sctx;
	mutex_unlock(&fs_info->fs_devices->device_list_mutex);

	/*
	 * checking @scrub_pause_req here, we can avoid
	 * race between committing transaction and scrubbing.
	 */
	__scrub_blocked_if_needed(fs_info);
	atomic_inc(&fs_info->scrubs_running);
	mutex_unlock(&fs_info->scrub_lock);

	/*
	 * In order to avoid deadlock with reclaim when there is a transaction
	 * trying to pause scrub, make sure we use GFP_NOFS for all the
	 * allocations done at btrfs_scrub_pages() and scrub_pages_for_parity()
	 * invoked by our callees. The pausing request is done when the
	 * transaction commit starts, and it blocks the transaction until scrub
	 * is paused (done at specific points at scrub_stripe() or right above
	 * before incrementing fs_info->scrubs_running).
	 */
	nofs_flag = memalloc_nofs_save();
	if (!is_dev_replace) {
		/*
		 * by holding device list mutex, we can
		 * kick off writing super in log tree sync.
		 */
		mutex_lock(&fs_info->fs_devices->device_list_mutex);
		ret = scrub_supers(sctx, dev);
		mutex_unlock(&fs_info->fs_devices->device_list_mutex);
	}

	if (!ret)
		ret = scrub_enumerate_chunks(sctx, dev, start, end);
	memalloc_nofs_restore(nofs_flag);

	wait_event(sctx->list_wait, atomic_read(&sctx->bios_in_flight) == 0);
	atomic_dec(&fs_info->scrubs_running);
	wake_up(&fs_info->scrub_pause_wait);

	wait_event(sctx->list_wait, atomic_read(&sctx->workers_pending) == 0);

	if (progress)
		memcpy(progress, &sctx->stat, sizeof(*progress));

	mutex_lock(&fs_info->scrub_lock);
	dev->scrub_ctx = NULL;
	scrub_workers_put(fs_info);
	mutex_unlock(&fs_info->scrub_lock);

	scrub_put_ctx(sctx);

	return ret;

out_free_ctx:
	scrub_free_ctx(sctx);

	return ret;
}