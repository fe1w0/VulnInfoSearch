int btrfs_init_dev_replace(struct btrfs_fs_info *fs_info)
{
	struct btrfs_key key;
	struct btrfs_root *dev_root = fs_info->dev_root;
	struct btrfs_dev_replace *dev_replace = &fs_info->dev_replace;
	struct extent_buffer *eb;
	int slot;
	int ret = 0;
	struct btrfs_path *path = NULL;
	int item_size;
	struct btrfs_dev_replace_item *ptr;
	u64 src_devid;

	path = btrfs_alloc_path();
	if (!path) {
		ret = -ENOMEM;
		goto out;
	}

	key.objectid = 0;
	key.type = BTRFS_DEV_REPLACE_KEY;
	key.offset = 0;
	ret = btrfs_search_slot(NULL, dev_root, &key, path, 0, 0);
	if (ret) {
no_valid_dev_replace_entry_found:
		ret = 0;
		dev_replace->replace_state =
			BTRFS_DEV_REPLACE_ITEM_STATE_NEVER_STARTED;
		dev_replace->cont_reading_from_srcdev_mode =
		    BTRFS_DEV_REPLACE_ITEM_CONT_READING_FROM_SRCDEV_MODE_ALWAYS;
		dev_replace->time_started = 0;
		dev_replace->time_stopped = 0;
		atomic64_set(&dev_replace->num_write_errors, 0);
		atomic64_set(&dev_replace->num_uncorrectable_read_errors, 0);
		dev_replace->cursor_left = 0;
		dev_replace->committed_cursor_left = 0;
		dev_replace->cursor_left_last_write_of_item = 0;
		dev_replace->cursor_right = 0;
		dev_replace->srcdev = NULL;
		dev_replace->tgtdev = NULL;
		dev_replace->is_valid = 0;
		dev_replace->item_needs_writeback = 0;
		goto out;
	}
	slot = path->slots[0];
	eb = path->nodes[0];
	item_size = btrfs_item_size_nr(eb, slot);
	ptr = btrfs_item_ptr(eb, slot, struct btrfs_dev_replace_item);

	if (item_size != sizeof(struct btrfs_dev_replace_item)) {
		btrfs_warn(fs_info,
			"dev_replace entry found has unexpected size, ignore entry");
		goto no_valid_dev_replace_entry_found;
	}

	src_devid = btrfs_dev_replace_src_devid(eb, ptr);
	dev_replace->cont_reading_from_srcdev_mode =
		btrfs_dev_replace_cont_reading_from_srcdev_mode(eb, ptr);
	dev_replace->replace_state = btrfs_dev_replace_replace_state(eb, ptr);
	dev_replace->time_started = btrfs_dev_replace_time_started(eb, ptr);
	dev_replace->time_stopped =
		btrfs_dev_replace_time_stopped(eb, ptr);
	atomic64_set(&dev_replace->num_write_errors,
		     btrfs_dev_replace_num_write_errors(eb, ptr));
	atomic64_set(&dev_replace->num_uncorrectable_read_errors,
		     btrfs_dev_replace_num_uncorrectable_read_errors(eb, ptr));
	dev_replace->cursor_left = btrfs_dev_replace_cursor_left(eb, ptr);
	dev_replace->committed_cursor_left = dev_replace->cursor_left;
	dev_replace->cursor_left_last_write_of_item = dev_replace->cursor_left;
	dev_replace->cursor_right = btrfs_dev_replace_cursor_right(eb, ptr);
	dev_replace->is_valid = 1;

	dev_replace->item_needs_writeback = 0;
	switch (dev_replace->replace_state) {
	case BTRFS_IOCTL_DEV_REPLACE_STATE_NEVER_STARTED:
	case BTRFS_IOCTL_DEV_REPLACE_STATE_FINISHED:
	case BTRFS_IOCTL_DEV_REPLACE_STATE_CANCELED:
		dev_replace->srcdev = NULL;
		dev_replace->tgtdev = NULL;
		break;
	case BTRFS_IOCTL_DEV_REPLACE_STATE_STARTED:
	case BTRFS_IOCTL_DEV_REPLACE_STATE_SUSPENDED:
		dev_replace->srcdev = btrfs_find_device(fs_info->fs_devices,
						src_devid, NULL, NULL, true);
		dev_replace->tgtdev = btrfs_find_device(fs_info->fs_devices,
							BTRFS_DEV_REPLACE_DEVID,
							NULL, NULL, true);
		/*
		 * allow 'btrfs dev replace_cancel' if src/tgt device is
		 * missing
		 */
		if (!dev_replace->srcdev &&
		    !btrfs_test_opt(fs_info, DEGRADED)) {
			ret = -EIO;
			btrfs_warn(fs_info,
			   "cannot mount because device replace operation is ongoing and");
			btrfs_warn(fs_info,
			   "srcdev (devid %llu) is missing, need to run 'btrfs dev scan'?",
			   src_devid);
		}
		if (!dev_replace->tgtdev &&
		    !btrfs_test_opt(fs_info, DEGRADED)) {
			ret = -EIO;
			btrfs_warn(fs_info,
			   "cannot mount because device replace operation is ongoing and");
			btrfs_warn(fs_info,
			   "tgtdev (devid %llu) is missing, need to run 'btrfs dev scan'?",
				BTRFS_DEV_REPLACE_DEVID);
		}
		if (dev_replace->tgtdev) {
			if (dev_replace->srcdev) {
				dev_replace->tgtdev->total_bytes =
					dev_replace->srcdev->total_bytes;
				dev_replace->tgtdev->disk_total_bytes =
					dev_replace->srcdev->disk_total_bytes;
				dev_replace->tgtdev->commit_total_bytes =
					dev_replace->srcdev->commit_total_bytes;
				dev_replace->tgtdev->bytes_used =
					dev_replace->srcdev->bytes_used;
				dev_replace->tgtdev->commit_bytes_used =
					dev_replace->srcdev->commit_bytes_used;
			}
			set_bit(BTRFS_DEV_STATE_REPLACE_TGT,
				&dev_replace->tgtdev->dev_state);

			WARN_ON(fs_info->fs_devices->rw_devices == 0);
			dev_replace->tgtdev->io_width = fs_info->sectorsize;
			dev_replace->tgtdev->io_align = fs_info->sectorsize;
			dev_replace->tgtdev->sector_size = fs_info->sectorsize;
			dev_replace->tgtdev->fs_info = fs_info;
			set_bit(BTRFS_DEV_STATE_IN_FS_METADATA,
				&dev_replace->tgtdev->dev_state);
		}
		break;
	}

out:
	btrfs_free_path(path);
	return ret;
}