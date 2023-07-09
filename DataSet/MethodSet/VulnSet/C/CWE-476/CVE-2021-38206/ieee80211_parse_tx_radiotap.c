bool ieee80211_parse_tx_radiotap(struct sk_buff *skb,
				 struct net_device *dev)
{
	struct ieee80211_local *local = wdev_priv(dev->ieee80211_ptr);
	struct ieee80211_radiotap_iterator iterator;
	struct ieee80211_radiotap_header *rthdr =
		(struct ieee80211_radiotap_header *) skb->data;
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
	struct ieee80211_supported_band *sband =
		local->hw.wiphy->bands[info->band];
	int ret = ieee80211_radiotap_iterator_init(&iterator, rthdr, skb->len,
						   NULL);
	u16 txflags;
	u16 rate = 0;
	bool rate_found = false;
	u8 rate_retries = 0;
	u16 rate_flags = 0;
	u8 mcs_known, mcs_flags, mcs_bw;
	u16 vht_known;
	u8 vht_mcs = 0, vht_nss = 0;
	int i;

	/* check for not even having the fixed radiotap header part */
	if (unlikely(skb->len < sizeof(struct ieee80211_radiotap_header)))
		return false; /* too short to be possibly valid */

	/* is it a header version we can trust to find length from? */
	if (unlikely(rthdr->it_version))
		return false; /* only version 0 is supported */

	/* does the skb contain enough to deliver on the alleged length? */
	if (unlikely(skb->len < ieee80211_get_radiotap_len(skb->data)))
		return false; /* skb too short for claimed rt header extent */

	info->flags |= IEEE80211_TX_INTFL_DONT_ENCRYPT |
		       IEEE80211_TX_CTL_DONTFRAG;

	/*
	 * for every radiotap entry that is present
	 * (ieee80211_radiotap_iterator_next returns -ENOENT when no more
	 * entries present, or -EINVAL on error)
	 */

