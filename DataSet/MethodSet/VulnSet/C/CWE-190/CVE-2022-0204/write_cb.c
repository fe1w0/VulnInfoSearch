static void write_cb(struct bt_att_chan *chan, uint8_t opcode, const void *pdu,
					uint16_t length, void *user_data)
{
	struct bt_gatt_server *server = user_data;
	struct gatt_db_attribute *attr;
	uint16_t handle = 0;
	struct async_write_op *op = NULL;
	uint8_t ecode;

	if (length < 2) {
		ecode = BT_ATT_ERROR_INVALID_PDU;
		goto error;
	}

	ecode = authorize_req(server, opcode, handle);
	if (ecode)
		goto error;

	handle = get_le16(pdu);
	attr = gatt_db_get_attribute(server->db, handle);
	if (!attr) {
		ecode = BT_ATT_ERROR_INVALID_HANDLE;
		goto error;
	}

	util_debug(server->debug_callback, server->debug_data,
				"Write %s - handle: 0x%04x",
				(opcode == BT_ATT_OP_WRITE_REQ) ? "Req" : "Cmd",
				handle);

	ecode = check_permissions(server, attr, BT_ATT_PERM_WRITE_MASK);
	if (ecode)
		goto error;

	op = new0(struct async_write_op, 1);
	op->chan = chan;
	op->server = bt_gatt_server_ref(server);
	op->opcode = opcode;

	if (gatt_db_attribute_write(attr, 0, pdu + 2, length - 2, opcode,
							server->att,
							write_complete_cb, op))
		return;

	async_write_op_destroy(op);

	ecode = BT_ATT_ERROR_UNLIKELY;

error:
	if (opcode == BT_ATT_OP_WRITE_CMD)
		return;

	bt_att_chan_send_error_rsp(chan, opcode, handle, ecode);
}