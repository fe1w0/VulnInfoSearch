static void sctp_skb_set_owner_r_frag(struct sk_buff *skb, struct sock *sk)
{
	struct sk_buff *frag;

	if (!skb->data_len)
		goto done;

	/* Don't forget the fragments. */
	for (frag = skb_shinfo(skb)->frag_list; frag; frag = frag->next)
		sctp_skb_set_owner_r_frag(frag, sk);

done:
	sctp_skb_set_owner_r(skb, sk);
}