	while (!ret) {
		ret = ieee80211_radiotap_iterator_next(&iterator);

		if (ret)
			continue;

		/* see if this argument is something we can use */
		switch (iterator.this_arg_index) {
		/*
		 * You must take care when dereferencing iterator.this_arg
		 * for multibyte types... the pointer is not aligned.  Use
		 * get_unaligned((type *)iterator.this_arg) to dereference
		 * iterator.this_arg for type "type" safely on all arches.
		*/
		case IEEE80211_RADIOTAP_FLAGS:
			if (*iterator.this_arg & IEEE80211_RADIOTAP_F_FCS) {
				/*
				 * this indicates that the skb we have been
				 * handed has the 32-bit FCS CRC at the end...
				 * we should react to that by snipping it off
				 * because it will be recomputed and added
				 * on transmission
				 */
				if (skb->len < (iterator._max_length + FCS_LEN))
					return false;

				skb_trim(skb, skb->len - FCS_LEN);
			}
			if (*iterator.this_arg & IEEE80211_RADIOTAP_F_WEP)
				info->flags &= ~IEEE80211_TX_INTFL_DONT_ENCRYPT;
			if (*iterator.this_arg & IEEE80211_RADIOTAP_F_FRAG)
				info->flags &= ~IEEE80211_TX_CTL_DONTFRAG;
			break;

		case IEEE80211_RADIOTAP_TX_FLAGS:
			txflags = get_unaligned_le16(iterator.this_arg);
			if (txflags & IEEE80211_RADIOTAP_F_TX_NOACK)
				info->flags |= IEEE80211_TX_CTL_NO_ACK;
			if (txflags & IEEE80211_RADIOTAP_F_TX_NOSEQNO)
				info->control.flags |= IEEE80211_TX_CTRL_NO_SEQNO;
			if (txflags & IEEE80211_RADIOTAP_F_TX_ORDER)
				info->control.flags |=
					IEEE80211_TX_CTRL_DONT_REORDER;
			break;

		case IEEE80211_RADIOTAP_RATE:
			rate = *iterator.this_arg;
			rate_flags = 0;
			rate_found = true;
			break;

		case IEEE80211_RADIOTAP_DATA_RETRIES:
			rate_retries = *iterator.this_arg;
			break;

		case IEEE80211_RADIOTAP_MCS:
			mcs_known = iterator.this_arg[0];
			mcs_flags = iterator.this_arg[1];
			if (!(mcs_known & IEEE80211_RADIOTAP_MCS_HAVE_MCS))
				break;

			rate_found = true;
			rate = iterator.this_arg[2];
			rate_flags = IEEE80211_TX_RC_MCS;

			if (mcs_known & IEEE80211_RADIOTAP_MCS_HAVE_GI &&
			    mcs_flags & IEEE80211_RADIOTAP_MCS_SGI)
				rate_flags |= IEEE80211_TX_RC_SHORT_GI;

			mcs_bw = mcs_flags & IEEE80211_RADIOTAP_MCS_BW_MASK;
			if (mcs_known & IEEE80211_RADIOTAP_MCS_HAVE_BW &&
			    mcs_bw == IEEE80211_RADIOTAP_MCS_BW_40)
				rate_flags |= IEEE80211_TX_RC_40_MHZ_WIDTH;

			if (mcs_known & IEEE80211_RADIOTAP_MCS_HAVE_FEC &&
			    mcs_flags & IEEE80211_RADIOTAP_MCS_FEC_LDPC)
				info->flags |= IEEE80211_TX_CTL_LDPC;

			if (mcs_known & IEEE80211_RADIOTAP_MCS_HAVE_STBC) {
				u8 stbc = u8_get_bits(mcs_flags,
						      IEEE80211_RADIOTAP_MCS_STBC_MASK);

				info->flags |=
					u32_encode_bits(stbc,
							IEEE80211_TX_CTL_STBC);
			}
			break;

		case IEEE80211_RADIOTAP_VHT:
			vht_known = get_unaligned_le16(iterator.this_arg);
			rate_found = true;

			rate_flags = IEEE80211_TX_RC_VHT_MCS;
			if ((vht_known & IEEE80211_RADIOTAP_VHT_KNOWN_GI) &&
			    (iterator.this_arg[2] &
			     IEEE80211_RADIOTAP_VHT_FLAG_SGI))
				rate_flags |= IEEE80211_TX_RC_SHORT_GI;
			if (vht_known &
			    IEEE80211_RADIOTAP_VHT_KNOWN_BANDWIDTH) {
				if (iterator.this_arg[3] == 1)
					rate_flags |=
						IEEE80211_TX_RC_40_MHZ_WIDTH;
				else if (iterator.this_arg[3] == 4)
					rate_flags |=
						IEEE80211_TX_RC_80_MHZ_WIDTH;
				else if (iterator.this_arg[3] == 11)
					rate_flags |=
						IEEE80211_TX_RC_160_MHZ_WIDTH;
			}

			vht_mcs = iterator.this_arg[4] >> 4;
			vht_nss = iterator.this_arg[4] & 0xF;
			break;

		/*
		 * Please update the file
		 * Documentation/networking/mac80211-injection.rst
		 * when parsing new fields here.
		 */

		default:
			break;
		}
	}

	if (ret != -ENOENT) /* ie, if we didn't simply run out of fields */
		return false;

	if (rate_found) {
		info->control.flags |= IEEE80211_TX_CTRL_RATE_INJECT;

		for (i = 0; i < IEEE80211_TX_MAX_RATES; i++) {
			info->control.rates[i].idx = -1;
			info->control.rates[i].flags = 0;
			info->control.rates[i].count = 0;
		}

		if (rate_flags & IEEE80211_TX_RC_MCS) {
			info->control.rates[0].idx = rate;
		} else if (rate_flags & IEEE80211_TX_RC_VHT_MCS) {
			ieee80211_rate_set_vht(info->control.rates, vht_mcs,
					       vht_nss);
		} else {
			for (i = 0; i < sband->n_bitrates; i++) {
				if (rate * 5 != sband->bitrates[i].bitrate)
					continue;

				info->control.rates[0].idx = i;
				break;
			}
		}

		if (info->control.rates[0].idx < 0)
			info->control.flags &= ~IEEE80211_TX_CTRL_RATE_INJECT;

		info->control.rates[0].flags = rate_flags;
		info->control.rates[0].count = min_t(u8, rate_retries + 1,
						     local->hw.max_rate_tries);
	}

	return true;
}