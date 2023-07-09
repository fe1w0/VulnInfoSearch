static int pb0100_start(struct sd *sd)
{
	int err, packet_size, max_packet_size;
	struct usb_host_interface *alt;
	struct usb_interface *intf;
	struct gspca_dev *gspca_dev = (struct gspca_dev *)sd;
	struct cam *cam = &sd->gspca_dev.cam;
	u32 mode = cam->cam_mode[sd->gspca_dev.curr_mode].priv;

	intf = usb_ifnum_to_if(sd->gspca_dev.dev, sd->gspca_dev.iface);
	alt = usb_altnum_to_altsetting(intf, sd->gspca_dev.alt);
	if (!alt)
		return -ENODEV;
	packet_size = le16_to_cpu(alt->endpoint[0].desc.wMaxPacketSize);

	/* If we don't have enough bandwidth use a lower framerate */
	max_packet_size = sd->sensor->max_packet_size[sd->gspca_dev.curr_mode];
	if (packet_size < max_packet_size)
		stv06xx_write_sensor(sd, PB_ROWSPEED, BIT(4)|BIT(3)|BIT(1));
	else
		stv06xx_write_sensor(sd, PB_ROWSPEED, BIT(5)|BIT(3)|BIT(1));

	/* Setup sensor window */
	if (mode & PB0100_CROP_TO_VGA) {
		stv06xx_write_sensor(sd, PB_RSTART, 30);
		stv06xx_write_sensor(sd, PB_CSTART, 20);
		stv06xx_write_sensor(sd, PB_RWSIZE, 240 - 1);
		stv06xx_write_sensor(sd, PB_CWSIZE, 320 - 1);
	} else {
		stv06xx_write_sensor(sd, PB_RSTART, 8);
		stv06xx_write_sensor(sd, PB_CSTART, 4);
		stv06xx_write_sensor(sd, PB_RWSIZE, 288 - 1);
		stv06xx_write_sensor(sd, PB_CWSIZE, 352 - 1);
	}

	if (mode & PB0100_SUBSAMPLE) {
		stv06xx_write_bridge(sd, STV_Y_CTRL, 0x02); /* Wrong, FIXME */
		stv06xx_write_bridge(sd, STV_X_CTRL, 0x06);

		stv06xx_write_bridge(sd, STV_SCAN_RATE, 0x10);
	} else {
		stv06xx_write_bridge(sd, STV_Y_CTRL, 0x01);
		stv06xx_write_bridge(sd, STV_X_CTRL, 0x0a);
		/* larger -> slower */
		stv06xx_write_bridge(sd, STV_SCAN_RATE, 0x20);
	}

	err = stv06xx_write_sensor(sd, PB_CONTROL, BIT(5)|BIT(3)|BIT(1));
	gspca_dbg(gspca_dev, D_STREAM, "Started stream, status: %d\n", err);

	return (err < 0) ? err : 0;
}