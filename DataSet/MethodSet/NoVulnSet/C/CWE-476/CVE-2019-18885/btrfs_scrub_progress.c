int btrfs_scrub_progress(struct btrfs_fs_info *fs_info, u64 devid,
			 struct btrfs_scrub_progress *progress)
{
	struct btrfs_device *dev;
	struct scrub_ctx *sctx = NULL;

	mutex_lock(&fs_info->fs_devices->device_list_mutex);
	dev = btrfs_find_device(fs_info->fs_devices, devid, NULL, NULL, true);
	if (dev)
		sctx = dev->scrub_ctx;
	if (sctx)
		memcpy(progress, &sctx->stat, sizeof(*progress));
	mutex_unlock(&fs_info->fs_devices->device_list_mutex);

	return dev ? (sctx ? 0 : -ENOTCONN) : -ENODEV;
}