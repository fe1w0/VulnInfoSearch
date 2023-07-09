static int sd_isoc_nego(struct gspca_dev *gspca_dev)
{
	int ret, packet_size, min_packet_size;
	struct usb_host_interface *alt;

	switch (gspca_dev->pixfmt.width) {
	case 160:
		min_packet_size = 200;
		break;
	case 176:
		min_packet_size = 266;
		break;
	default:
		min_packet_size = 400;
		break;
	}

	alt = &gspca_dev->dev->actconfig->intf_cache[0]->altsetting[1];
	packet_size = le16_to_cpu(alt->endpoint[0].desc.wMaxPacketSize);
	if (packet_size <= min_packet_size)
		return -EIO;

	packet_size -= 100;
	if (packet_size < min_packet_size)
		packet_size = min_packet_size;
	alt->endpoint[0].desc.wMaxPacketSize = cpu_to_le16(packet_size);

	ret = usb_set_interface(gspca_dev->dev, gspca_dev->iface, 1);
	if (ret < 0)
		pr_err("set alt 1 err %d\n", ret);

	return ret;
}