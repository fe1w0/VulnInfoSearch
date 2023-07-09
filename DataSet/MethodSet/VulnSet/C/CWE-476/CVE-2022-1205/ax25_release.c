static int ax25_release(struct socket *sock)
{
	struct sock *sk = sock->sk;
	ax25_cb *ax25;
	ax25_dev *ax25_dev;

	if (sk == NULL)
		return 0;

	sock_hold(sk);
	lock_sock(sk);
	sock_orphan(sk);
	ax25 = sk_to_ax25(sk);
	ax25_dev = ax25->ax25_dev;

	if (sk->sk_type == SOCK_SEQPACKET) {
		switch (ax25->state) {
		case AX25_STATE_0:
			release_sock(sk);
			ax25_disconnect(ax25, 0);
			lock_sock(sk);
			ax25_destroy_socket(ax25);
			break;

		case AX25_STATE_1:
		case AX25_STATE_2:
			ax25_send_control(ax25, AX25_DISC, AX25_POLLON, AX25_COMMAND);
			release_sock(sk);
			ax25_disconnect(ax25, 0);
			lock_sock(sk);
			if (!sock_flag(ax25->sk, SOCK_DESTROY))
				ax25_destroy_socket(ax25);
			break;

		case AX25_STATE_3:
		case AX25_STATE_4:
			ax25_clear_queues(ax25);
			ax25->n2count = 0;

			switch (ax25->ax25_dev->values[AX25_VALUES_PROTOCOL]) {
			case AX25_PROTO_STD_SIMPLEX:
			case AX25_PROTO_STD_DUPLEX:
				ax25_send_control(ax25,
						  AX25_DISC,
						  AX25_POLLON,
						  AX25_COMMAND);
				ax25_stop_t2timer(ax25);
				ax25_stop_t3timer(ax25);
				ax25_stop_idletimer(ax25);
				break;
#ifdef CONFIG_AX25_DAMA_SLAVE
			case AX25_PROTO_DAMA_SLAVE:
				ax25_stop_t3timer(ax25);
				ax25_stop_idletimer(ax25);
				break;
#endif
			}
			ax25_calculate_t1(ax25);
			ax25_start_t1timer(ax25);
			ax25->state = AX25_STATE_2;
			sk->sk_state                = TCP_CLOSE;
			sk->sk_shutdown            |= SEND_SHUTDOWN;
			sk->sk_state_change(sk);
			sock_set_flag(sk, SOCK_DESTROY);
			break;

		default:
			break;
		}
	} else {
		sk->sk_state     = TCP_CLOSE;
		sk->sk_shutdown |= SEND_SHUTDOWN;
		sk->sk_state_change(sk);
		ax25_destroy_socket(ax25);
	}
	if (ax25_dev) {
		dev_put_track(ax25_dev->dev, &ax25_dev->dev_tracker);
		ax25_dev_put(ax25_dev);
	}

	sock->sk   = NULL;
	release_sock(sk);
	sock_put(sk);

	return 0;
}