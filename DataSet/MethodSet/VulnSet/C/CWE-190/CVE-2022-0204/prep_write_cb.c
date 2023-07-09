static void prep_write_cb(struct bt_att_chan *chan, uint8_t opcode,
					const void *pdu, uint16_t length,
					void *user_data)
{
	struct bt_gatt_server *server = user_data;
	uint16_t handle = 0;
	uint16_t offset;
	struct gatt_db_attribute *attr;
	struct prep_write_complete_data *pwcd;
	uint8_t ecode, status;

	if (length < 4) {
		ecode = BT_ATT_ERROR_INVALID_PDU;
		goto error;
	}

	if (queue_length(server->prep_queue) >= server->max_prep_queue_len) {
		ecode = BT_ATT_ERROR_PREPARE_QUEUE_FULL;
		goto error;
	}

	handle = get_le16(pdu);
	offset = get_le16(pdu + 2);

	attr = gatt_db_get_attribute(server->db, handle);
	if (!attr) {
		ecode = BT_ATT_ERROR_INVALID_HANDLE;
		goto error;
	}

	util_debug(server->debug_callback, server->debug_data,
				"Prep Write Req - handle: 0x%04x", handle);

	ecode = check_permissions(server, attr, BT_ATT_PERM_WRITE_MASK);
	if (ecode)
		goto error;

	pwcd = new0(struct prep_write_complete_data, 1);
	pwcd->chan = chan;
	pwcd->pdu = malloc(length);
	memcpy(pwcd->pdu, pdu, length);
	pwcd->length = length;
	pwcd->server = server;

	status = gatt_db_attribute_write(attr, offset, NULL, 0,
						BT_ATT_OP_PREP_WRITE_REQ,
						server->att,
						prep_write_complete_cb, pwcd);

	if (status)
		return;

	ecode = BT_ATT_ERROR_UNLIKELY;

error:
	bt_att_chan_send_error_rsp(chan, opcode, handle, ecode);
}