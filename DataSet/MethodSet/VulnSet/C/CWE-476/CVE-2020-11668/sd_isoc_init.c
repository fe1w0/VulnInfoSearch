static int sd_isoc_init(struct gspca_dev *gspca_dev)
{
	struct usb_host_interface *alt;
	int max_packet_size;

	switch (gspca_dev->pixfmt.width) {
	case 160:
		max_packet_size = 450;
		break;
	case 176:
		max_packet_size = 600;
		break;
	default:
		max_packet_size = 1022;
		break;
	}

	/* Start isoc bandwidth "negotiation" at max isoc bandwidth */
	alt = &gspca_dev->dev->actconfig->intf_cache[0]->altsetting[1];
	alt->endpoint[0].desc.wMaxPacketSize = cpu_to_le16(max_packet_size);

	return 0;
}