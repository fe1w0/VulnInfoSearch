static bool load_buffer(RBinFile *bf, void **bin_obj, RBuffer *buf, ut64 loadaddr, Sdb *sdb) {
	QnxObj *qo = R_NEW0 (QnxObj);
	if (!qo) {
		return false;
	}
	lmf_record lrec;
	lmf_resource lres;
	lmf_data ldata;
	ut64 offset = QNX_RECORD_SIZE;
	RList *sections = NULL;
	RList *fixups = NULL;

	if (!qo) {
		goto beach;
	}
	if (!(sections = r_list_newf ((RListFree)r_bin_section_free)) || !(fixups = r_list_new ())) {
		goto beach;
	}
	qo->kv = sdb_new0 ();
	if (!qo->kv) {
		goto beach;
	}
	// Read the first record
	if (r_buf_fread_at (bf->buf, 0, (ut8 *)&lrec, "ccss", 1) < QNX_RECORD_SIZE) {
		goto beach;
	}
	// Load the header
	lmf_header_load (&qo->lmfh, bf->buf, qo->kv);
	offset += lrec.data_nbytes;

	for (;;) {
		if (r_buf_fread_at (bf->buf, offset, (ut8 *)&lrec, "ccss", 1) < QNX_RECORD_SIZE) {
			goto beach;
		}
		offset += sizeof (lmf_record);

		if (lrec.rec_type == LMF_IMAGE_END_REC) {
			break;
		} else if (lrec.rec_type == LMF_RESOURCE_REC) {
			RBinSection *ptr = R_NEW0 (RBinSection);
			if (!ptr) {
				goto beach;
			}
			if (r_buf_fread_at (bf->buf, offset, (ut8 *)&lres, "ssss", 1) < sizeof (lmf_resource)) {
				goto beach;
			}
			ptr->name = strdup ("LMF_RESOURCE");
			ptr->paddr = offset;
			ptr->vsize = lrec.data_nbytes - sizeof (lmf_resource);
			ptr->size = ptr->vsize;
			ptr->add = true;
		 	r_list_append (sections, ptr);
		} else if (lrec.rec_type == LMF_LOAD_REC) {
			RBinSection *ptr = R_NEW0 (RBinSection);
			if (r_buf_fread_at (bf->buf, offset, (ut8 *)&ldata, "si", 1) < sizeof (lmf_data)) {
				goto beach;
			}
			if (!ptr) {
				goto beach;
			}
			ptr->name = strdup ("LMF_LOAD");
			ptr->paddr = offset;
			ptr->vaddr = ldata.offset;
			ptr->vsize = lrec.data_nbytes - sizeof (lmf_data);
			ptr->size = ptr->vsize;
			ptr->add = true;
		 	r_list_append (sections, ptr);
		} else if (lrec.rec_type == LMF_FIXUP_REC) {
			RBinReloc *ptr = R_NEW0 (RBinReloc);
			if (!ptr || r_buf_fread_at (bf->buf, offset, (ut8 *)&ldata, "si", 1) < sizeof (lmf_data)) {
				goto beach;
			}
			ptr->vaddr = ptr->paddr = ldata.offset;
			ptr->type = 'f'; // "LMF_FIXUP";
			r_list_append (fixups, ptr);
		} else if (lrec.rec_type == LMF_8087_FIXUP_REC) {
			RBinReloc *ptr = R_NEW0 (RBinReloc);
			if (!ptr || r_buf_fread_at (bf->buf, offset, (ut8 *)&ldata, "si", 1) < sizeof (lmf_data)) {
				goto beach;
			}
			ptr->vaddr = ptr->paddr = ldata.offset;
			ptr->type = 'F'; // "LMF_8087_FIXUP";
			r_list_append (fixups, ptr);
		} else if (lrec.rec_type == LMF_RW_END_REC) {
			r_buf_fread_at (bf->buf, offset, (ut8 *)&qo->rwend, "si", 1);
		}
		offset += lrec.data_nbytes;
	}
	sdb_ns_set (sdb, "info", qo->kv);
	qo->sections = sections;
	qo->fixups = fixups;
	*bin_obj = qo;
	return true;
beach:
	free (qo);
	r_list_free (fixups);
	r_list_free (sections);
	return false;
}