static int xfrm_dump_policy_start(struct netlink_callback *cb)
{
	struct xfrm_policy_walk *walk = (struct xfrm_policy_walk *)cb->args;

	BUILD_BUG_ON(sizeof(*walk) > sizeof(cb->args));

	xfrm_policy_walk_init(walk, XFRM_POLICY_TYPE_ANY);
	return 0;
}