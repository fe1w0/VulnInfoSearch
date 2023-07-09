static void sctp_sock_rfree_frag(struct sk_buff *skb)
{
	struct sk_buff *frag;

	if (!skb->data_len)
		goto done;

	/* Don't forget the fragments. */
	for (frag = skb_shinfo(skb)->frag_list; frag; frag = frag->next)
		sctp_sock_rfree_frag(frag);

done:
	sctp_sock_rfree(skb);
}