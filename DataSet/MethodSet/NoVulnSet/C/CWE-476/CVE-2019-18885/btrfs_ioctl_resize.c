static noinline int btrfs_ioctl_resize(struct file *file,
					void __user *arg)
{
	struct inode *inode = file_inode(file);
	struct btrfs_fs_info *fs_info = btrfs_sb(inode->i_sb);
	u64 new_size;
	u64 old_size;
	u64 devid = 1;
	struct btrfs_root *root = BTRFS_I(inode)->root;
	struct btrfs_ioctl_vol_args *vol_args;
	struct btrfs_trans_handle *trans;
	struct btrfs_device *device = NULL;
	char *sizestr;
	char *retptr;
	char *devstr = NULL;
	int ret = 0;
	int mod = 0;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	ret = mnt_want_write_file(file);
	if (ret)
		return ret;

	if (test_and_set_bit(BTRFS_FS_EXCL_OP, &fs_info->flags)) {
		mnt_drop_write_file(file);
		return BTRFS_ERROR_DEV_EXCL_RUN_IN_PROGRESS;
	}

	vol_args = memdup_user(arg, sizeof(*vol_args));
	if (IS_ERR(vol_args)) {
		ret = PTR_ERR(vol_args);
		goto out;
	}

	vol_args->name[BTRFS_PATH_NAME_MAX] = '\0';

	sizestr = vol_args->name;
	devstr = strchr(sizestr, ':');
	if (devstr) {
		sizestr = devstr + 1;
		*devstr = '\0';
		devstr = vol_args->name;
		ret = kstrtoull(devstr, 10, &devid);
		if (ret)
			goto out_free;
		if (!devid) {
			ret = -EINVAL;
			goto out_free;
		}
		btrfs_info(fs_info, "resizing devid %llu", devid);
	}

	device = btrfs_find_device(fs_info->fs_devices, devid, NULL, NULL, true);
	if (!device) {
		btrfs_info(fs_info, "resizer unable to find device %llu",
			   devid);
		ret = -ENODEV;
		goto out_free;
	}

	if (!test_bit(BTRFS_DEV_STATE_WRITEABLE, &device->dev_state)) {
		btrfs_info(fs_info,
			   "resizer unable to apply on readonly device %llu",
		       devid);
		ret = -EPERM;
		goto out_free;
	}

	if (!strcmp(sizestr, "max"))
		new_size = device->bdev->bd_inode->i_size;
	else {
		if (sizestr[0] == '-') {
			mod = -1;
			sizestr++;
		} else if (sizestr[0] == '+') {
			mod = 1;
			sizestr++;
		}
		new_size = memparse(sizestr, &retptr);
		if (*retptr != '\0' || new_size == 0) {
			ret = -EINVAL;
			goto out_free;
		}
	}

	if (test_bit(BTRFS_DEV_STATE_REPLACE_TGT, &device->dev_state)) {
		ret = -EPERM;
		goto out_free;
	}

	old_size = btrfs_device_get_total_bytes(device);

	if (mod < 0) {
		if (new_size > old_size) {
			ret = -EINVAL;
			goto out_free;
		}
		new_size = old_size - new_size;
	} else if (mod > 0) {
		if (new_size > ULLONG_MAX - old_size) {
			ret = -ERANGE;
			goto out_free;
		}
		new_size = old_size + new_size;
	}

	if (new_size < SZ_256M) {
		ret = -EINVAL;
		goto out_free;
	}
	if (new_size > device->bdev->bd_inode->i_size) {
		ret = -EFBIG;
		goto out_free;
	}

	new_size = round_down(new_size, fs_info->sectorsize);

	btrfs_info_in_rcu(fs_info, "new size for %s is %llu",
			  rcu_str_deref(device->name), new_size);

	if (new_size > old_size) {
		trans = btrfs_start_transaction(root, 0);
		if (IS_ERR(trans)) {
			ret = PTR_ERR(trans);
			goto out_free;
		}
		ret = btrfs_grow_device(trans, device, new_size);
		btrfs_commit_transaction(trans);
	} else if (new_size < old_size) {
		ret = btrfs_shrink_device(device, new_size);
	} /* equal, nothing need to do */

out_free:
	kfree(vol_args);
out:
	clear_bit(BTRFS_FS_EXCL_OP, &fs_info->flags);
	mnt_drop_write_file(file);
	return ret;
}