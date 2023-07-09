static int cit_get_packet_size(struct gspca_dev *gspca_dev)
{
	struct usb_host_interface *alt;
	struct usb_interface *intf;

	intf = usb_ifnum_to_if(gspca_dev->dev, gspca_dev->iface);
	alt = usb_altnum_to_altsetting(intf, gspca_dev->alt);
	if (!alt) {
		pr_err("Couldn't get altsetting\n");
		return -EIO;
	}

	return le16_to_cpu(alt->endpoint[0].desc.wMaxPacketSize);
}