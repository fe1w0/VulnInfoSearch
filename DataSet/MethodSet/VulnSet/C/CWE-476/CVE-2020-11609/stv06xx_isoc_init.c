static int stv06xx_isoc_init(struct gspca_dev *gspca_dev)
{
	struct usb_host_interface *alt;
	struct sd *sd = (struct sd *) gspca_dev;

	/* Start isoc bandwidth "negotiation" at max isoc bandwidth */
	alt = &gspca_dev->dev->actconfig->intf_cache[0]->altsetting[1];
	alt->endpoint[0].desc.wMaxPacketSize =
		cpu_to_le16(sd->sensor->max_packet_size[gspca_dev->curr_mode]);

	return 0;
}