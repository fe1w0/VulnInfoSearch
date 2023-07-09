static int xfrm_dump_policy_done(struct netlink_callback *cb)
{
	struct xfrm_policy_walk *walk = (struct xfrm_policy_walk *) &cb->args[1];

	xfrm_policy_walk_done(walk);
	return 0;
}