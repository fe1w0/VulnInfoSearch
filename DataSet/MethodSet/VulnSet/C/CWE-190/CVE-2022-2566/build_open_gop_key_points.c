static int build_open_gop_key_points(AVStream *st)
{
    int k;
    int sample_id = 0;
    uint32_t cra_index;
    MOVStreamContext *sc = st->priv_data;

    if (st->codecpar->codec_id != AV_CODEC_ID_HEVC || !sc->sync_group_count)
        return 0;

    /* Build an unrolled index of the samples */
    sc->sample_offsets_count = 0;
    for (uint32_t i = 0; i < sc->ctts_count; i++)
        sc->sample_offsets_count += sc->ctts_data[i].count;
    av_freep(&sc->sample_offsets);
    sc->sample_offsets = av_calloc(sc->sample_offsets_count, sizeof(*sc->sample_offsets));
    if (!sc->sample_offsets)
        return AVERROR(ENOMEM);
    k = 0;
    for (uint32_t i = 0; i < sc->ctts_count; i++)
        for (int j = 0; j < sc->ctts_data[i].count; j++)
             sc->sample_offsets[k++] = sc->ctts_data[i].duration;

    /* The following HEVC NAL type reveal the use of open GOP sync points
     * (TODO: BLA types may also be concerned) */
    cra_index = get_sgpd_sync_index(sc, HEVC_NAL_CRA_NUT); /* Clean Random Access */
    if (!cra_index)
        return 0;

    /* Build a list of open-GOP key samples */
    sc->open_key_samples_count = 0;
    for (uint32_t i = 0; i < sc->sync_group_count; i++)
        if (sc->sync_group[i].index == cra_index)
            sc->open_key_samples_count += sc->sync_group[i].count;
    av_freep(&sc->open_key_samples);
    sc->open_key_samples = av_calloc(sc->open_key_samples_count, sizeof(*sc->open_key_samples));
    if (!sc->open_key_samples)
        return AVERROR(ENOMEM);
    k = 0;
    for (uint32_t i = 0; i < sc->sync_group_count; i++) {
        const MOVSbgp *sg = &sc->sync_group[i];
        if (sg->index == cra_index)
            for (uint32_t j = 0; j < sg->count; j++)
                sc->open_key_samples[k++] = sample_id;
        sample_id += sg->count;
    }

    /* Identify the minimal time step between samples */
    sc->min_sample_duration = UINT_MAX;
    for (uint32_t i = 0; i < sc->stts_count; i++)
        sc->min_sample_duration = FFMIN(sc->min_sample_duration, sc->stts_data[i].duration);

    return 0;
}