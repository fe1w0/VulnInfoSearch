MOBI_RET mobi_build_opf_metadata(OPF *opf,  const MOBIData *m, const MOBIRawml *rawml) {
    if (m == NULL) {
        debug_print("%s\n", "Initialization failed");
        return MOBI_INIT_FAILED;
    }
    opf->metadata = calloc(1, sizeof(OPFmetadata));
    if (opf->metadata == NULL) {
        debug_print("%s\n", "Memory allocation failed");
        return MOBI_MALLOC_FAILED;
    }
    /* initialize metadata sub-elements */
    opf->metadata->meta = calloc(OPF_META_MAX_TAGS, sizeof(OPFmeta*));
    if (opf->metadata->meta == NULL) {
        debug_print("%s\n", "Memory allocation failed");
        return MOBI_MALLOC_FAILED;
    }
    opf->metadata->dc_meta = calloc(1, sizeof(OPFdcmeta));
    if (opf->metadata->dc_meta == NULL) {
        debug_print("%s\n", "Memory allocation failed");
        return MOBI_MALLOC_FAILED;
    }
    opf->metadata->x_meta = calloc(1, sizeof(OPFxmeta));
    if (opf->metadata->x_meta == NULL) {
        debug_print("%s\n", "Memory allocation failed");
        return MOBI_MALLOC_FAILED;
    }
    if (m->eh) {
        MOBI_RET ret = mobi_get_opf_from_exth(opf->metadata, m);
        if (ret != MOBI_SUCCESS) {
            return ret;
        }
    }
    /* check for required elements */
    if (opf->metadata->dc_meta->identifier == NULL) {
        /* default id will be "0" */
        char uid_string[11] = "0";
        if (m->mh && m->mh->uid) {
            snprintf(uid_string, 11, "%u", *m->mh->uid);
        }
        mobi_opf_set_tagtype(OPFidentifier, opf->metadata->dc_meta->identifier, value, uid_string);
        mobi_opf_set_tagtype(OPFidentifier, opf->metadata->dc_meta->identifier, id, "uid");
    } else {
        opf->metadata->dc_meta->identifier[0]->id = strdup("uid");
    }
    if (opf->metadata->dc_meta->title == NULL) {
        opf->metadata->dc_meta->title = calloc(OPF_META_MAX_TAGS, sizeof(char*));
        if (opf->metadata->dc_meta->title == NULL) {
            debug_print("%s\n", "Memory allocation failed");
            return MOBI_MALLOC_FAILED;
        }
        char *title = mobi_meta_get_title(m);
        if (title == NULL) {
            title = strdup("Unknown");
        }
        opf->metadata->dc_meta->title[0] = title;
    }
    if (opf->metadata->dc_meta->language == NULL) {
        opf->metadata->dc_meta->language = calloc(OPF_META_MAX_TAGS, sizeof(char*));
        if (opf->metadata->dc_meta->language == NULL) {
            debug_print("%s\n", "Memory allocation failed");
            return MOBI_MALLOC_FAILED;
        }
        const char *lang_string = NULL;
        if (m->mh && m->mh->locale) {
            uint32_t lang_code = *m->mh->locale;
            lang_string = mobi_get_locale_string(lang_code);
        }
        if (lang_string) {
            opf->metadata->dc_meta->language[0] = strdup(lang_string);
        } else {
            opf->metadata->dc_meta->language[0] = strdup("en");
        }
    }
    /* write optional elements */
    if (mobi_is_dictionary(m)) {
        if (opf->metadata->x_meta->dictionary_in_lang == NULL) {
            if (m->mh && m->mh->dict_input_lang) {
                opf->metadata->x_meta->dictionary_in_lang = calloc(OPF_META_MAX_TAGS, sizeof(char*));
                if (opf->metadata->x_meta->dictionary_in_lang == NULL) {
                    debug_print("%s\n", "Memory allocation failed");
                    return MOBI_MALLOC_FAILED;
                }
                uint32_t dict_lang_in = *m->mh->dict_input_lang;
                opf->metadata->x_meta->dictionary_in_lang[0] = strdup(mobi_get_locale_string(dict_lang_in));
            }
        }
        if (opf->metadata->x_meta->dictionary_out_lang == NULL) {
            if (m->mh && m->mh->dict_output_lang) {
                opf->metadata->x_meta->dictionary_out_lang = calloc(OPF_META_MAX_TAGS, sizeof(char*));
                if (opf->metadata->x_meta->dictionary_out_lang == NULL) {
                    debug_print("%s\n", "Memory allocation failed");
                    return MOBI_MALLOC_FAILED;
                }
                uint32_t dict_lang_in = *m->mh->dict_output_lang;
                opf->metadata->x_meta->dictionary_out_lang[0] = strdup(mobi_get_locale_string(dict_lang_in));
            }
        }
        if (rawml->orth->orth_index_name) {
            opf->metadata->x_meta->default_lookup_index = calloc(OPF_META_MAX_TAGS, sizeof(char*));
            if (opf->metadata->x_meta->default_lookup_index == NULL) {
                debug_print("%s\n", "Memory allocation failed");
                return MOBI_MALLOC_FAILED;
            }
            opf->metadata->x_meta->default_lookup_index[0] = strdup(rawml->orth->orth_index_name);
        }
    }
    return MOBI_SUCCESS;
}