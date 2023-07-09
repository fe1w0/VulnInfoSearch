GF_Err gf_bifs_dec_sf_field(GF_BifsDecoder * codec, GF_BitStream *bs, GF_Node *node, GF_FieldInfo *field, Bool is_mem_com)
{
	GF_Err e;
	GF_Node *new_node;
	u32 size, length, w, h, i;
	char *buffer;

	//blindly call unquantize. return is OK, error or GF_EOS
	if (codec->ActiveQP && node) {
		e = gf_bifs_dec_unquant_field(codec, bs, node, field);
		if (e != GF_EOS) return e;
	}
	//not quantized, use normal scheme
	switch (field->fieldType) {
	case GF_SG_VRML_SFBOOL:
		* ((SFBool *) field->far_ptr) = (SFBool) gf_bs_read_int(bs, 1);
		break;
	case GF_SG_VRML_SFCOLOR:
		((SFColor *)field->far_ptr)->red = BD_ReadSFFloat(codec, bs);
		((SFColor *)field->far_ptr)->green = BD_ReadSFFloat(codec, bs);
		((SFColor *)field->far_ptr)->blue = BD_ReadSFFloat(codec, bs);
		break;
	case GF_SG_VRML_SFFLOAT:
		*((SFFloat *)field->far_ptr) = BD_ReadSFFloat(codec, bs);
		break;
	case GF_SG_VRML_SFINT32:
		*((SFInt32 *)field->far_ptr) = (s32) gf_bs_read_int(bs, 32);
		break;
	case GF_SG_VRML_SFTIME:
		*((SFTime *)field->far_ptr) = gf_bs_read_double(bs);
		if (node) BD_CheckSFTimeOffset(codec, node, field);
		break;
	case GF_SG_VRML_SFVEC2F:
		((SFVec2f *)field->far_ptr)->x = BD_ReadSFFloat(codec, bs);
		((SFVec2f *)field->far_ptr)->y = BD_ReadSFFloat(codec, bs);
		break;
	case GF_SG_VRML_SFVEC3F:
		((SFVec3f *)field->far_ptr)->x = BD_ReadSFFloat(codec, bs);
		((SFVec3f *)field->far_ptr)->y = BD_ReadSFFloat(codec, bs);
		((SFVec3f *)field->far_ptr)->z = BD_ReadSFFloat(codec, bs);
		break;
	case GF_SG_VRML_SFROTATION:
		((SFRotation *)field->far_ptr)->x = BD_ReadSFFloat(codec, bs);
		((SFRotation *)field->far_ptr)->y = BD_ReadSFFloat(codec, bs);
		((SFRotation *)field->far_ptr)->z = BD_ReadSFFloat(codec, bs);
		((SFRotation *)field->far_ptr)->q = BD_ReadSFFloat(codec, bs);
		break;
	case GF_SG_VRML_SFSTRING:
		size = gf_bs_read_int(bs, 5);
		length = gf_bs_read_int(bs, size);
		if (gf_bs_available(bs) < length) return GF_NON_COMPLIANT_BITSTREAM;

		if (node && (node->sgprivate->tag==TAG_MPEG4_CacheTexture) && (field->fieldIndex<=2)) {
			M_CacheTexture *ct = (M_CacheTexture *) node;
			ct->data_len = length;
			if (ct->data) gf_free(ct->data);
			ct->data = (u8*)gf_malloc(sizeof(char)*length);
			gf_bs_read_data(bs, (char*)ct->data, length);
		} else if (node && (node->sgprivate->tag==TAG_MPEG4_BitWrapper) ) {
			M_BitWrapper *bw = (M_BitWrapper*) node;
			if (bw->buffer.buffer) gf_free(bw->buffer.buffer);
			bw->buffer_len = length;
			bw->buffer.buffer = (char*)gf_malloc(sizeof(char)*length);
			gf_bs_read_data(bs, (char*)bw->buffer.buffer, length);
		} else {
			if ( ((SFString *)field->far_ptr)->buffer ) gf_free( ((SFString *)field->far_ptr)->buffer);
			((SFString *)field->far_ptr)->buffer = (char *)gf_malloc(sizeof(char)*(length+1));
			memset(((SFString *)field->far_ptr)->buffer , 0, length+1);
			for (i=0; i<length; i++) {
				((SFString *)field->far_ptr)->buffer[i] = gf_bs_read_int(bs, 8);
			}
		}
		break;
	case GF_SG_VRML_SFURL:
	{
		SFURL *url = (SFURL *) field->far_ptr;
		size = gf_bs_read_int(bs, 1);
		if (size) {
			if (url->url) gf_free(url->url );
			url->url = NULL;
			length = gf_bs_read_int(bs, 10);
			url->OD_ID = length;
		} else {
			if ( url->OD_ID ) url->OD_ID = (u32) -1;
			size = gf_bs_read_int(bs, 5);
			length = gf_bs_read_int(bs, size);
			if (gf_bs_available(bs) < length) return GF_NON_COMPLIANT_BITSTREAM;
			buffer = NULL;
			if (length) {
				buffer = (char *)gf_malloc(sizeof(char)*(length+1));
				memset(buffer, 0, length+1);
				for (i=0; i<length; i++) buffer[i] = gf_bs_read_int(bs, 8);
			}
			if (url->url) gf_free( url->url);
			/*if URL is empty set it to NULL*/
			if (buffer && strlen(buffer)) {
				url->url = buffer;
			} else {
				gf_free(buffer);
				url->url = NULL;
			}
		}
	}
	break;
	case GF_SG_VRML_SFIMAGE:
		if (((SFImage *)field->far_ptr)->pixels) {
			gf_free(((SFImage *)field->far_ptr)->pixels);
			((SFImage *)field->far_ptr)->pixels = NULL;
		}
		w = gf_bs_read_int(bs, 12);
		h = gf_bs_read_int(bs, 12);
		length = gf_bs_read_int(bs, 2);

		if (length > 3) length = 3;
		length += 1;
		size = w * h * length;
		if (gf_bs_available(bs) < size) return GF_NON_COMPLIANT_BITSTREAM;
		((SFImage *)field->far_ptr)->width = w;
		((SFImage *)field->far_ptr)->height = h;
		((SFImage *)field->far_ptr)->numComponents = length;
		((SFImage *)field->far_ptr)->pixels = (unsigned char *)gf_malloc(sizeof(char)*size);
		//WARNING: Buffers are NOT ALIGNED IN THE BITSTREAM
		for (i=0; i<size; i++) {
			((SFImage *)field->far_ptr)->pixels[i] = gf_bs_read_int(bs, 8);
		}
		break;
	case GF_SG_VRML_SFCOMMANDBUFFER:
	{
		SFCommandBuffer *sfcb = (SFCommandBuffer *)field->far_ptr;
		if (!node) return GF_BAD_PARAM;
		if (sfcb->buffer) {
			gf_free(sfcb->buffer);
			sfcb->buffer = NULL;
		}
		while (gf_list_count(sfcb->commandList)) {
			GF_Command *com = (GF_Command*)gf_list_get(sfcb->commandList, 0);
			gf_list_rem(sfcb->commandList, 0);
			gf_sg_command_del(com);
		}

		size = gf_bs_read_int(bs, 5);
		length = gf_bs_read_int(bs, size);
		if (gf_bs_available(bs) < length) return GF_NON_COMPLIANT_BITSTREAM;

		sfcb->bufferSize = length;
		if (length) {
			sfcb->buffer = (unsigned char *)gf_malloc(sizeof(char)*(length));
			//WARNING Buffers are NOT ALIGNED IN THE BITSTREAM
			for (i=0; i<length; i++) {
				sfcb->buffer[i] = gf_bs_read_int(bs, 8);
			}
		}
		//notify the node - this is needed in case an enhencement layer replaces the buffer, in which case
		//the # ID Bits may change
		SFCommandBufferChanged(codec, node);

		/*
		 1 - memory mode, register command buffer for later parsing
		 2 - InputSensor only works on decompressed commands
		*/
		if (codec->dec_memory_mode || (node->sgprivate->tag==TAG_MPEG4_InputSensor)) {
			CommandBufferItem *cbi = (CommandBufferItem *)gf_malloc(sizeof(CommandBufferItem));
			cbi->node = node;
			gf_node_register(cbi->node, NULL);
			cbi->cb = sfcb;
			gf_list_add(codec->command_buffers, cbi);
		}
	}
	break;
	case GF_SG_VRML_SFNODE:
		//if not memory dec mode, unregister previous node
		//otherwise the field points to the memory command internal field
		if (!is_mem_com) {
			GF_Node *old_node = *((GF_Node **) field->far_ptr);
			if (old_node != NULL) {
				u32 i, count = gf_list_count(codec->command_buffers);
				for (i=0; i<count; i++) {
					CommandBufferItem *cbi = (CommandBufferItem*) gf_list_get(codec->command_buffers, i);
					if (cbi->node == old_node) {
						gf_list_rem(codec->command_buffers, i);
						i--;
						count--;
						gf_node_unregister(cbi->node, NULL);
						gf_free(cbi);
					}
				}
				gf_node_unregister(old_node, node);
				 *((GF_Node **) field->far_ptr) = NULL;
			}
		}
		//for nodes the field ptr is a ptr to the field, which is a node ptr ;)
		new_node = gf_bifs_dec_node(codec, bs, field->NDTtype);
		if (new_node) {
			e = gf_node_register(new_node, is_mem_com ? NULL : node);
			if (e) return e;
		}
		//it may happen that new_node is NULL (this is valid for a proto declaration)
		*((GF_Node **) field->far_ptr) = new_node;
		break;
	case GF_SG_VRML_SFSCRIPT:
#ifdef GPAC_HAS_QJS
		codec->LastError = node ? SFScript_Parse(codec, (SFScript*)field->far_ptr, bs, node) : GF_NON_COMPLIANT_BITSTREAM;
#else
		return GF_NOT_SUPPORTED;
#endif
		break;
	case GF_SG_VRML_SFATTRREF:
	{
		SFAttrRef *ar = (SFAttrRef *)field->far_ptr;
		u32 nodeID = 1 + gf_bs_read_int(bs, codec->info->config.NodeIDBits);
		ar->node = gf_sg_find_node(codec->current_graph, nodeID);
		if (!ar->node) {

		} else {
			u32 nbBitsDEF = gf_get_bit_size(gf_node_get_num_fields_in_mode(ar->node, GF_SG_FIELD_CODING_DEF) - 1);
			u32 field_ref = gf_bs_read_int(bs, nbBitsDEF);
			codec->LastError = gf_bifs_get_field_index(ar->node, field_ref, GF_SG_FIELD_CODING_DEF, &ar->fieldIndex);
		}
	}
	break;
	default:
		return GF_NON_COMPLIANT_BITSTREAM;
	}
	return codec->LastError;
}