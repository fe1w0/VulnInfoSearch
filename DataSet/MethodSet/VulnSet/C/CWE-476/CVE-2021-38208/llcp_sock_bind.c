static int llcp_sock_bind(struct socket *sock, struct sockaddr *addr, int alen)
{
	struct sock *sk = sock->sk;
	struct nfc_llcp_sock *llcp_sock = nfc_llcp_sock(sk);
	struct nfc_llcp_local *local;
	struct nfc_dev *dev;
	struct sockaddr_nfc_llcp llcp_addr;
	int len, ret = 0;

	if (!addr || alen < offsetofend(struct sockaddr, sa_family) ||
	    addr->sa_family != AF_NFC)
		return -EINVAL;

	pr_debug("sk %p addr %p family %d\n", sk, addr, addr->sa_family);

	memset(&llcp_addr, 0, sizeof(llcp_addr));
	len = min_t(unsigned int, sizeof(llcp_addr), alen);
	memcpy(&llcp_addr, addr, len);

	/* This is going to be a listening socket, dsap must be 0 */
	if (llcp_addr.dsap != 0)
		return -EINVAL;

	lock_sock(sk);

	if (sk->sk_state != LLCP_CLOSED) {
		ret = -EBADFD;
		goto error;
	}

	dev = nfc_get_device(llcp_addr.dev_idx);
	if (dev == NULL) {
		ret = -ENODEV;
		goto error;
	}

	local = nfc_llcp_find_local(dev);
	if (local == NULL) {
		ret = -ENODEV;
		goto put_dev;
	}

	llcp_sock->dev = dev;
	llcp_sock->local = nfc_llcp_local_get(local);
	llcp_sock->nfc_protocol = llcp_addr.nfc_protocol;
	llcp_sock->service_name_len = min_t(unsigned int,
					    llcp_addr.service_name_len,
					    NFC_LLCP_MAX_SERVICE_NAME);
	llcp_sock->service_name = kmemdup(llcp_addr.service_name,
					  llcp_sock->service_name_len,
					  GFP_KERNEL);
	if (!llcp_sock->service_name) {
		nfc_llcp_local_put(llcp_sock->local);
		llcp_sock->local = NULL;
		ret = -ENOMEM;
		goto put_dev;
	}
	llcp_sock->ssap = nfc_llcp_get_sdp_ssap(local, llcp_sock);
	if (llcp_sock->ssap == LLCP_SAP_MAX) {
		nfc_llcp_local_put(llcp_sock->local);
		llcp_sock->local = NULL;
		kfree(llcp_sock->service_name);
		llcp_sock->service_name = NULL;
		ret = -EADDRINUSE;
		goto put_dev;
	}

	llcp_sock->reserved_ssap = llcp_sock->ssap;

	nfc_llcp_sock_link(&local->sockets, sk);

	pr_debug("Socket bound to SAP %d\n", llcp_sock->ssap);

	sk->sk_state = LLCP_BOUND;

put_dev:
	nfc_put_device(dev);

error:
	release_sock(sk);
	return ret;
}