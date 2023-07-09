GF_Err ilst_item_box_read(GF_Box *s,GF_BitStream *bs)
{
	GF_Err e;
	u32 sub_type;
	GF_Box *a = NULL;
	GF_ListItemBox *ptr = (GF_ListItemBox *)s;
	/*iTunes way: there's a data atom containing the data*/
	sub_type = gf_bs_peek_bits(bs, 32, 4);
	if (sub_type == GF_ISOM_BOX_TYPE_DATA ) {
		e = gf_isom_box_parse(&a, bs);

		if (!e && ptr->size < a->size) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[isom] not enough bytes in box %s: %d left, reading %d (file %s, line %d)\n", gf_4cc_to_str(ptr->type), ptr->size, a->size, __FILE__, __LINE__ )); \
			e = GF_ISOM_INVALID_FILE;
		}
		if (e) {
			if (a) gf_isom_box_del(a);
			return e;
		}

		ISOM_DECREASE_SIZE(ptr, a->size);

		if (a && ptr->data) gf_isom_box_del_parent(&ptr->child_boxes, (GF_Box *) ptr->data);

		/* otherwise a->data will always overflow */
		if (a && a->size > 4 && a->type != GF_ISOM_BOX_TYPE_VOID) {
			ptr->data = (GF_DataBox *)a;
			if (!ptr->child_boxes) ptr->child_boxes = gf_list_new();
			gf_list_add(ptr->child_boxes, ptr->data);
		} else {
			ptr->data = NULL;
			gf_isom_box_del(a);
		}
	}
	/*QT way*/
	else {
		u64 pos = gf_bs_get_position(bs);
		u64 prev_size = s->size;
		/*try parsing as generic box list*/
		e = gf_isom_box_array_read(s, bs, NULL);
		if (e==GF_OK) return GF_OK;
		//reset content and retry - this deletes ptr->data !!
		gf_isom_box_array_del(s->child_boxes);
		s->child_boxes=NULL;
		gf_bs_seek(bs, pos);
		s->size = prev_size;

		ptr->data = (GF_DataBox *)gf_isom_box_new_parent(&ptr->child_boxes, GF_ISOM_BOX_TYPE_DATA);
		//nope, check qt-style
		ptr->data->qt_style = GF_TRUE;
		ISOM_DECREASE_SIZE(ptr, 2);
		ptr->data->dataSize = gf_bs_read_u16(bs);
		gf_bs_read_u16(bs);

		ptr->data->data = (char *) gf_malloc(sizeof(char)*(ptr->data->dataSize + 1));
		gf_bs_read_data(bs, ptr->data->data, ptr->data->dataSize);
		ptr->data->data[ptr->data->dataSize] = 0;
		ISOM_DECREASE_SIZE(ptr, ptr->data->dataSize);
	}
	return GF_OK;
}