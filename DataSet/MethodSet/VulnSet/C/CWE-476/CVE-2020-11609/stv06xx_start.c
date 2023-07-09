static int stv06xx_start(struct gspca_dev *gspca_dev)
{
	struct sd *sd = (struct sd *) gspca_dev;
	struct usb_host_interface *alt;
	struct usb_interface *intf;
	int err, packet_size;

	intf = usb_ifnum_to_if(sd->gspca_dev.dev, sd->gspca_dev.iface);
	alt = usb_altnum_to_altsetting(intf, sd->gspca_dev.alt);
	if (!alt) {
		gspca_err(gspca_dev, "Couldn't get altsetting\n");
		return -EIO;
	}

	packet_size = le16_to_cpu(alt->endpoint[0].desc.wMaxPacketSize);
	err = stv06xx_write_bridge(sd, STV_ISO_SIZE_L, packet_size);
	if (err < 0)
		return err;

	/* Prepare the sensor for start */
	err = sd->sensor->start(sd);
	if (err < 0)
		goto out;

	/* Start isochronous streaming */
	err = stv06xx_write_bridge(sd, STV_ISO_ENABLE, 1);

out:
	if (err < 0)
		gspca_dbg(gspca_dev, D_STREAM, "Starting stream failed\n");
	else
		gspca_dbg(gspca_dev, D_STREAM, "Started streaming\n");

	return (err < 0) ? err : 0;
}