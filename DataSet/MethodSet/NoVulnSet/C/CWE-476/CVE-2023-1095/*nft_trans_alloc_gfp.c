static struct nft_trans *nft_trans_alloc_gfp(const struct nft_ctx *ctx,
					     int msg_type, u32 size, gfp_t gfp)
{
	struct nft_trans *trans;

	trans = kzalloc(sizeof(struct nft_trans) + size, gfp);
	if (trans == NULL)
		return NULL;

	INIT_LIST_HEAD(&trans->list);
	trans->msg_type = msg_type;
	trans->ctx	= *ctx;

	return trans;
}