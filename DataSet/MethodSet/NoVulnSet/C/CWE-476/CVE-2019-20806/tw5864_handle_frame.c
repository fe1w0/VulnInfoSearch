static void tw5864_handle_frame(struct tw5864_h264_frame *frame)
{
#define SKIP_VLCBUF_BYTES 3
	struct tw5864_input *input = frame->input;
	struct tw5864_dev *dev = input->root;
	struct tw5864_buf *vb;
	struct vb2_v4l2_buffer *v4l2_buf;
	int frame_len = frame->vlc_len - SKIP_VLCBUF_BYTES;
	u8 *dst = input->buf_cur_ptr;
	u8 tail_mask, vlc_mask = 0;
	int i;
	u8 vlc_first_byte = ((u8 *)(frame->vlc.addr + SKIP_VLCBUF_BYTES))[0];
	unsigned long flags;
	int zero_run;
	u8 *src;
	u8 *src_end;

#ifdef DEBUG
	if (frame->checksum !=
	    tw5864_vlc_checksum((u32 *)frame->vlc.addr, frame_len))
		dev_err(&dev->pci->dev,
			"Checksum of encoded frame doesn't match!\n");
#endif

	spin_lock_irqsave(&input->slock, flags);
	vb = input->vb;
	input->vb = NULL;
	spin_unlock_irqrestore(&input->slock, flags);

	if (!vb) { /* Gone because of disabling */
		dev_dbg(&dev->pci->dev, "vb is empty, dropping frame\n");
		return;
	}

	v4l2_buf = to_vb2_v4l2_buffer(&vb->vb.vb2_buf);

	/*
	 * Check for space.
	 * Mind the overhead of startcode emulation prevention.
	 */
	if (input->buf_cur_space_left < frame_len * 5 / 4) {
		dev_err_once(&dev->pci->dev,
			     "Left space in vb2 buffer, %d bytes, is less than considered safely enough to put frame of length %d. Dropping this frame.\n",
			     input->buf_cur_space_left, frame_len);
		return;
	}

	for (i = 0; i < 8 - input->tail_nb_bits; i++)
		vlc_mask |= 1 << i;
	tail_mask = (~vlc_mask) & 0xff;

	dst[0] = (input->tail & tail_mask) | (vlc_first_byte & vlc_mask);
	frame_len--;
	dst++;

	/* H.264 startcode emulation prevention */
	src = frame->vlc.addr + SKIP_VLCBUF_BYTES + 1;
	src_end = src + frame_len;
	zero_run = 0;
	for (; src < src_end; src++) {
		if (zero_run < 2) {
			if (*src == 0)
				++zero_run;
			else
				zero_run = 0;
		} else {
			if ((*src & ~0x03) == 0)
				*dst++ = 0x03;
			zero_run = *src == 0;
		}
		*dst++ = *src;
	}

	vb2_set_plane_payload(&vb->vb.vb2_buf, 0,
			      dst - (u8 *)vb2_plane_vaddr(&vb->vb.vb2_buf, 0));

	vb->vb.vb2_buf.timestamp = frame->timestamp;
	v4l2_buf->field = V4L2_FIELD_INTERLACED;
	v4l2_buf->sequence = frame->seqno;

	/* Check for motion flags */
	if (frame->gop_seqno /* P-frame */ &&
	    tw5864_is_motion_triggered(frame)) {
		struct v4l2_event ev = {
			.type = V4L2_EVENT_MOTION_DET,
			.u.motion_det = {
				.flags = V4L2_EVENT_MD_FL_HAVE_FRAME_SEQ,
				.frame_sequence = v4l2_buf->sequence,
			},
		};

		v4l2_event_queue(&input->vdev, &ev);
	}

	vb2_buffer_done(&vb->vb.vb2_buf, VB2_BUF_STATE_DONE);
}