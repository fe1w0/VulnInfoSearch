static int stv06xx_isoc_init(struct gspca_dev *gspca_dev)
{
	struct usb_interface_cache *intfc;
	struct usb_host_interface *alt;
	struct sd *sd = (struct sd *) gspca_dev;

	intfc = gspca_dev->dev->actconfig->intf_cache[0];

	if (intfc->num_altsetting < 2)
		return -ENODEV;

	alt = &intfc->altsetting[1];

	if (alt->desc.bNumEndpoints < 1)
		return -ENODEV;

	/* Start isoc bandwidth "negotiation" at max isoc bandwidth */
	alt->endpoint[0].desc.wMaxPacketSize =
		cpu_to_le16(sd->sensor->max_packet_size[gspca_dev->curr_mode]);

	return 0;
}