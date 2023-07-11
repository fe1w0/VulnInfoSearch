static struct btrfs_device *find_device(struct btrfs_fs_devices *fs_devices,
		u64 devid, const u8 *uuid)
{
	struct btrfs_device *dev;

	list_for_each_entry(dev, &fs_devices->devices, dev_list) {
		if (dev->devid == devid &&
		    (!uuid || !memcmp(dev->uuid, uuid, BTRFS_UUID_SIZE))) {
			return dev;
		}
	}
	return NULL;
}