void print_summary(const MOBIData *m) {
    char *title = mobi_meta_get_title(m);
    if (title) {
        printf("Title: %s\n", title);
        free(title);
    }
    char *author = mobi_meta_get_author(m);
    if (author) {
        printf("Author: %s\n", author);
        free(author);
    }
    char *contributor = mobi_meta_get_contributor(m);
    uint32_t major = 0, minor = 0, build = 0;
    bool is_calibre = false;
    if (contributor) {
        const char *calibre_contributor = "calibre (";
        if (strncmp(contributor, calibre_contributor, strlen(calibre_contributor)) == 0) {
            is_calibre = true;
            sscanf(contributor, "calibre (%u.%u.%u)", &major, &minor, &build);
        } else {
            printf("Contributor: %s\n", contributor);
        }
        free(contributor);
    }
    char *subject = mobi_meta_get_subject(m);
    if (subject) {
        printf("Subject: %s\n", subject);
        free(subject);
    }
    char *publisher = mobi_meta_get_publisher(m);
    if (publisher) {
        printf("Publisher: %s\n", publisher);
        free(publisher);
    }
    char *date = mobi_meta_get_publishdate(m);
    if (date) {
        printf("Publishing date: %s\n", date);
        free(date);
    }
    char *description = mobi_meta_get_description(m);
    if (description) {
        printf("Description: %s\n", description);
        free(description);
    }
    char *review = mobi_meta_get_review(m);
    if (review) {
        printf("Review: %s\n", review);
        free(review);
    }
    char *imprint = mobi_meta_get_imprint(m);
    if (imprint) {
        printf("Imprint: %s\n", imprint);
        free(imprint);
    }
    char *copyright = mobi_meta_get_copyright(m);
    if (copyright) {
        printf("Copyright: %s\n", copyright);
        free(copyright);
    }
    char *isbn = mobi_meta_get_isbn(m);
    if (isbn) {
        printf("ISBN: %s\n", isbn);
        free(isbn);
    }
    char *asin = mobi_meta_get_asin(m);
    if (asin) {
        printf("ASIN: %s\n", asin);
        free(asin);
    }
    char *language = mobi_meta_get_language(m);
    if (language) {
        printf("Language: %s", language);
        free(language);
        if (m->mh && m->mh->text_encoding) {
            uint32_t encoding = *m->mh->text_encoding;
            if (encoding == MOBI_CP1252) {
                printf(" (cp1252)");
            } else if (encoding == MOBI_UTF8) {
                printf(" (utf8)");
            }
        }
        printf("\n");
    }
    if (mobi_is_dictionary(m)) {
        printf("Dictionary");
        if (m->mh && m->mh->dict_input_lang && m->mh->dict_output_lang &&
            *m->mh->dict_input_lang && *m->mh->dict_output_lang) {
            const char *locale_in = mobi_get_locale_string(*m->mh->dict_input_lang);
            const char *locale_out = mobi_get_locale_string(*m->mh->dict_output_lang);
            printf(": %s => %s", locale_in, locale_out);
        }
        printf("\n");
    }
    printf("__\n");
    if (strcmp(m->ph->type, "TEXt") == 0) {
        if (strcmp(m->ph->creator, "TlDc") == 0) {
            printf("TealDoc\n");
        } else {
            printf("PalmDoc\n");
        }
    } else {
        printf("Mobi version: %zu", mobi_get_fileversion(m));
        if (mobi_is_hybrid(m)) {
            size_t version = mobi_get_fileversion(m->next);
            if (version != MOBI_NOTSET) {
                printf(" (hybrid with version %zu)", version);
            }
        }
        printf("\n");
    }
    if (mobi_is_replica(m)) {
        printf("Print Replica\n");
    }
    if (mobi_is_encrypted(m)) {
        printf("Document is encrypted\n");
    }
    if (is_calibre) {
        printf("Creator software: calibre %u.%u.%u\n", major, minor, build);
    } else {
        MOBIExthHeader *exth = mobi_get_exthrecord_by_tag(m, EXTH_CREATORSOFT);
        if (exth) {
            printf("Creator software: ");
            uint32_t creator = mobi_decode_exthvalue(exth->data, exth->size);
            exth = mobi_get_exthrecord_by_tag(m, EXTH_CREATORMAJOR);
            if (exth) {
                major = mobi_decode_exthvalue(exth->data, exth->size);
            }
            exth = mobi_get_exthrecord_by_tag(m, EXTH_CREATORMINOR);
            if (exth) {
                minor = mobi_decode_exthvalue(exth->data, exth->size);
            }
            exth = mobi_get_exthrecord_by_tag(m, EXTH_CREATORBUILD);
            if (exth) {
                build = mobi_decode_exthvalue(exth->data, exth->size);
            }
            exth = mobi_get_exthrecord_by_tag(m, EXTH_CREATORBUILDREV);
            if (major == 2 && minor == 9 && build == 0 && exth) {
                char *rev = mobi_decode_exthstring(m, exth->data, exth->size);
                if (rev) {
                    if (strcmp(rev, "0730-890adc2") == 0) {
                        is_calibre = true;
                    }
                    free(rev);
                }
            }
            switch (creator) {
                case 0:
                    printf("mobipocket reader %u.%u.%u", major, minor, build);
                    break;
                case 1:
                case 101:
                    printf("mobigen %u.%u.%u", major, minor, build);
                    break;
                case 2:
                    printf("mobipocket creator %u.%u.%u", major, minor, build);
                    break;
                case 200:
                    printf("kindlegen %u.%u.%u (windows)", major, minor, build);
                    if (is_calibre) {
                        printf(" or calibre");
                    }
                    break;
                case 201:
                    printf("kindlegen %u.%u.%u (linux)", major, minor, build);
                    if ((major == 1 && minor == 2 && build == 33307) ||
                        (major == 2 && minor == 0 && build == 101) ||
                        is_calibre) {
                        printf(" or calibre");
                    }
                    break;
                case 202:
                    printf("kindlegen %u.%u.%u (mac)", major, minor, build);
                    if (is_calibre) {
                        printf(" or calibre");
                    }
                    break;
                default:
                    printf("unknown");
                    break;
            }
            printf("\n");
        }
    }
}