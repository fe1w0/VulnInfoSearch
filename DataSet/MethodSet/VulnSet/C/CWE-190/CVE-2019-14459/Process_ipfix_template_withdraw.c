static void Process_ipfix_template_withdraw(exporter_ipfix_domain_t *exporter, void *DataPtr, uint32_t size_left, FlowSource_t *fs) {
ipfix_template_record_t *ipfix_template_record;

	// a template flowset can contain multiple records ( templates )
	while ( size_left ) {
		uint32_t id;

		// map next record.
		ipfix_template_record = (ipfix_template_record_t *)DataPtr;
		size_left 		-= 4;

		id 	  = ntohs(ipfix_template_record->TemplateID);
		// count = ntohs(ipfix_template_record->FieldCount);

		if ( id == IPFIX_TEMPLATE_FLOWSET_ID ) {
			// withdraw all templates
			remove_all_translation_tables(exporter);
			ReInitExtensionMapList(fs);
		} else {
			remove_translation_table(fs, exporter, id);
		}

		DataPtr = DataPtr + 4;
		if ( size_left < 4 ) {
			// pading
			dbg_printf("Skip %u bytes padding\n", size_left);
			size_left = 0;
		}
	}
 
} // End of Process_ipfix_template_withdraw