void ax25_disconnect(ax25_cb *ax25, int reason)
{
	ax25_clear_queues(ax25);

	if (!ax25->sk || !sock_flag(ax25->sk, SOCK_DESTROY))
		ax25_stop_heartbeat(ax25);
	ax25_stop_t1timer(ax25);
	ax25_stop_t2timer(ax25);
	ax25_stop_t3timer(ax25);
	ax25_stop_idletimer(ax25);

	ax25->state = AX25_STATE_0;

	ax25_link_failed(ax25, reason);

	if (ax25->sk != NULL) {
		local_bh_disable();
		bh_lock_sock(ax25->sk);
		ax25->sk->sk_state     = TCP_CLOSE;
		ax25->sk->sk_err       = reason;
		ax25->sk->sk_shutdown |= SEND_SHUTDOWN;
		if (!sock_flag(ax25->sk, SOCK_DEAD)) {
			ax25->sk->sk_state_change(ax25->sk);
			sock_set_flag(ax25->sk, SOCK_DEAD);
		}
		bh_unlock_sock(ax25->sk);
		local_bh_enable();
	}
}