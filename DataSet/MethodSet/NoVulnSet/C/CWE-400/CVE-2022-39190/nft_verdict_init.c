static int nft_verdict_init(const struct nft_ctx *ctx, struct nft_data *data,
			    struct nft_data_desc *desc, const struct nlattr *nla)
{
	u8 genmask = nft_genmask_next(ctx->net);
	struct nlattr *tb[NFTA_VERDICT_MAX + 1];
	struct nft_chain *chain;
	int err;

	err = nla_parse_nested_deprecated(tb, NFTA_VERDICT_MAX, nla,
					  nft_verdict_policy, NULL);
	if (err < 0)
		return err;

	if (!tb[NFTA_VERDICT_CODE])
		return -EINVAL;
	data->verdict.code = ntohl(nla_get_be32(tb[NFTA_VERDICT_CODE]));

	switch (data->verdict.code) {
	default:
		switch (data->verdict.code & NF_VERDICT_MASK) {
		case NF_ACCEPT:
		case NF_DROP:
		case NF_QUEUE:
			break;
		default:
			return -EINVAL;
		}
		fallthrough;
	case NFT_CONTINUE:
	case NFT_BREAK:
	case NFT_RETURN:
		break;
	case NFT_JUMP:
	case NFT_GOTO:
		if (tb[NFTA_VERDICT_CHAIN]) {
			chain = nft_chain_lookup(ctx->net, ctx->table,
						 tb[NFTA_VERDICT_CHAIN],
						 genmask);
		} else if (tb[NFTA_VERDICT_CHAIN_ID]) {
			chain = nft_chain_lookup_byid(ctx->net, ctx->table,
						      tb[NFTA_VERDICT_CHAIN_ID]);
			if (IS_ERR(chain))
				return PTR_ERR(chain);
		} else {
			return -EINVAL;
		}

		if (IS_ERR(chain))
			return PTR_ERR(chain);
		if (nft_is_base_chain(chain))
			return -EOPNOTSUPP;
		if (nft_chain_is_bound(chain))
			return -EINVAL;
		if (desc->flags & NFT_DATA_DESC_SETELEM &&
		    chain->flags & NFT_CHAIN_BINDING)
			return -EINVAL;

		chain->use++;
		data->verdict.chain = chain;
		break;
	}

	desc->len = sizeof(data->verdict);

	return 0;
}