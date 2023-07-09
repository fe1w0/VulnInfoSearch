static inline int copy_regset_from_user(struct task_struct *target,
					const struct user_regset_view *view,
					unsigned int setno,
					unsigned int offset, unsigned int size,
					const void __user *data)
{
	const struct user_regset *regset = &view->regsets[setno];

	if (!access_ok(VERIFY_READ, data, size))
		return -EIO;

	return regset->set(target, regset, offset, size, NULL, data);
}