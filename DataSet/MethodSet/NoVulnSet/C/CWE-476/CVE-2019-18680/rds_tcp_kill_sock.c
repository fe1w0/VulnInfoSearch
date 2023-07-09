static void rds_tcp_kill_sock(struct net *net)
{
	struct rds_tcp_connection *tc, *_tc;
	struct sock *sk;
	LIST_HEAD(tmp_list);
	struct rds_tcp_net *rtn = net_generic(net, rds_tcp_netid);

	rds_tcp_listen_stop(rtn->rds_tcp_listen_sock);
	rtn->rds_tcp_listen_sock = NULL;
	flush_work(&rtn->rds_tcp_accept_w);
	spin_lock_irq(&rds_tcp_conn_lock);
	list_for_each_entry_safe(tc, _tc, &rds_tcp_conn_list, t_tcp_node) {
		struct net *c_net = read_pnet(&tc->conn->c_net);

		if (net != c_net)
			continue;
		list_move_tail(&tc->t_tcp_node, &tmp_list);
	}
	spin_unlock_irq(&rds_tcp_conn_lock);
	list_for_each_entry_safe(tc, _tc, &tmp_list, t_tcp_node) {
		if (tc->t_sock) {
			sk = tc->t_sock->sk;
			sk->sk_prot->disconnect(sk, 0);
			tcp_done(sk);
		}
		if (tc->conn->c_passive)
			rds_conn_destroy(tc->conn->c_passive);
		rds_conn_destroy(tc->conn);
	}
}