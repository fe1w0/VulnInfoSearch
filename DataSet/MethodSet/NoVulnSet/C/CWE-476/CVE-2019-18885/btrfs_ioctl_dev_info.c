static long btrfs_ioctl_dev_info(struct btrfs_fs_info *fs_info,
				 void __user *arg)
{
	struct btrfs_ioctl_dev_info_args *di_args;
	struct btrfs_device *dev;
	int ret = 0;
	char *s_uuid = NULL;

	di_args = memdup_user(arg, sizeof(*di_args));
	if (IS_ERR(di_args))
		return PTR_ERR(di_args);

	if (!btrfs_is_empty_uuid(di_args->uuid))
		s_uuid = di_args->uuid;

	rcu_read_lock();
	dev = btrfs_find_device(fs_info->fs_devices, di_args->devid, s_uuid,
				NULL, true);

	if (!dev) {
		ret = -ENODEV;
		goto out;
	}

	di_args->devid = dev->devid;
	di_args->bytes_used = btrfs_device_get_bytes_used(dev);
	di_args->total_bytes = btrfs_device_get_total_bytes(dev);
	memcpy(di_args->uuid, dev->uuid, sizeof(di_args->uuid));
	if (dev->name) {
		strncpy(di_args->path, rcu_str_deref(dev->name),
				sizeof(di_args->path) - 1);
		di_args->path[sizeof(di_args->path) - 1] = 0;
	} else {
		di_args->path[0] = '\0';
	}

out:
	rcu_read_unlock();
	if (ret == 0 && copy_to_user(arg, di_args, sizeof(*di_args)))
		ret = -EFAULT;

	kfree(di_args);
	return ret;
}